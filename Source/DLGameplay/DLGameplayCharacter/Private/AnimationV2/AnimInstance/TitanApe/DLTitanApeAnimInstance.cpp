// Fill out your copyright notice in the Description page of Project Settings.


#include "DLTitanApeAnimInstance.h"
#include "ConvertString.h"
#include "AnimationV2/AnimInstance/Human/DLHumanAnimInsHelper.h"
#include "Animation/DLAnimationLibrary.h"
#include "AnimationV2/DLAnimationMacros.h"
#include "Interface/ICharacterStateAccessor.h"
#include "AnimationV2/AnimationTags.h"
#include "AnimationV2/UAnimInstanceHelper.h"
#include "AttributeSet/DLUnitAttributeBaseSet.h"
#include "Component/DLCharacterMovementComponentBase.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

DECLARE_CYCLE_STAT(TEXT("DLHumanAnimInstance_CopyNewAnimCharacterInfo"), STAT_DLHumanAnimInstance_CopyNewAnimCharacterInfo, STATGROUP_Anim);

void UDLTitanApeAnimInstance::CopyNewAnimCharacterInfo(float DeltaSeconds)
{
	SCOPE_CYCLE_COUNTER(STAT_DLHumanAnimInstance_CopyNewAnimCharacterInfo);

	//记录旧的角色信息
	PrevAnimCharacterInfo = AnimCharacterInfo;

	//组装新的角色信息
	FHumanAnimCharacterInfo NewInfo;

	CurrentCharacterState = ICharacterStateAccessor::Execute_k2_GetCurrentCharacterState(CharacterStateAccessor.GetObject());

	SetMainState(NewInfo);
	SetSpecialState(NewInfo);
	SetMovementAction(NewInfo);
	SetGait(NewInfo);

	SetVelocityRelated(NewInfo, DeltaSeconds);

	SetMovementDirection(NewInfo);

	SetMovementState(NewInfo);

	if (NewInfo.bHasMovementInput)
	{
		LastMovementInputRotation = CurrentAcceleration.ToOrientationRotator();
	}

	{
		//设置是否正在转身,以及目标角度
		bIsTurning = !ICharacterStateAccessor::Execute_K2_IsTurnToComplate(CharacterStateAccessor.GetObject());

		if (bIsTurning)
		{
			ICharacterStateAccessor::Execute_K2_GetTurnToTargetRotation(CharacterStateAccessor.GetObject(), NewInfo.Turn_TargetRotation);
		}
	}

	SetTargetRotation(NewInfo);

	AnimCharacterInfo = NewInfo;
}

void UDLTitanApeAnimInstance::OnUpdateAnimation(float DeltaSeconds)
{
	UpdateAimingValues(DeltaSeconds);
	UpdateMovementState(DeltaSeconds);
}

void UDLTitanApeAnimInstance::OnPostUpdateAnimation(float DeltaSeconds)
{
	Super::OnPostUpdateAnimation(DeltaSeconds);
	PreviousVelocity = AnimCharacterInfo.Velocity;
	PrevIsTurning = !ICharacterStateAccessor::Execute_K2_IsTurnToComplate(CharacterStateAccessor.GetObject());
}

bool UDLTitanApeAnimInstance::GetIsEnableAnimLog()
{
	return IS_ANIM_DEBUG_LOG_ENABLED();
}

void UDLTitanApeAnimInstance::PrintLog(const FString& Log)
{
	DL_ANIM_DEBUG_LOG(Log, TEXT("<%s> %s"), *GetLogCategory(), *Log)
}

bool UDLTitanApeAnimInstance::ShouldMoveCheck()
{
	return ((AnimCharacterInfo.bIsMoving && AnimCharacterInfo.bHasMovementInput)
		|| AnimCharacterInfo.Speed >= AnimConfig.MinMoveThreshold) && AnimCharacterInfo.MovementAction.None();
}

void UDLTitanApeAnimInstance::OnPostInit(const FDLAnimInsInitParams& InitParams)
{
	Super::OnPostInit(InitParams);
	ACharacter* Character = Cast<ACharacter>(TryGetPawnOwner());
	if (Character)
	{
		Character->LandedDelegate.AddDynamic(this, &UDLTitanApeAnimInstance::OnLanded);
	}
}

