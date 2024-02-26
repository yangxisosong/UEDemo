// Fill out your copyright notice in the Description page of Project Settings.


#include "DLAnimComponentHumanLocomotion.h"

#include "DLCharacterMovementComponent.h"
#include "Animation/AnimationTestCharacter.h"
#include "Animation/DLAnimCommonLibrary.h"
#include "Animation/DLLinkedAnimInstance.h"
#include "Animation/DLMainAnimInstance.h"
#include "Animation/AnimDef/DLAnimTag.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::EBoneName::Pelvis, "BoneName");

// Sets default values for this component's properties
UDLAnimComponentHumanLocomotion::UDLAnimComponentHumanLocomotion()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	AnimCharacterInfo.CharacterMainState = ECharacterMainState::Locking;
}


// Called when the game starts
void UDLAnimComponentHumanLocomotion::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UDLAnimComponentHumanLocomotion::OnTick(float DeltaTime)
{
	if (!GetCharacterOwner())
	{
		return;
	}

	if (!GetMovementComponent())
	{
		return;
	}

	if (!GetMainAnimIns())
	{
		return;
	}

	SetEssentialValues(DeltaTime);

	switch (AnimCharacterInfo.MovementState.MovementState)
	{
	case EMovementState::Grounded:
		UpdateCharacterMovement();
		UpdateGroundedRotation(DeltaTime);
		break;
	case EMovementState::InAir:
		UpdateInAirRotation(DeltaTime);
		break;
	case EMovementState::Ragdoll:
		UpdateRagdoll(DeltaTime);
		break;
	default:
		break;
	}

	// Cache values
	PreviousVelocity = GetCharacterOwner()->GetVelocity();
	PreviousAimYaw = AimingRotation.Yaw;
}

void UDLAnimComponentHumanLocomotion::OnLanded(const FHitResult& Hit)
{
	EventOnLanded();
}

void UDLAnimComponentHumanLocomotion::OnReceivedFrameFrozenStart(UAnimSequence* TargetSeq, const float StartTime,
	const float EndTime, const FName SocketName)
{
	if (TargetSeq)
	{
		// GetCharacterOwner()->GetMainAnimInstance()->GetActiveInstanceForMontage(
		// 	GetCharacterOwner()->GetCurrentMontage())->SetPlaying(false);
		// GetAnimCharacterInfoRef().FrameFrozenInfo.CurAnimSeq = TargetSeq;
		// GetAnimCharacterInfoRef().FrameFrozenInfo.StartTimeOffset = StartTime;
		// GetAnimCharacterInfoRef().FrameFrozenInfo.EndTimeOffset = EndTime;
		// GetAnimCharacterInfoRef().FrameFrozenInfo.SocketName = SocketName;
	}
}

void UDLAnimComponentHumanLocomotion::OnReceivedFrameFrozenEnd()
{
	// GetAnimCharacterInfoRef().FrameFrozenInfo = FFrameFrozenInfo();
	//
	// GetCharacterOwner()->GetMainAnimInstance()->GetActiveInstanceForMontage(
	// 	GetCharacterOwner()->GetCurrentMontage())->SetPlaying(true);
}

