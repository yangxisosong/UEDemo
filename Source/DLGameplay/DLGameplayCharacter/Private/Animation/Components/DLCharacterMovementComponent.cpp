// Fill out your copyright notice in the Description page of Project Settings.


#include "DLCharacterMovementComponent.h"

#include "ConvertString.h"
#include "DLAnimComponentHumanLocomotion.h"
#include "Animation/AnimationTestCharacter.h"
#include "Animation/DLAnimCommonLibrary.h"
#include "Curves/CurveVector.h"
#include "GameplayFramwork/DLCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

class UDLAnimComponentHumanLocomotion;

UDLCharacterMovementComponent::UDLCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UDLCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	ensureMsgf(GetDLCharacterOwner(), TEXT("Character为啥是空的！"));
}

void UDLCharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	if (CurrentMovementSettings.MovementCurve)
	{
		GroundFriction = CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed()).Z;
	}
	Super::PhysWalking(deltaTime, Iterations);
}



void UDLCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
													  const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (!GetDLCharacterOwner())
	{
		return;
	}

	if (Velocity.IsNearlyZero(1.0f) || OldLocation.Equals(GetActorLocation(), 1.0f))
	{
		bStopped = true;
		bStopping = false;
	}

	const float UpdateMaxWalkSpeed = GetDLCharacterOwner()->GetTargetMovementSettings().GetSpeedForGait(AllowedGait)
		.CalculateSpeed(Velocity, GetDLCharacterOwner()->GetActorRotation());
	MaxWalkSpeed = UpdateMaxWalkSpeed;
	DL_ANIM_LOG(Warning, TEXT("<UDLCharacterMovementComponent::OnMovementUpdated>MaxWalkSpeed:%f,AllowedGait:%s")
		, MaxWalkSpeed, *UDLAnimCommonLibrary::GetEnumerationToString(AllowedGait));


}

float UDLCharacterMovementComponent::GetMaxAcceleration() const
{
	if (!IsMovingOnGround() || !GetDLCharacterOwner()->GetTargetMovementSettings().MovementCurve)
	{
		return Super::GetMaxAcceleration();
	}

	float Result = GetDLCharacterOwner()->GetTargetMovementSettings().MovementCurve->GetVectorValue(GetMappedSpeed()).X;
	// if (GetDLCharacterOwner()->GetAnimConfig().RotationAcceleration)
	// {
	// 	const float RotateDelta = GetDLCharacterOwner()->GetAnimComponent()->GetAnimCharacterInfo().RotateDelta;
	// 	Result *= GetDLCharacterOwner()->GetAnimConfig().RotationAcceleration->GetFloatValue(RotateDelta);
	// 	DL_ANIM_LOG(Warning, TEXT("<UDLCharacterMovementComponent::GetMaxAcceleration>  RotateDelta:%f"), RotateDelta);
	// }
	DL_ANIM_LOG(Warning, TEXT("<UDLCharacterMovementComponent::GetMaxAcceleration>  MaxAcceleration:%f"), Result);
	return Result;
}

float UDLCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (!IsMovingOnGround() || !GetDLCharacterOwner()->GetTargetMovementSettings().MovementCurve)
	{
		return Super::GetMaxBrakingDeceleration();
	}

	const float Result = GetDLCharacterOwner()->GetTargetMovementSettings().MovementCurve->GetVectorValue(GetMappedSpeed()).Y;
	DL_ANIM_LOG(Warning, TEXT("<UDLCharacterMovementComponent::GetMaxBrakingDeceleration>  %f"), Result);
	return Result;
}

void UDLCharacterMovementComponent::ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration)
{
	PredictStop(DeltaTime);
	Super::ApplyVelocityBraking(DeltaTime, Friction, BrakingDeceleration);
	// UpdateCharacterLocation(DeltaTime);
}

void UDLCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDLCharacterMovementComponent::PredictStop(const float DeltaTime)
{
	if (!(Velocity.IsZero() || !HasValidData() || HasAnimRootMotion() || DeltaTime < MIN_TICK_TIME))
	{
		if (Velocity.Size2D() > 0.0f && FMath::IsNearlyZero(Acceleration.Size2D()))
		{
			ACharacter* Character = Cast<ACharacter>(GetOwner());
			if (Character && !bStopping)
			{
				bStopping = true;
				bStopped = false;

				// const UCurveFloat* StopDistanceCurve = GetDLCharacterOwner()->GetAnimConfig().MovementStateSettings.StopDistanceCurve;
				const UCurveFloat* StopAnimMatchCurve = GetStopCurve();
				if (ensureAlwaysMsgf(StopAnimMatchCurve, TEXT("又是那个没配停止曲线！！！！")))
				{

					const float Speed = GetDLCharacterOwner()->GetAnimComp<UDLAnimComponentHumanLocomotion>()->GetAnimCharacterInfo().Speed;

					auto& InfoRef = GetDLCharacterOwner()->GetAnimComp<UDLAnimComponentHumanLocomotion>()->GetAnimCharacterInfoRef();
					InfoRef.StopAnimBeginTimeStamp = StopAnimMatchCurve->GetFloatValue(Speed);
					if (InfoRef.bHasMovementInput)
					{
						InfoRef.LastInputGait = InfoRef.Gait;
					}

					// float LocStopDis = 0.0f;
					// TargetStopPosition = GetDLCharacterOwner()->GetActorLocation();
					// PredictStopAnimPos(DeltaTime, TargetStopPosition, LocStopDis);

					DL_ANIM_LOG(Warning, TEXT("<UDLCharacterMovementComponent::PredictStop> StopAnimBeginTimeStamp:%f")
					, InfoRef.StopAnimBeginTimeStamp);
				}

			}
		}
	}
}

void UDLCharacterMovementComponent::PredictStopAnimPos(const float DeltaTime, FVector& OutStopPosition,
	float& OutStopDis) const
{
	float LocVelocity = Velocity.Size2D();
	const float LocBrakingDeceleration = BrakingDecelerationWalking;
	const float LocBrakingFrictionFactor = FMath::Max(0.f, BrakingFrictionFactor);
	const float LocFriction = FMath::Max(GroundFriction * LocBrakingFrictionFactor, 0.0f);

	const float RevAccel = -LocBrakingDeceleration;
	float LocStoppingDis = 0.0f;
	float LocStopTime = 0.0f;
	while (LocVelocity > 1.0f)
	{
		LocVelocity = LocVelocity + (RevAccel + (-LocFriction) * LocVelocity) * DeltaTime;
		LocStopTime += DeltaTime;
		LocStoppingDis += LocVelocity * DeltaTime;
	}

	OutStopDis = LocStoppingDis;
	OutStopPosition = CharacterOwner->GetActorLocation() + Velocity.GetSafeNormal2D() * LocStoppingDis;
	// #if WITH_EDITOR
	// 	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), FVector(OutStopPosition.X, OutStopPosition.Y, OutStopPosition.Z - 75.0f), 20, 12, FColor::Red, 0.0f, 2);
	// #endif
}

void UDLCharacterMovementComponent::UpdateCharacterLocation(const float DeltaTime)
{
	if (!bStopped && !FMath::IsNearlyZero(DeltaTime))
	{

		FVector LocCurrentLocation = CharacterOwner->GetActorLocation();
		LocCurrentLocation = FMath::VInterpTo(
			LocCurrentLocation, TargetStopPosition, DeltaTime, StepLength
		);
		GetDLCharacterOwner()->SetActorLocation(LocCurrentLocation);
	}
}

UCurveFloat* UDLCharacterMovementComponent::GetStopCurve() const
{
	const auto& Info = GetDLCharacterOwner()->GetAnimComp<UDLAnimComponentHumanLocomotion>()->GetAnimCharacterInfo();
	const auto& Config = GetDLCharacterOwner()->GetAnimConfig();

	if (Info.CharacterMainState.Normal())
	{
		switch (AllowedGait)
		{
		case EGaitType::Walking:
			return Config.MovementStateSettings.Normal.Walk.StopSetting.Forward;
		case EGaitType::Running:
			return Config.MovementStateSettings.Normal.Run.StopSetting.Forward;
		case EGaitType::Sprinting:
			return Config.MovementStateSettings.Normal.Sprint.StopSetting.Forward;
		default:
			return Config.MovementStateSettings.Normal.Walk.StopSetting.Forward;
		}
	}
	else
	{
		FStopAnimConfigs StopSetting;
		switch (AllowedGait)
		{
		case EGaitType::Walking:
			StopSetting = Config.MovementStateSettings.Locking.Walk.StopSetting;
			break;
		case EGaitType::Running:
			StopSetting = Config.MovementStateSettings.Locking.Run.StopSetting;
			break;
		case EGaitType::Sprinting:
			StopSetting = Config.MovementStateSettings.Locking.Sprint.StopSetting;
			break;
		default:
			StopSetting = Config.MovementStateSettings.Locking.Walk.StopSetting;
			break;
		}

		const EMovementDirection MovementDirection = GetMovementDirection();
		switch (MovementDirection)
		{
		case EMovementDirection::Forward:
			return StopSetting.Forward;
		case EMovementDirection::Backward:
			return StopSetting.Backward;
		case EMovementDirection::Left:
			return StopSetting.Left;
		case EMovementDirection::Right:
			return StopSetting.Right;
		default:
			return StopSetting.Forward;
		}
	}
}