bool UDLTitanApeAnimInstance::CanTurnInPlace()
{
	//临时检测当前正在播放蒙太奇是不允许播放转身动画

	TArray<UAnimMontage*> ActiveMontages;
	UDLAnimationLibrary::GetAllActiveMontages(this, ActiveMontages);
	bool bIsDefaultSlotActive = false;
	for (const auto& ActiveMontage : ActiveMontages)
	{
		if (ActiveMontage->IsValidSlot("DefaultSlot"))
		{
			bIsDefaultSlotActive = true;
			break;
		}
	}

	return FMath::IsNearlyEqual(GetCurveValue("EnableTransition"), 1.0f, 0.1f) && bIsTurning && !bIsDefaultSlotActive;
}

void UDLTitanApeAnimInstance::TurnInPlaceCheck(float DeltaSeconds)
{
	// 如果瞄准Yaw值速率小于瞄准Yaw值速率限制，检查瞄准角度是否达到了转身的阈值。
	// 对ElapsedDelayTime进行调整以保证在转身前能一直通过上述判断。

	const auto& CurTurnInPlaceConfig = GetCurTurnInPlaceConfig();

	if (FMath::Abs(AimingValues.AimingAngle.X) <= CurTurnInPlaceConfig.TurnCheckMinAngle)
	{
		ElapsedDelayTime = 0.0f;
		return;
	}

	ElapsedDelayTime += DeltaSeconds;
	const float ClampedAimAngle = FMath::GetMappedRangeValueClamped(
		{ CurTurnInPlaceConfig.TurnCheckMinAngle, 180.0f }
		, { CurTurnInPlaceConfig.MinAngleDelay,CurTurnInPlaceConfig.MaxAngleDelay }
	, AimingValues.AimingAngle.X);

	// 检查ElapsedDelayTime是否超过了设置的延迟时间，如果是，则触发转身。
	if (ElapsedDelayTime > ClampedAimAngle)
	{
		FRotator TurnInPlaceYawRot = AnimCharacterInfo.Turn_TargetRotation;
		TurnInPlaceYawRot.Roll = 0.0f;
		TurnInPlaceYawRot.Pitch = 0.0f;
		const bool bIsNearlyEqual = FMath::IsNearlyEqual(
			PrevAnimCharacterInfo.Turn_TargetRotation.Yaw, AnimCharacterInfo.Turn_TargetRotation.Yaw, 5.0f);

		DL_ANIM_DEBUG_LOG(Log, TEXT("<UDLTitanApeAnimInstance::TurnInPlaceCheck> %s[%s] Turn_TargetRotation.Yaw:%f,bIsNearlyEqual:%d")
			, *TryGetPawnOwner()->GetName(), *ENUM_TO_STRING(TryGetPawnOwner()->GetLocalRole()), AnimCharacterInfo.Turn_TargetRotation.Yaw, bIsNearlyEqual);

		TurnInPlace(TurnInPlaceYawRot, 1.0f, 0.0f, !bIsNearlyEqual);
	}
}

void UDLTitanApeAnimInstance::UpdateMovementState(float DeltaSeconds)
{
	if (PrevIsTurning && !bIsTurning)
	{
		StopSlotAnimation(0.2f, "TurnOrRotate");
	}

	if (AnimCharacterInfo.MovementState.Grounded())
	{
		const bool bPrevShouldMove = GroundedValues.bShouldMove;
		GroundedValues.bShouldMove = ShouldMoveCheck();
		if (bPrevShouldMove == false && GroundedValues.bShouldMove)
		{
			// 开始移动
			ElapsedDelayTime = 0.0f;
			GroundedValues.bRotateL = false;
			GroundedValues.bRotateR = false;
		}

		if (GroundedValues.bShouldMove)
		{
			UpdateMovementValues(DeltaSeconds);
			UpdateRotationValues();
		}
		else
		{
			if (CanTurnInPlace())
			{
				TurnInPlaceCheck(DeltaSeconds);
			}
			else
			{
				ElapsedDelayTime = 0.0f;
			}
		}
	}
	else if (AnimCharacterInfo.MovementState.InAir())
	{
		// UpdateInAirValues(this, DeltaSeconds);
	}
}