void UDLAnimComponentHumanLocomotion::SetEssentialValues(float DeltaTime)
{

	CurrentAcceleration = GetMovementComponent()->GetCurrentAcceleration();
	EasedMaxAcceleration = GetMovementComponent()->GetMaxAcceleration() != 0
		? GetMovementComponent()->GetMaxAcceleration() : EasedMaxAcceleration / 2;

	const FRotator ControlRotation = GetCharacterOwner()->GetControlRotation();
	// 插值AimingRotation到当前的控制旋转角度来平滑角色旋转移动。
	AimingRotation = FMath::RInterpTo(AimingRotation, ControlRotation, DeltaTime, 30);

	// 设置加速度
	const FVector CurrentVel = GetCharacterOwner()->GetVelocity();
	SetAcceleration((CurrentVel - PreviousVelocity) / DeltaTime);

	DL_ANIM_LOG(Warning, TEXT("CurrentVel:%f,Acceleration:%f"), CurrentVel.Size2D(), Acceleration.Size2D());
	DL_ANIM_LOG(Warning, TEXT("CurrentVel:%s,MaxWAlkSpeed:%f"), *CurrentVel.ToString(), GetMovementComponent()->MaxWalkSpeed);

	// 通过获取角色的速度来确定角色是否在移动。速度等于x,y方向的矢量长度。
	// 如果角色正在移动，则更新最后一次速度旋转。保存此值是因为了解最后的运动方向可能很有用，即使在角色停止之后。 
	SetSpeed(CurrentVel.Size2D());
	SetIsMoving(GetAnimCharacterInfo().Speed > 1.0f);
	if (GetAnimCharacterInfo().bIsMoving)
	{
		LastVelocityRotation = CurrentVel.ToOrientationRotator();
	}

	// 通过获取其移动输入量来确定角色是否有移动输入。
	// 运动输入量等于当前加速度除以最大加速度，使得它的范围是 0-1。
	// 如果角色有移动输入，则更新 Last Movement Input Rotation。 
	SetMovementInputAmount(CurrentAcceleration.Size() / EasedMaxAcceleration);
	DL_ANIM_LOG(Warning
		, TEXT("<SetEssentialValues>MovementInput.Size():%f,EasedMaxAcceleration:%f,MovementInputAmount:%f")
	, GetAnimCharacterInfo().MovementInput.Size(), EasedMaxAcceleration, GetAnimCharacterInfo().MovementInputAmount);

	SetHasMovementInput(GetAnimCharacterInfo().MovementInputAmount > 0.0f);
	if (GetAnimCharacterInfo().bHasMovementInput)
	{
		LastMovementInputRotation = CurrentAcceleration.ToOrientationRotator();
	}

	// 通过比较当前和之前的 Aim Yaw 值除以 Delta Seconds 来设置 Aim Yaw 速率。
	// 这也是相机从左到右旋转的速度。 
	GetAnimCharacterInfoRef().AimYawRate = (FMath::Abs((AimingRotation.Yaw - PreviousAimYaw) / DeltaTime));
}

void UDLAnimComponentHumanLocomotion::UpdateCharacterMovement()
{
	const EGaitType AllowedGait = GetAllowedGait();

	const EGaitType ActualGait = GetActualGait(AllowedGait);

	if (ActualGait != GetAnimCharacterInfo().Gait.Gait)
	{
		SetGait(ActualGait);
	}

	GetMovementComponent()->SetAllowedGait(ActualGait);
}

float UDLAnimComponentHumanLocomotion::CalculateGroundedRotationRate()
{
	// 使用移动设置中的当前旋转速率曲线计算旋转速率。
	// 将曲线与映射速度结合使用可让您高度控制每个速度的旋转速率。如果相机快速旋转，则提高速度以获得更快速的旋转响应。 
	const float MappedSpeedVal = GetMovementComponent()->GetMappedSpeed();
	const float CurveVal =
		GetMovementComponent()->CurrentMovementSettings.RotationRateCurve->GetFloatValue(MappedSpeedVal);
	const float ClampedAimYawRate = FMath::GetMappedRangeValueClamped(
		FVector2f{ 0.0f, 300.0f }, FVector2f{ 1.0f, 3.0f }, GetAnimCharacterInfo().AimYawRate);
	DL_ANIM_LOG(Warning, TEXT("<UDLAnimComponentHumanLocomotion::CalculateGroundedRotationRate> MappedSpeedVal:%f,CurveVal:%f,ClampedAimYawRate:%f")
		, MappedSpeedVal, CurveVal, ClampedAimYawRate);
	return CurveVal * ClampedAimYawRate;
}

