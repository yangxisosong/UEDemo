#include "Component/DLCharacterMovementComponentBase.h"

#include "DLKit/Public/ConvertString.h"
#include "Engine/Canvas.h"
#include "Engine/ViewportStatsSubsystem.h"
#include "GameplayCoreLog.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "GameplayFramwork/DLCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

void UDLCharacterMovementComponentBase::BeginPlay()
{
	Super::BeginPlay();
}

void UDLCharacterMovementComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

float UDLCharacterMovementComponentBase::GetMaxSpeed() const
{
	if (GetWorld()->WorldType == EWorldType::PIE && GEngine->GetNetMode(GWorld) == NM_Standalone)
	{
		return Super::GetMaxSpeed();
	}


	if (!StrategyInterface.IsAllowMove())
	{
		return 0.f;
	}

	const float MaxSpeed = StrategyInterface.GetMaxSpeedAttribute();

	return MaxSpeed;
}

void UDLCharacterMovementComponentBase::AddInputVector(const FVector WorldVector, const bool bForce)
{
	bool IsMove = false;
	if (!WorldVector.IsNearlyZero())
	{
		if (StrategyInterface.IsAllowMove())
		{
			Super::AddInputVector(WorldVector, bForce);
			IsMove = true;
		}
	}

	bool CanSample = true;

	// 没有移动的 迭代 超过一定次数 才算真的没有移动
	if ((!IsMove) && bIsMoving)
	{
		NoMoveCount++;
		static int32 MaxNoMoveStep = 3;
		if (NoMoveCount < MaxNoMoveStep)
		{
			CanSample = false;
		}
	}
	else
	{
		NoMoveCount = 0;
	}

	if (bIsMoving != IsMove && CanSample)
	{
		bIsMoving = IsMove;
		this->OnMoveStateChange.Broadcast(bIsMoving);
	}
}


static constexpr float TurnToComplateErrorValue = 0.05f;

void UDLCharacterMovementComponentBase::PhysicsRotation(float DeltaTime)
{
	const auto& TurnToInfo = GetTurnToInfo();
	// 如果发生自定义的转向
	if (!TurnToInfo.bIsTurnToComplate)
	{
		FRotator NewRotation = FRotator::ZeroRotator;

		const auto& MyRotation = UpdatedComponent->GetComponentRotation();

		if (TurnToInfo.bImmediatelyTurn)
		{
			NewRotation = TurnToInfo.TargetTurnToRotation;
		}
		else
		{
			NewRotation = UKismetMathLibrary::RInterpTo_Constant(MyRotation, TurnToInfo.TargetTurnToRotation, DeltaTime, TurnToInfo.TurnSpeedYaw);
		}

		NewRotation.Pitch = MyRotation.Pitch;
		NewRotation.Roll = MyRotation.Roll;

		UE_LOG(LogDLGameplayCore, Verbose, TEXT("(%s)<%s> DeltaTime %2f, TargetRotation %s, PreRotation %s NewRotation %s SpeedYaw %2f"),
			TO_STR(this), NET_ROLE_STR(GetOwner()),
			DeltaTime,
			*TurnToInfo.TargetTurnToRotation.ToString(),
			*MyRotation.ToString(),
			*NewRotation.ToString(),
			TurnToInfo.TurnSpeedYaw);

		MoveUpdatedComponent(FVector::ZeroVector, NewRotation, false);


		// 检查是否转到位了 如果转到位 就标记完成
		auto&& TurnToMyRotation = UpdatedComponent->GetComponentRotation();
		if (FMath::IsNearlyEqual(FRotator::ClampAxis(TurnToMyRotation.Yaw), FRotator::ClampAxis(TurnToInfo.TargetTurnToRotation.Yaw), TurnToComplateErrorValue))
		{
			GetTurnToInfo_Mutable().bIsTurnToComplate = true;
			UE_LOG(LogDLGameplayCore, Verbose, TEXT("[%s] TryTurnToTargetRotation %s  Complate"), NET_ROLE_STR(GetOwner()), *NewRotation.ToString());
		}
	}
	else
	{

		Super::PhysicsRotation(DeltaTime);
	}
}

void UDLCharacterMovementComponentBase::RegisterMoveStrategy(const FDLMoveStrategyInterface& InStrategy)
{
	StrategyInterface = InStrategy;
}

FDelegateHandle UDLCharacterMovementComponentBase::AddMoveStateChangeDelegate(const FOnMoveStateChange::FDelegate& InDelegate)
{
	return OnMoveStateChange.Add(InDelegate);
}