void UDLTitanApeAnimInstance::UpdateMovementValues(float DeltaSeconds)
{
	FAnimVelocityBlend TargetBlend;
	DLAnimInstanceHelper::CalculateVelocityBlend(this, TargetBlend, AnimCharacterInfo.CharacterActorRotation, AnimCharacterInfo.Velocity);

	DL_ANIM_DEBUG_LOG(Log, TEXT("<UDLTitanApeAnimInstance::UpdateMovementValues> OldVelocityBlend:%s")
		, *USTRUCT_TO_STR(VelocityBlend));
	VelocityBlend.Forward = FMath::FInterpTo(VelocityBlend.Forward, TargetBlend.Forward, DeltaSeconds, AnimConfig.VelocityBlendInterpSpeed);
	VelocityBlend.Backward = FMath::FInterpTo(VelocityBlend.Backward, TargetBlend.Backward, DeltaSeconds, AnimConfig.VelocityBlendInterpSpeed);
	VelocityBlend.Left = FMath::FInterpTo(VelocityBlend.Left, TargetBlend.Left, DeltaSeconds, AnimConfig.VelocityBlendInterpSpeed);
	VelocityBlend.Right = FMath::FInterpTo(VelocityBlend.Right, TargetBlend.Right, DeltaSeconds, AnimConfig.VelocityBlendInterpSpeed);

	DL_ANIM_DEBUG_LOG(Log, TEXT("UDLTitanApeAnimInstance::UpdateMovementValues> NewVelocityBlend:%s \tTargetBlend:%s")
		, *UDLAnimationLibrary::ConvertUStructToJson<FAnimVelocityBlend>(VelocityBlend)
		, *UDLAnimationLibrary::ConvertUStructToJson<FAnimVelocityBlend>(TargetBlend));

	// GroundedValues.DiagonalScaleAmount = CalculateDiagonalScaleAmount();

	RelativeAcceleration = DLAnimInstanceHelper::CalculateRelativeAccelerationAmount(this, AnimCharacterInfo.CharacterActorRotation, AnimCharacterInfo.Velocity, AnimCharacterInfo.Acceleration);
	LeanAmount.ForwardBack = FMath::FInterpTo(LeanAmount.ForwardBack
		, FMath::Clamp(RelativeAcceleration.X * AnimConfig.GroundedLeanScale, -1.0f, 1.0f)
		, DeltaSeconds, AnimConfig.GroundedLeanInterpSpeed);

	LeanAmount.LeftRight = FMath::FInterpTo(LeanAmount.LeftRight
		, FMath::Clamp(RelativeAcceleration.Y * AnimConfig.GroundedLeanScale, -1.0f, 1.0f)
		, DeltaSeconds, AnimConfig.GroundedLeanInterpSpeed);

	FVector2D TargetWalkRushBlend;
	GroundedValues.WalkRunBlend = DLAnimInstanceHelper::CalculateSpeedBlend(this, TargetWalkRushBlend, AnimCharacterInfo.Gait
		, VelocityBlend,AnimCharacterInfo.CharacterMainState);

	WalkRushBlend = FMath::Vector2DInterpTo(WalkRushBlend, TargetWalkRushBlend
		, DeltaSeconds, AnimConfig.VelocityBlendInterpSpeed);

	GroundedValues.StrideBlend = DLAnimInstanceHelper::CalculateStrideBlend(this, AnimCharacterInfo.Speed
		, AnimConfig.MovementStateSettings.GetBlendCurveConfigByState(AnimCharacterInfo.CharacterMainState));

	GroundedValues.StandingPlayRate = DLAnimInstanceHelper::CalculateStandingPlayRate(AnimCharacterInfo.Speed, AnimConfig.AnimatedWalkSpeed
		, AnimConfig.AnimatedJogSpeed, AnimConfig.AnimatedSprintSpeed, this, AnimCharacterInfo.Gait, GroundedValues);
}