void UDLAnimComponentHumanLocomotion::SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed,
	float ActorInterpSpeed, float DeltaTime)
{
	const FRotator CurRotation = GetCharacterOwner()->GetActorRotation();
	DL_ANIM_LOG(Warning, TEXT("SmoothCharacterRotation 1111,CurrentRotation:%s,TargetRotation:%s,TargetInterpSpeed:%f,ActorInterpSpeed:%f")
, *CurRotation.ToCompactString(), *Target.ToCompactString(), TargetInterpSpeed, ActorInterpSpeed);

	// GetAnimCharacterInfoRef().RotateDelta = FMath::Abs(TargetRotation.Yaw - CurRotation.Yaw);

	// 平滑旋转
	TargetRotation =
		FMath::RInterpConstantTo(TargetRotation, Target, DeltaTime, TargetInterpSpeed);
	GetCharacterOwner()->SetActorRotation(TargetRotation);
}

void UDLAnimComponentHumanLocomotion::LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed,
	float DeltaTime)
{
	// 防止角色旋转超过某个角度。 
	FRotator Delta = AimingRotation - GetCharacterOwner()->GetActorRotation();
	Delta.Normalize();
	const float RangeVal = Delta.Yaw;

	if (RangeVal < AimYawMin || RangeVal > AimYawMax)
	{
		const float ControlRotYaw = AimingRotation.Yaw;
		const float TargetYaw = ControlRotYaw + (RangeVal > 0.0f ? AimYawMin : AimYawMax);
		SmoothCharacterRotation({ 0.0f, TargetYaw, 0.0f }, 0.0f, InterpSpeed, DeltaTime);
	}
}

EGaitType UDLAnimComponentHumanLocomotion::GetAllowedGait()
{
	// 计算允许的步态。 表示角色当前允许的最大步态，可以由想要的步态、旋转方式、站姿等来决定。
// 例如，如果你想在室内强制角色进入行走状态，可以在这里完成。 

	if (!GetAnimCharacterInfo().CharacterMainState.Normal())
	{
		if (DesiredGait == EGaitType::Sprinting)
		{
			return EGaitType::Running;
		}
		return DesiredGait;
	}

	return DesiredGait;
}

EGaitType UDLAnimComponentHumanLocomotion::GetActualGait(EGaitType AllowedGait)
{
	// 获取实际步态。 这是根据角色的实际运动计算得出的，因此它可能与所需的步态或允许的步态不同。
	// 例如，如果允许的步态变为步行，则实际步态仍将运行，直到角色减速到步行速度。 

#if WITH_EDITOR
	AAnimationTestCharacter* TestCharacter = Cast<AAnimationTestCharacter>(GetCharacterOwner());
	if (TestCharacter && TestCharacter->ForceGait != EGaitType::None)
	{
		return TestCharacter->ForceGait;
	}
#endif

	const float LocWalkSpeed = GetMovementComponent()->CurrentMovementSettings.Walk
		.CalculateSpeed(GetMovementComponent()->Velocity, GetCharacterOwner()->GetActorRotation());
	const float LocRunSpeed = GetMovementComponent()->CurrentMovementSettings.Run
		.CalculateSpeed(GetMovementComponent()->Velocity, GetCharacterOwner()->GetActorRotation());

	DL_ANIM_LOG(Warning, TEXT("<GetActualGait> GetActualGait:%f,LocRunSpeed:%f,CurSpeed:%f")
		, LocWalkSpeed, LocRunSpeed, GetAnimCharacterInfo().Speed);

	// if (GetAnimCharacterInfo().Speed > LocRunSpeed - 10.0f)
	// {
	// 	if (GetAnimCharacterInfo().CharacterMainState.Normal())
	// 	{
	// 		return EGaitType::Sprinting;
	// 	}
	// 	return EGaitType::Running;
	// }

	if (GetAnimCharacterInfo().Speed >= LocWalkSpeed)
	{
		return EGaitType::Running;
	}

	return EGaitType::Walking;
}