EMovementDirection UDLCharacterMovementComponent::GetMovementDirection() const
{
	const FVector LocRelativeVelocityDir = GetDLCharacterOwner()->GetActorRotation().UnrotateVector(Velocity.GetSafeNormal(0.1f));

	const float Sum = FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y) +
		FMath::Abs(LocRelativeVelocityDir.Z);
	const FVector RelativeDir = LocRelativeVelocityDir / Sum;
	FVelocityBlend Result;
	Result.Forward = FMath::Clamp(RelativeDir.X, 0.0f, 1.0f);
	Result.Backward = FMath::Abs(FMath::Clamp(RelativeDir.X, -1.0f, 0.0f));
	Result.Left = FMath::Abs(FMath::Clamp(RelativeDir.Y, -1.0f, 0.0f));
	Result.Right = FMath::Clamp(RelativeDir.Y, 0.0f, 1.0f);

	if (Result.Forward > 0.5f)
	{
		return EMovementDirection::Forward;
	}
	else if (Result.Backward > 0.5f)
	{
		return EMovementDirection::Backward;
	}
	else if (Result.Left > 0.5f)
	{
		return EMovementDirection::Left;
	}
	else if (Result.Right > 0.5f)
	{
		return EMovementDirection::Right;
	}
	return EMovementDirection::Forward;
}

AAnimationTestCharacter* UDLCharacterMovementComponent::GetDLCharacterOwner() const
{
	return Cast<AAnimationTestCharacter>(GetOwner());
}

// float UDLCharacterMovementComponent::GetMaxSpeed() const
// {
// 	return MaxWalkSpeed;
// }

float UDLCharacterMovementComponent::GetMappedSpeed() const
{
	const float Speed = Velocity.Size2D();
	const float LocWalkSpeed = GetDLCharacterOwner()->GetTargetMovementSettings().Walk
		.CalculateSpeed(Velocity, GetDLCharacterOwner()->GetActorRotation());
	const float LocJogSpeed = GetDLCharacterOwner()->GetTargetMovementSettings().Run
		.CalculateSpeed(Velocity, GetDLCharacterOwner()->GetActorRotation());
	const float LocSprintSpeed = GetDLCharacterOwner()->GetTargetMovementSettings().Sprint
		.CalculateSpeed(Velocity, GetDLCharacterOwner()->GetActorRotation());

	DL_ANIM_LOG(Warning, TEXT("<UDLCharacterMovementComponent::GetMappedSpeed>MaxWalkSpeed:%f,Speed:%f,WalkSpeed:%f,JogSpeed:%f,SprintSpeed:%f")
		, MaxWalkSpeed, Speed, LocWalkSpeed, LocJogSpeed, LocSprintSpeed);

	if (Speed > LocJogSpeed)
	{
		return  FMath::GetMappedRangeValueClamped({ LocJogSpeed,LocSprintSpeed }, { 2.0f,3.0f }, Speed);
	}

	if (Speed > LocWalkSpeed)
	{
		return  FMath::GetMappedRangeValueClamped({ LocWalkSpeed,LocJogSpeed }, { 1.0f,2.0f }, Speed);
	}

	return  FMath::GetMappedRangeValueClamped({ 0.0f,LocWalkSpeed }, { 1.0f,2.0f }, Speed);
}

void UDLCharacterMovementComponent::SetAllowedGait(const EGaitType NewGait)
{
	if (NewGait != AllowedGait)
	{
		if (PawnOwner->IsLocallyControlled())
		{
			AllowedGait = NewGait;
			return;
		}

		if (!PawnOwner->HasAuthority())
		{
			const float UpdateMaxWalkSpeed = GetDLCharacterOwner()->GetTargetMovementSettings().GetSpeedForGait(AllowedGait)
				.CalculateSpeed(Velocity, GetDLCharacterOwner()->GetActorRotation());
			MaxWalkSpeed = UpdateMaxWalkSpeed;
			DL_ANIM_LOG(Warning, TEXT("<SetAllowedGait>MaxWalkSpeed:%f"), MaxWalkSpeed);

		}
	}
}

void UDLCharacterMovementComponent::SetCurrentMovementSettings(const FMovementSettings& NewMovementSettings)
{
	CurrentMovementSettings = NewMovementSettings;
}