void UDLTitanApeAnimInstance::UpdateRotationValues()
{
	// YawOffset动画曲线是为了让人物的旋转有更加自然的效果。
	FRotator Delta = AnimCharacterInfo.Velocity.ToOrientationRotator() - AnimCharacterInfo.TargetRotation;
	Delta.Normalize();
	const FVector& FBOffset = AnimConfig.MovementStateSettings.GetBlendCurveConfigByState(AnimCharacterInfo.CharacterMainState)
		.YawOffset_FB->GetVectorValue(Delta.Yaw);
	GroundedValues.ForwardYaw = FBOffset.X;
	GroundedValues.BackwardYaw = FBOffset.Y;
	const FVector& LROffset = AnimConfig.MovementStateSettings.GetBlendCurveConfigByState(AnimCharacterInfo.CharacterMainState)
		.YawOffset_LR->GetVectorValue(Delta.Yaw);
	GroundedValues.LeftYaw = LROffset.X;
	GroundedValues.RightYaw = LROffset.Y;
}

void UDLTitanApeAnimInstance::UpdateAimingValues(float DeltaSeconds)
{
	// 通过瞄准旋转值和actor旋转的差值计算瞄准角度和平滑过渡的瞄准角度。
	FRotator Delta = AnimCharacterInfo.Turn_TargetRotation - AnimCharacterInfo.CharacterActorRotation;
	Delta.Normalize();
	AimingValues.AimingAngle.X = Delta.Yaw;
	AimingValues.AimingAngle.Y = Delta.Pitch;
}

void UDLTitanApeAnimInstance::TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime,
	bool OverrideCurrent)
{
	FRotator Delta = TargetRotation - AnimCharacterInfo.CharacterActorRotation;
	Delta.Normalize();
	const float TurnAngle = Delta.Yaw;

	FDLTurnInPlaceAsset TargetTurnAsset;
	const auto& CurTurnInPlaceConfig = GetCurTurnInPlaceConfig();
	if (FMath::Abs(TurnAngle) < CurTurnInPlaceConfig.Turn180Threshold)
	{
		TargetTurnAsset = TurnAngle < 0.0f
			? CurTurnInPlaceConfig.N_TurnIP_L90
			: CurTurnInPlaceConfig.N_TurnIP_R90;
	}
	else
	{
		TargetTurnAsset = TurnAngle < 0.0f
			? CurTurnInPlaceConfig.N_TurnIP_L180
			: CurTurnInPlaceConfig.N_TurnIP_R180;
	}
	if (!OverrideCurrent && IsPlayingSlotAnimation(TargetTurnAsset.Animation, TargetTurnAsset.SlotName))
	{
		return;
	}

	DL_ANIM_DEBUG_LOG(Log, TEXT("<UDLTitanApeAnimInstance::TurnInPlace> %s[%s] Animation:%s")
		, *TryGetPawnOwner()->GetName(), *ENUM_TO_STRING(TryGetPawnOwner()->GetLocalRole()), *TargetTurnAsset.Animation->GetName());
	PlaySlotAnimationAsDynamicMontage(TargetTurnAsset.Animation, TargetTurnAsset.SlotName, CurTurnInPlaceConfig.BlendInTime
		, CurTurnInPlaceConfig.BlendOutTime, TargetTurnAsset.PlayRate * PlayRateScale
		, 1, 0.0f, StartTime);

	if (TargetTurnAsset.ScaleTurnAngle)
	{
		GroundedValues.RotationScale = (TurnAngle / TargetTurnAsset.AnimatedAngle) * TargetTurnAsset.PlayRate * PlayRateScale;
	}
	else
	{
		GroundedValues.RotationScale = TargetTurnAsset.PlayRate * PlayRateScale;
	}

}

const FDLAnimTurnInPlaceConfig& UDLTitanApeAnimInstance::GetCurTurnInPlaceConfig() const
{
	// if (CurrentCharacterState.HasTag(EAnimationTag::ECharacterState::ENPC::Stage_1))
	// {
	return AnimConfig.TurnInPlaceConfig_Stage_1;
	// }
	// else
	// {
	// 	return AnimConfig.TurnInPlaceConfig_Stage_2_3;
	// }
}