void UDLAnimComponentHumanLocomotion::SetDesiredGait(EGaitType NewGait)
{
	DesiredGait = NewGait;
}

void UDLAnimComponentHumanLocomotion::SetActorLocationAndTargetRotation(FVector NewLocation, FRotator NewRotation)
{
	GetCharacterOwner()->SetActorLocationAndRotation(NewLocation, NewRotation);
	TargetRotation = NewRotation;
}

void UDLAnimComponentHumanLocomotion::SetMovementState(const EMovementState NewState)
{
	if (GetAnimCharacterInfo().MovementState != NewState)
	{
		GetAnimCharacterInfoRef().PrevMovementState = GetAnimCharacterInfo().MovementState;
		GetAnimCharacterInfoRef().MovementState = NewState;
		OnMovementStateChanged(GetAnimCharacterInfo().PrevMovementState);
	}
}

void UDLAnimComponentHumanLocomotion::EventOnLanded()
{
	const float VelZ = FMath::Abs(GetMovementComponent()->Velocity.Z);

	if (GetAnimCharacterInfo().bHasMovementInput && VelZ >= GetCharacterOwner()->GetAnimConfig().LandConfig.BreakfallOnLandVelocity)
	{
		OnLandRoll();
	}
	else
	{
		GetMovementComponent()->BrakingFrictionFactor = GetAnimCharacterInfo().bHasMovementInput ? 0.5f : 3.0f;

		GetCharacterOwner()->GetWorldTimerManager().SetTimer(OnLandedFrictionResetTimer, this,
			&UDLAnimComponentHumanLocomotion::OnLandFrictionReset, 0.5f, false);
	}
}

void UDLAnimComponentHumanLocomotion::OnLandRoll() const
{
	GetMainAnimIns()->Montage_Play(GetCharacterOwner()->GetAnimConfig().LandConfig.LandRollMontage);
}

void UDLAnimComponentHumanLocomotion::OnLandFrictionReset()
{
	// 重设制动摩擦系数
	GetMovementComponent()->BrakingFrictionFactor = 0.0f;
}

void UDLAnimComponentHumanLocomotion::OnMovementStateChanged(EMovementState PreviousState)
{
	DL_ANIM_LOG(Log, TEXT("<UDLAnimComponentHumanLocomotion::OnMovementStateChanged> %s")
		, *UDLAnimCommonLibrary::GetEnumerationToString(PreviousState));
	if (GetAnimCharacterInfo().MovementState.InAir())
	{
		if (GetAnimCharacterInfo().MovementAction.None())
		{
			// 如果角色进入空中，设置空中的旋转值并取消蹲伏状态。
			InAirRotation = GetCharacterOwner()->GetActorRotation();
		}
		else if (GetAnimCharacterInfo().MovementAction.Rolling())
		{
			InAirRotation = GetCharacterOwner()->GetActorRotation();
			// 如果角色当前正在滚动，则启用布娃娃。
			RagdollStart();
		}
	}
}

void UDLAnimComponentHumanLocomotion::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{

	// 使用角色移动模式更改将移动状态设置为正确的值。 这让你有一组自定义的移动状态，但仍使用默认角色移动组件的功能。

	if (GetMovementComponent()->MovementMode == MOVE_Walking ||
		GetMovementComponent()->MovementMode == MOVE_NavWalking)
	{
		SetMovementState(EMovementState::Grounded);
	}
	else if (GetMovementComponent()->MovementMode == MOVE_Falling)
	{
		SetMovementState(EMovementState::InAir);
	}
}

FVector UDLAnimComponentHumanLocomotion::GetMovementInput() const
{
	return CurrentAcceleration;
}