void UDLCharacterMovementComponentBase::RemoveMoveStateChangeDelegate(const FDelegateHandle& Handle)
{
	OnMoveStateChange.Remove(Handle);
}

void UDLCharacterMovementComponentBase::ShowDebugInfo(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	FString ClientStr;
	if (GetOwner()->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		ClientStr = FString::Printf(TEXT("Client state : %s velocity : %s  MaxSpeed : %f  ActorRotation: %s"), ENUM_TO_STR(EMovementMode, MovementMode), *Velocity.ToCompactString(), GetMaxSpeed(), *GetOwner()->GetActorRotation().ToString());
	}
	Canvas->SetDrawColor(FColor::Red);
	YL = Canvas->DrawText(GEngine->GetLargeFont(), ClientStr, 4.0f, YPos, 1.5f, 1.5f);
	YPos += YL;
}

void UDLCharacterMovementComponentBase::ServerTurnToTargetRotation_Implementation(const FRotator& InTargetRotation, bool ImmediatelyTurn,
	float OverrideTurnSpeedYaw)
{
	this->TurnToTargetRotation(InTargetRotation, ImmediatelyTurn, OverrideTurnSpeedYaw);
}


void UDLCharacterMovementComponentBase::TurnToTargetRotation(const FRotator& InTargetRotation, bool ImmediatelyTurn,
	float OverrideTurnSpeedYaw)
{
	const auto& CurrentTurnToInfo = GetTurnToInfo();

	const auto& MyRotation = UpdatedComponent->GetComponentRotation();

	if (FMath::IsNearlyEqual(FRotator::ClampAxis(CurrentTurnToInfo.TargetTurnToRotation.Yaw), FRotator::ClampAxis(InTargetRotation.Yaw)))
	{
		if (FMath::IsNearlyEqual(FRotator::ClampAxis(InTargetRotation.Yaw), FRotator::ClampAxis(MyRotation.Yaw), TurnToComplateErrorValue))
		{
			return;
		}
	}

	auto& MutableTurnToInfo = GetTurnToInfo_Mutable();

	MutableTurnToInfo.TargetTurnToRotation = InTargetRotation.Clamp();
	MutableTurnToInfo.bIsTurnToComplate = false;
	MutableTurnToInfo.bImmediatelyTurn = ImmediatelyTurn;

	if (FMath::IsNearlyZero(OverrideTurnSpeedYaw))
	{
		MutableTurnToInfo.TurnSpeedYaw = RotationRate.Yaw;
	}
	else
	{
		MutableTurnToInfo.TurnSpeedYaw = OverrideTurnSpeedYaw;
	}

	if (GetOwnerRole() == ENetRole::ROLE_AutonomousProxy)
	{
		this->ServerTurnToTargetRotation(InTargetRotation, ImmediatelyTurn, OverrideTurnSpeedYaw);
	}
}

void UDLCharacterMovementComponentBase::UpdateMoveInfo(const FCharacterMovementInfo& Info)
{
	RotationRate.Yaw = Info.TurnSpeedYaw;
}

FDLCharacterTurnToInfo& UDLCharacterMovementComponentBase::GetTurnToInfo_Mutable()
{
	if (GetOwnerRole() == ENetRole::ROLE_AutonomousProxy)
	{
		return AutonomousTurnInfo;
	}
	else
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(UDLCharacterMovementComponentBase, NetRepTurnToInfo, this);
		return NetRepTurnToInfo;
	}
}

void UDLCharacterMovementComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;

	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(UDLCharacterMovementComponentBase, NetRepTurnToInfo, Params);
}

FDLCharacterTurnToInfo UDLCharacterMovementComponentBase::GetTurnToInfo() const
{
	FDLCharacterTurnToInfo Ret;
	if (GetOwnerRole() == ENetRole::ROLE_AutonomousProxy)
	{
		Ret = AutonomousTurnInfo;
	}
	else
	{
		Ret = NetRepTurnToInfo;
	}

	UE_LOG(LogDLGameplayCore, Verbose, TEXT("(%s)<%s> bIsTurnToComplate %d"), TO_STR(this), NET_ROLE_STR(GetOwner()), Ret.bIsTurnToComplate);
	return Ret;
}

void UDLCharacterMovementComponentBase::OnRep_NetRepTurnToInfo() const
{
	UE_LOG(LogDLGameplayCore, Verbose, TEXT("(%s)<%s> Rep_NetRepTurnToInfo bIsTurnToComplate %d"), TO_STR(this), NET_ROLE_STR(GetOwner()), NetRepTurnToInfo.bIsTurnToComplate);
}