void UDLTitanApeAnimInstance::SetMainState(FHumanAnimCharacterInfo& NewInfo)
{
	//判断主状态
	// if (CurrentCharacterState.HasTagExact(EAnimationTag::ECharacterAnimState::LockTarget))
	if (ICharacterStateAccessor::Execute_K2_IsLockTargetUnit(CharacterStateAccessor.GetObject()))
	{
		NewInfo.CharacterMainState = EAnimCharacterMainState::Locking;
	}
	else
	{
		NewInfo.CharacterMainState = EAnimCharacterMainState::Normal;
	}
}

void UDLTitanApeAnimInstance::SetGait(FHumanAnimCharacterInfo& NewInfo)
{
	//判断行走类型
	if (CurrentCharacterState.HasTagExact(EAnimationTag::ECharacterAnimState::Walk))
	{
		NewInfo.Gait = EAnimGaitType::Walking;
	}
	else if (CurrentCharacterState.HasTagExact(EAnimationTag::ECharacterAnimState::Run))
	{
		NewInfo.Gait = EAnimGaitType::Running;
	}
	else if (CurrentCharacterState.HasTagExact(EAnimationTag::ECharacterAnimState::Sprint))
	{
		NewInfo.Gait = EAnimGaitType::Sprinting;
	}
	else
	{
		NewInfo.Gait = EAnimGaitType::None;
	}

	if (NewInfo.Gait.None())
	{
		NewInfo.LastInputGait = AnimCharacterInfo.LastInputGait;
	}
	else
	{
		NewInfo.LastInputGait = NewInfo.Gait;

		// DL_ANIM_DEBUG_LOG(Log, TEXT("<UDLTitanApeAnimInstance::CopyNewAnimCharacterInfo> 000 LastInputGait:%s")
		// , *ENUM_TO_STRING(PrevAnimCharacterInfo.LastInputGait.AnimGaitType));
	}

	// DL_ANIM_DEBUG_LOG(Log, TEXT("%s[%s] New Gait:%s"), *TryGetPawnOwner()->GetName(), *ENUM_TO_STRING(TryGetPawnOwner()->GetLocalRole()), *ENUM_TO_STRING(NewInfo.Gait.AnimGaitType));
}

void UDLTitanApeAnimInstance::SetTargetRotation(FHumanAnimCharacterInfo& NewInfo)
{

	switch (NewInfo.CharacterMainState.CharacterMainState)
	{
	case EAnimCharacterMainState::None:
		NewInfo.TargetRotation = FRotator::ZeroRotator;
		break;
	case EAnimCharacterMainState::Normal:
		NewInfo.TargetRotation = LastVelocityRotation;
		break;
	case EAnimCharacterMainState::Locking:
	{
		const AActor* LockedTarget =
			ICharacterStateAccessor::Execute_K2_GetLockTargetUnit(CharacterStateAccessor.GetObject());
		if (!ensureMsgf(LockedTarget, TEXT("为什么处于锁定状态却没有锁定目标！！！！")))
		{
			return;
		}
		NewInfo.TargetRotation = UKismetMathLibrary::FindLookAtRotation(TryGetPawnOwner()->GetActorLocation(), LockedTarget->GetActorLocation());
	}
	break;
	}
}

void UDLTitanApeAnimInstance::SetMovementState(FHumanAnimCharacterInfo& NewInfo)
{
	//刷新移动状态
	if (CurrentCharacterState.HasTagExact(EAnimationTag::ECharacterAnimState::InTheLand))
	{
		NewInfo.MovementState = EAnimMovementState::Grounded;
	}
	else if (CurrentCharacterState.HasTagExact(EAnimationTag::ECharacterAnimState::InTheSky))
	{
		NewInfo.MovementState = EAnimMovementState::InAir;
	}
	else
	{
		NewInfo.MovementState = EAnimMovementState::Grounded;
	}
}

void UDLTitanApeAnimInstance::SetMovementDirection(FHumanAnimCharacterInfo& NewInfo)
{
	//设置运动方向
	if (PrevAnimCharacterInfo.MovementDirection.None())
	{
		NewInfo.MovementDirection = UDLAnimationLibrary::CalculateRelativeDirection(TryGetPawnOwner()->GetControlRotation()
			, NewInfo.Velocity);
	}
	else
	{
		const FRotator RelativeRotation = UKismetMathLibrary::NormalizedDeltaRotator(
			NewInfo.Velocity.ToOrientationRotator(), TryGetPawnOwner()->GetControlRotation());

		NewInfo.MovementDirection = UDLAnimationLibrary::CalculateQuadrant(PrevAnimCharacterInfo.MovementDirection
			, 70.0f, -70.0f, 110.0f, -110.0f, 5.0f, RelativeRotation.Yaw);
	}
}