void UDLAnimComponentHumanLocomotion::UpdateGroundedRotation(float DeltaTime)
{
	if (GetAnimCharacterInfo().MovementAction.None())
	{
		const bool bCanUpdateMovingRot = ((GetAnimCharacterInfo().bIsMoving && GetAnimCharacterInfo().bHasMovementInput)
			|| GetAnimCharacterInfo().Speed > 10.0f) && !GetCharacterOwner()->HasAnyRootMotion();
		if (bCanUpdateMovingRot)
		{
			const float GroundedRotationRate = CalculateGroundedRotationRate();
			if (GetAnimCharacterInfo().CharacterMainState.Normal())
			{
				SmoothCharacterRotation({ 0.0f, LastVelocityRotation.Yaw, 0.0f }, 800.0f, GroundedRotationRate,
										DeltaTime);
			}
			else if (GetAnimCharacterInfo().CharacterMainState.Locking())
			{
				float YawValue;
				// if (GetAnimCharacterInfo().Gait.Sprinting())
				// {
				// 	YawValue = LastVelocityRotation.Yaw;
				// }
				// else
				// {
				const float YawOffsetCurveVal = GetMainAnimIns()->GetCurveValueByEnum(EDLAnimCurveName::YawOffset);
				YawValue = AimingRotation.Yaw + YawOffsetCurveVal;
				// }
				SmoothCharacterRotation({ 0.0f, YawValue, 0.0f }, 500.0f, GroundedRotationRate, DeltaTime);
			}
			else if (GetAnimCharacterInfo().CharacterMainState.Aiming())
			{
				const float ControlYaw = AimingRotation.Yaw;
				SmoothCharacterRotation({ 0.0f, ControlYaw, 0.0f }, 1000.0f, 20.0f, DeltaTime);
			}
		}
		else
		{
			if (GetAnimCharacterInfo().CharacterMainState.Aiming())
			{
				LimitRotation(-100.0f, 100.0f, 20.0f, DeltaTime);
			}

			// 应用原地转身动画中的 RotationAmount 曲线。
			// Rotation Amount 曲线定义了每帧应该应用多少旋转，并为以 30fps 设置动画的动画计算。 

			const float RotAmountCurve = GetMainAnimIns()->GetCurveValueByEnum(EDLAnimCurveName::RotationAmount);

			if (FMath::Abs(RotAmountCurve) > 0.001f)
			{
				if (GetCharacterOwner()->GetLocalRole() == ROLE_AutonomousProxy)
				{
					TargetRotation.Yaw = UKismetMathLibrary::NormalizeAxis(
						TargetRotation.Yaw + (RotAmountCurve * (DeltaTime / (1.0f / 30.0f))));
					GetCharacterOwner()->SetActorRotation(TargetRotation);
				}
				else
				{

					GetCharacterOwner()->AddActorWorldRotation({ 0, RotAmountCurve * (DeltaTime / (1.0f / 30.0f)), 0 });
				}
				TargetRotation = GetCharacterOwner()->GetActorRotation();
			}
		}
	}
	else if (GetAnimCharacterInfo().MovementAction.Rolling())
	{
		if (GetAnimCharacterInfo().bHasMovementInput)
		{
			// SmoothCharacterRotation({ 0.0f, LastMovementInputRotation.Yaw, 0.0f }, 0.0f, 2.0f, DeltaTime);
		}
	}
}

void UDLAnimComponentHumanLocomotion::UpdateInAirRotation(float DeltaTime)
{
	if (GetAnimCharacterInfo().CharacterMainState.Normal() || GetAnimCharacterInfo().CharacterMainState.Locking())
	{
		SmoothCharacterRotation({ 0.0f, InAirRotation.Yaw, 0.0f }, 0.0f, 5.0f, DeltaTime);
	}
	else if (GetAnimCharacterInfo().CharacterMainState.Aiming())
	{
		SmoothCharacterRotation({ 0.0f,AimingRotation.Yaw, 0.0f }
		, 0.0f, 15.0f, DeltaTime);
		InAirRotation = GetCharacterOwner()->GetActorRotation();
	}
}