void UDLTitanApeAnimInstance::SetVelocityRelated(FHumanAnimCharacterInfo& NewInfo, float DeltaSeconds)
{
	NewInfo.Velocity = MovementComponentBase->Velocity;

	NewInfo.CharacterActorRotation = GetOwningActor()->GetActorRotation();

	NewInfo.RelativeVelocityDirection = NewInfo.CharacterActorRotation.UnrotateVector(NewInfo.Velocity.GetSafeNormal(0.1f));

	{
		//计算加速度
		CurrentAcceleration = MovementComponentBase->GetCurrentAcceleration();
		EasedMaxAcceleration = MovementComponentBase->GetMaxAcceleration() != 0
			? MovementComponentBase->GetMaxAcceleration() : EasedMaxAcceleration / 2;
		const FVector CurrentVel = NewInfo.Velocity;
		NewInfo.Acceleration = (CurrentVel - PreviousVelocity) / DeltaSeconds;

		//计算移动输入
		NewInfo.MovementInput = CurrentAcceleration;

		//计算速度
		NewInfo.Speed = CurrentVel.Size();

		//是否移动
		NewInfo.bIsMoving = (NewInfo.Speed > 1.0f);

		NewInfo.MovementInputAmount = CurrentAcceleration.Size() / EasedMaxAcceleration;
		NewInfo.bHasMovementInput = (NewInfo.MovementInputAmount > 0.0f);

		if (NewInfo.bIsMoving)
		{
			LastVelocityRotation = CurrentVel.ToOrientationRotator();
		}
	}
}

void UDLTitanApeAnimInstance::SetMovementAction(FHumanAnimCharacterInfo& NewInfo)
{
	if (CurrentCharacterState.HasTagExact(EAnimationTag::ECharacterAnimState::Roll))
	{
		NewInfo.MovementAction = EDLMovementAction::Rolling;
	}
	else
	{
		NewInfo.MovementAction = EDLMovementAction::None;
	}
}

void UDLTitanApeAnimInstance::SetSpecialState(FHumanAnimCharacterInfo& NewInfo)
{
	const float Health = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(
		CharacterStateAccessor.GetObject(), UDLUnitAttributeBaseSet::GetHealthAttribute());
	if (Health <= 0.0f)
	{
		NewInfo.SpecialState = EAnimSpecialState::Dead;
	}
	else
	{
		NewInfo.SpecialState = EAnimSpecialState::None;
	}
}

void UDLTitanApeAnimInstance::OnLanded(const FHitResult& Hit)
{
	const float VelZ = FMath::Abs(TryGetPawnOwner()->GetMovementComponent()->Velocity.Z);

	if (AnimCharacterInfo.bHasMovementInput && VelZ >= AnimConfig.LandConfig.BreakfallOnLandVelocity)
	{
		if (!ensureMsgf(AnimConfig.LandConfig.LandRollMontage, TEXT("为什么不配置落地的翻滚动画！！！")))
		{
			return;
		}
		Montage_Play(AnimConfig.LandConfig.LandRollMontage);
	}
	else
	{
		const ACharacter* Character = Cast<ACharacter>(TryGetPawnOwner());
		PrevBrakingFrictionFactor = Character->GetCharacterMovement()->BrakingFrictionFactor;
		Character->GetCharacterMovement()->BrakingFrictionFactor = AnimCharacterInfo.bHasMovementInput ? 0.5f : 3.0f;

		Character->GetWorldTimerManager().SetTimer(OnLandedFrictionResetTimer, this,
			&UDLTitanApeAnimInstance::OnLandFrictionReset, 0.5f, false);
	}
}

void UDLTitanApeAnimInstance::OnLandFrictionReset() const
{
	const ACharacter* Character = Cast<ACharacter>(TryGetPawnOwner());
	if (Character)
	{
		Character->GetCharacterMovement()->BrakingFrictionFactor = PrevBrakingFrictionFactor;
	}
}