void UDLAnimComponentHumanLocomotion::UpdateRagdoll(float DeltaTime)
{
	const FVector NewRagdollVel = GetCharacterOwner()->GetMesh()->GetPhysicsLinearVelocity(GetMainAnimIns()->GetBoneByTag(EAnimTagDef::EBoneName::Root));
	LastRagdollVelocity = (NewRagdollVel != FVector::ZeroVector || GetCharacterOwner()->IsLocallyControlled())
		? NewRagdollVel
		: LastRagdollVelocity / 2;

	// 使用布娃娃速度来缩放布娃娃的关节强度以实现物理动画。
	const float SpringValue = FMath::GetMappedRangeValueClamped(FVector2f{ 0.0f, 1000.0f }, FVector2f{ 0.0f, 25000.0f },
																LastRagdollVelocity.Size());
	GetCharacterOwner()->GetMesh()->SetAllMotorsAngularDriveParams(SpringValue, 0.0f, 0.0f, false);

	// 如果下降速度超过4000，则禁用重力以防止持续加速。
	// 这也可以防止布娃娃穿过地板。
	const bool bEnableGrav = LastRagdollVelocity.Z > -4000.0f;
	GetCharacterOwner()->GetMesh()->SetEnableGravity(bEnableGrav);

	// 更新 Actor 位置以跟随布娃娃。
	SetActorLocationDuringRagdoll(DeltaTime);
}

void UDLAnimComponentHumanLocomotion::SetActorLocationDuringRagdoll(float DeltaTime)
{
	TargetRagdollLocation = GetCharacterOwner()->GetMesh()->GetSocketLocation(GetMainAnimIns()->GetBoneByTag(EAnimTagDef::EBoneName::Pelvis));

	// 判断布娃娃是朝上还是朝下，并相应地设置目标旋转。
	const FRotator PelvisRot = GetCharacterOwner()->GetMesh()->GetSocketRotation(GetMainAnimIns()->GetBoneByTag(EAnimTagDef::EBoneName::Pelvis));

	if (bReversedPelvis)
	{
		bRagdollFaceUp = PelvisRot.Roll > 0.0f;
	}
	else
	{
		bRagdollFaceUp = PelvisRot.Roll < 0.0f;
	}


	const FRotator TargetRagdollRotation(0.0f, bRagdollFaceUp ? PelvisRot.Yaw - 180.0f : PelvisRot.Yaw, 0.0f);

	// 从目标位置向下追踪以偏移目标位置，当布娃娃躺在地上时，防止胶囊的下半部分穿过地板。
	const FVector TraceVect(TargetRagdollLocation.X, TargetRagdollLocation.Y,
							TargetRagdollLocation.Z - GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	UWorld* World = GetWorld();
	check(World);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetCharacterOwner());

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(HitResult, TargetRagdollLocation, TraceVect,
													  ECC_Visibility, Params);

	// if (ALSDebugComponent && ALSDebugComponent->GetShowTraces())
	// {
	// 	UALSDebugComponent::DrawDebugLineTraceSingle(World,
	// 												 TargetRagdollLocation,
	// 												 TraceVect,
	// 												 EDrawDebugTrace::Type::ForOneFrame,
	// 												 bHit,
	// 												 HitResult,
	// 												 FLinearColor::Red,
	// 												 FLinearColor::Green,
	// 												 1.0f);
	// }

	bRagdollOnGround = HitResult.IsValidBlockingHit();
	FVector NewRagdollLoc = TargetRagdollLocation;

	if (bRagdollOnGround)
	{
		const float ImpactDistZ = FMath::Abs(HitResult.ImpactPoint.Z - HitResult.TraceStart.Z);
		NewRagdollLoc.Z += GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - ImpactDistZ + 2.0f;
	}

	SetActorLocationAndTargetRotation(bRagdollOnGround ? NewRagdollLoc : TargetRagdollLocation, TargetRagdollRotation);
}

void UDLAnimComponentHumanLocomotion::RagdollStart()
{
	if (RagdollStateChangedDelegate.IsBound())
	{
		RagdollStateChangedDelegate.Broadcast(true);
	}
	TargetRagdollLocation = GetCharacterOwner()->GetMesh()->GetSocketLocation(
		GetMainAnimIns()->GetBoneByTag(EAnimTagDef::EBoneName::Pelvis));
	RagdollPull = 0;

	// 清除角色移动模式并将移动状态设置为布娃娃
	GetCharacterOwner()->GetCharacterMovement()->SetMovementMode(MOVE_None);
	SetMovementState(EMovementState::Ragdoll);

	// 禁用胶囊碰撞并启用从骨盆开始的网格物理模拟。
	GetCharacterOwner()->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterOwner()->GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetCharacterOwner()->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterOwner()->GetMesh()->SetAllBodiesBelowSimulatePhysics(
		GetMainAnimIns()->GetBoneByTag(EAnimTagDef::EBoneName::Pelvis)
		, true, true);

	// 停止所有蒙太奇。
	GetMainAnimIns()->Montage_Stop(0.2f);

	// 修复角色网格在布娃娃结束前的瞬间显示默认 A 姿势的问题
	GetCharacterOwner()->GetMesh()->bOnlyAllowAutonomousTickPose = true;
}

void UDLAnimComponentHumanLocomotion::RagdollEnd()
{
	GetCharacterOwner()->GetMesh()->bOnlyAllowAutonomousTickPose = false;

	if (!GetMainAnimIns())
	{
		return;
	}

	// 如果布娃娃在地上，则设置运动模式为行走并播放起身动画。
	// 如果不是，则将移动模式设置为下落并更新角色移动速度以匹配最后的布娃娃速度。
	if (bRagdollOnGround)
	{
		GetCharacterOwner()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		//TODO:暂时注释
		// GetMainAnimIns()->Montage_Play(
		// 	GetMainAnimIns()->GetAnimMontageByTag(bRagdollFaceUp ? EDLAnimName::GetUpBack : EDLAnimName::GetUpFront),
		// 							   1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
	}
	else
	{
		GetCharacterOwner()->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterOwner()->GetCharacterMovement()->Velocity = LastRagdollVelocity;
	}

	// 重新启用胶囊碰撞，并禁用网格上的物理模拟。
	GetCharacterOwner()->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterOwner()->GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetCharacterOwner()->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCharacterOwner()->GetMesh()->SetAllBodiesSimulatePhysics(false);

	if (RagdollStateChangedDelegate.IsBound())
	{
		RagdollStateChangedDelegate.Broadcast(false);
	}
}

void UDLAnimComponentHumanLocomotion::SetSpeed(float NewSpeed)
{
	DL_ANIM_LOG(Warning
		, TEXT("<UDLLinkedAnimInstanceHumanLocomotion::SetSpeed>Actual Speed:%f")
		, NewSpeed);
	GetAnimCharacterInfoRef().Speed = NewSpeed;
}

void UDLAnimComponentHumanLocomotion::SetIsMoving(bool bNewIsMoving)
{
	GetAnimCharacterInfoRef().bIsMoving = bNewIsMoving;
}

void UDLAnimComponentHumanLocomotion::SetMovementInputAmount(float NewMovementInputAmount)
{
	GetAnimCharacterInfoRef().MovementInputAmount = NewMovementInputAmount;
}

void UDLAnimComponentHumanLocomotion::SetHasMovementInput(bool bNewHasMovementInput)
{
	GetAnimCharacterInfoRef().bHasMovementInput = bNewHasMovementInput;
}

void UDLAnimComponentHumanLocomotion::SetAcceleration(const FVector& NewAcceleration)
{
	Acceleration = (NewAcceleration != FVector::ZeroVector || GetCharacterOwner()->IsLocallyControlled())
		? NewAcceleration
		: Acceleration / 2;
	GetAnimCharacterInfoRef().Acceleration = Acceleration;
}

void UDLAnimComponentHumanLocomotion::SetGait(EGaitType NewGait)
{
	GetAnimCharacterInfoRef().Gait = NewGait;
}

