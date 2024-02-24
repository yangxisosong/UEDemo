// Fill out your copyright notice in the Description page of Project Settings.


#include "DLHumanAnimInsHelper.h"

#include "DLHumanAnimInstance.h"
#include "Animation/DLAnimationLibrary.h"
#include "Curves/CurveVector.h"
#include "AnimationV2/DLAnimationMacros.h"
#include "AnimationV2/UAnimInstanceHelper.h"
#include "Component/DLCharacterMovementComponentBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

DECLARE_CYCLE_STAT(TEXT("AnimInstanceHuman_UpdateLayerValues"), STAT_AnimInstanceHuman_UpdateLayerValues, STATGROUP_Anim);
DECLARE_CYCLE_STAT(TEXT("AnimInstanceHuman_UpdateMovementValues"), STAT_AnimInstanceHuman_UpdateMovementValues, STATGROUP_Anim);
DECLARE_CYCLE_STAT(TEXT("AnimInstanceHuman_UpdateRotationValues"), STAT_AnimInstanceHuman_UpdateRotationValues, STATGROUP_Anim);
DECLARE_CYCLE_STAT(TEXT("AnimInstanceHuman_UpdateInAirValues"), STAT_AnimInstanceHuman_UpdateInAirValues, STATGROUP_Anim);
DECLARE_CYCLE_STAT(TEXT("AnimInstanceHuman_UpdateRagRollValues"), STAT_AnimInstanceHuman_UpdateRagRollValues, STATGROUP_Anim);

void UDLHumanAnimInsHelper::UpdateLayerValues(UDLHumanAnimInstance* AnimIns)
{
	CHECK_ANIM_INS(AnimIns);
	SCOPE_CYCLE_COUNTER(STAT_AnimInstanceHuman_UpdateLayerValues);

	// // 通过Aim Offset Mask 曲线获取对应的瞄准偏移权重。
	// AnimIns->LayerBlendingValues.EnableAimOffset = FMath::Lerp(1.0f, 0.0f, AnimIns->GetCurveValueByEnum(EDLAnimCurveName::Mask_AimOffset));
	// // 设置基础姿势权重。
	// AnimIns->LayerBlendingValues.BasePose_N = AnimIns->GetCurveValueByEnum(EDLAnimCurveName::BasePoseStanding);
	// // 为身体部位设置叠加动画权重。
	// AnimIns->LayerBlendingValues.Spine_Add = AnimIns->GetCurveValueByEnum(EDLAnimCurveName::Layering_Spine_Add);
	// AnimIns->LayerBlendingValues.Head_Add = AnimIns->GetCurveValueByEnum(EDLAnimCurveName::Layering_Head_Add);
	// AnimIns->LayerBlendingValues.Arm_L_Add = AnimIns->GetCurveValueByEnum(EDLAnimCurveName::Layering_Arm_L_Add);
	// AnimIns->LayerBlendingValues.Arm_R_Add = AnimIns->GetCurveValueByEnum(EDLAnimCurveName::Layering_Arm_R_Add);
	// // 设置手部姿势叠加权重。
	// AnimIns->LayerBlendingValues.Hand_R = AnimIns->GetCurveValueByEnum(EDLAnimCurveName::Layering_Hand_R);
	// AnimIns->LayerBlendingValues.Hand_L = AnimIns->GetCurveValueByEnum(EDLAnimCurveName::Layering_Hand_L);
	// // 混合并设置手部IK权重来确保只被手臂的层动画所使用。
	// AnimIns->LayerBlendingValues.EnableHandIK_L = FMath::Lerp(0.0f, AnimIns->GetCurveValueByEnum(EDLAnimCurveName::Enable_HandIK_L),
	// 												AnimIns->GetCurveValueByEnum(EDLAnimCurveName::Layering_Arm_L));
	// AnimIns->LayerBlendingValues.EnableHandIK_R = FMath::Lerp(0.0f, AnimIns->GetCurveValueByEnum(EDLAnimCurveName::Enable_HandIK_R),
	// 												AnimIns->GetCurveValueByEnum(EDLAnimCurveName::Layering_Arm_R));
	// // 设置手臂是使用mesh space 或者 local space 进行姿势混合。
	// // mesh space 权重总是为1，除非local space 曲线为最大值。
	// AnimIns->LayerBlendingValues.Arm_L_LS = AnimIns->GetCurveValueByEnum(EDLAnimCurveName::Layering_Arm_L_LS);
	// AnimIns->LayerBlendingValues.Arm_L_MS = static_cast<float>(1 - FMath::FloorToInt(AnimIns->LayerBlendingValues.Arm_L_LS));
	// AnimIns->LayerBlendingValues.Arm_R_LS = AnimIns->GetCurveValueByEnum(EDLAnimCurveName::Layering_Arm_R_LS);
	// AnimIns->LayerBlendingValues.Arm_R_MS = static_cast<float>(1 - FMath::FloorToInt(AnimIns->LayerBlendingValues.Arm_R_LS));
}

void UDLHumanAnimInsHelper::UpdateMovementValues(UDLHumanAnimInstance* AnimIns, const float DeltaSeconds)
{
	CHECK_ANIM_INS(AnimIns);

	SCOPE_CYCLE_COUNTER(STAT_AnimInstanceHuman_UpdateMovementValues);

	FAnimVelocityBlend TargetBlend;
	CalculateVelocityBlend(AnimIns, TargetBlend);

	DL_ANIM_DEBUG_LOG(Verbose, TEXT("<UDLHumanAnimInsHelper::UpdateMovementValues> OldAnimIns->VelocityBlend:%s")
		, *UDLAnimationLibrary::ConvertUStructToJson<FAnimVelocityBlend>(AnimIns->VelocityBlend));
	AnimIns->VelocityBlend.Forward = FMath::FInterpTo(AnimIns->VelocityBlend.Forward, TargetBlend.Forward, DeltaSeconds, AnimIns->AnimConfig.VelocityBlendInterpSpeed);
	AnimIns->VelocityBlend.Backward = FMath::FInterpTo(AnimIns->VelocityBlend.Backward, TargetBlend.Backward, DeltaSeconds, AnimIns->AnimConfig.VelocityBlendInterpSpeed);
	AnimIns->VelocityBlend.Left = FMath::FInterpTo(AnimIns->VelocityBlend.Left, TargetBlend.Left, DeltaSeconds, AnimIns->AnimConfig.VelocityBlendInterpSpeed);
	AnimIns->VelocityBlend.Right = FMath::FInterpTo(AnimIns->VelocityBlend.Right, TargetBlend.Right, DeltaSeconds, AnimIns->AnimConfig.VelocityBlendInterpSpeed);

	DL_ANIM_DEBUG_LOG(Verbose, TEXT("<UDLHumanAnimInsHelper::UpdateMovementValues> NewAnimIns->VelocityBlend:%s \tTargetBlend:%s")
		, *UDLAnimationLibrary::ConvertUStructToJson<FAnimVelocityBlend>(AnimIns->VelocityBlend)
		, *UDLAnimationLibrary::ConvertUStructToJson<FAnimVelocityBlend>(TargetBlend));

	// AnimIns->GroundedValues.DiagonalScaleAmount = AnimIns->CalculateDiagonalScaleAmount();

	AnimIns->RelativeAcceleration = CalculateRelativeAccelerationAmount(AnimIns);
	AnimIns->LeanAmount.ForwardBack = FMath::FInterpTo(AnimIns->LeanAmount.ForwardBack
		, FMath::Clamp(AnimIns->RelativeAcceleration.X * AnimIns->AnimConfig.GroundedLeanScale, -1.0f, 1.0f)
		, DeltaSeconds, AnimIns->AnimConfig.GroundedLeanInterpSpeed);

	AnimIns->LeanAmount.LeftRight = FMath::FInterpTo(AnimIns->LeanAmount.LeftRight
		, FMath::Clamp(AnimIns->RelativeAcceleration.Y * AnimIns->AnimConfig.GroundedLeanScale, -1.0f, 1.0f)
		, DeltaSeconds, AnimIns->AnimConfig.GroundedLeanInterpSpeed);

	FVector2D TargetWalkRushBlend;
	AnimIns->GroundedValues.WalkRunBlend = CalculateSpeedBlend(AnimIns, TargetWalkRushBlend);

	DLAnimInstanceHelper::CalculateSpeedBlend(AnimIns, TargetWalkRushBlend, AnimIns->AnimCharacterInfo.Gait
		, AnimIns->VelocityBlend, AnimIns->AnimCharacterInfo.CharacterMainState);

	AnimIns->WalkRushBlend = FMath::Vector2DInterpTo(AnimIns->WalkRushBlend, TargetWalkRushBlend
		, DeltaSeconds, AnimIns->AnimConfig.VelocityBlendInterpSpeed);

	AnimIns->GroundedValues.StrideBlend = CalculateStrideBlend(AnimIns);

	AnimIns->GroundedValues.StandingPlayRate = CalculateStandingPlayRate(AnimIns);
}

void UDLHumanAnimInsHelper::UpdateRotationValues(UDLHumanAnimInstance* AnimIns)
{
	CHECK_ANIM_INS(AnimIns);

	SCOPE_CYCLE_COUNTER(STAT_AnimInstanceHuman_UpdateRotationValues);

	// YawOffset动画曲线是为了让人物的旋转有更加自然的效果。
	FRotator Delta = AnimIns->AnimCharacterInfo.Velocity.ToOrientationRotator() - AnimIns->AnimCharacterInfo.TargetRotation;
	Delta.Normalize();
	const FVector& FBOffset = AnimIns->AnimConfig.MovementStateSettings.GetBlendCurveConfigByState(AnimIns->AnimCharacterInfo.CharacterMainState)
		.YawOffset_FB->GetVectorValue(Delta.Yaw);
	AnimIns->GroundedValues.ForwardYaw = FBOffset.X;
	AnimIns->GroundedValues.BackwardYaw = FBOffset.Y;
	const FVector& LROffset = AnimIns->AnimConfig.MovementStateSettings.GetBlendCurveConfigByState(AnimIns->AnimCharacterInfo.CharacterMainState)
		.YawOffset_LR->GetVectorValue(Delta.Yaw);
	AnimIns->GroundedValues.LeftYaw = LROffset.X;
	AnimIns->GroundedValues.RightYaw = LROffset.Y;
}

void UDLHumanAnimInsHelper::UpdateInAirValues(UDLHumanAnimInstance* AnimIns, const float DeltaSeconds)
{
	CHECK_ANIM_INS(AnimIns);

	SCOPE_CYCLE_COUNTER(STAT_AnimInstanceHuman_UpdateInAirValues);

	// 仅在空中时设置下落速度。
	AnimIns->InAirValues.FallSpeed = AnimIns->AnimCharacterInfo.Velocity.Z;

	AnimIns->InAirValues.LandPrediction = CalculateLandPrediction(AnimIns);

	const FBodyLeanAmount& InAirLeanAmount = CalculateAirLeanAmount(AnimIns);
	AnimIns->LeanAmount.LeftRight = FMath::FInterpTo(AnimIns->LeanAmount.LeftRight, InAirLeanAmount.LeftRight, DeltaSeconds, AnimIns->AnimConfig.GroundedLeanInterpSpeed);
	AnimIns->LeanAmount.ForwardBack = FMath::FInterpTo(AnimIns->LeanAmount.ForwardBack, InAirLeanAmount.ForwardBack, DeltaSeconds, AnimIns->AnimConfig.GroundedLeanInterpSpeed);

}

void UDLHumanAnimInsHelper::CalculateVelocityBlend(UDLHumanAnimInstance* AnimIns, FAnimVelocityBlend& VelocityBlend)
{
	// Velocity Blend代表了每个方向的速度。使用它是为了让blend space 有更好的效果。
	const FVector LocRelativeVelocityDir =
		AnimIns->AnimCharacterInfo.CharacterActorRotation.UnrotateVector(AnimIns->AnimCharacterInfo.Velocity.GetSafeNormal(0.1f));
	DL_ANIM_DEBUG_LOG(Warning, TEXT("<CalculateVelocityBlend> LocRelativeVelocityDir:%s"), *LocRelativeVelocityDir.ToString());
	const float Sum = FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y) +
		FMath::Abs(LocRelativeVelocityDir.Z);
	const FVector RelativeDir = LocRelativeVelocityDir / Sum;
	VelocityBlend.Forward = FMath::Clamp(RelativeDir.X, 0.0f, 1.0f);
	VelocityBlend.Backward = FMath::Abs(FMath::Clamp(RelativeDir.X, -1.0f, 0.0f));
	VelocityBlend.Left = FMath::Abs(FMath::Clamp(RelativeDir.Y, -1.0f, 0.0f));
	VelocityBlend.Right = FMath::Clamp(RelativeDir.Y, 0.0f, 1.0f);
}

FVector UDLHumanAnimInsHelper::CalculateRelativeAccelerationAmount(UDLHumanAnimInstance* AnimIns)
{
	// Relative Acceleration Amount 表示相对于角色旋转的加速度。
	if (FVector::DotProduct(AnimIns->AnimCharacterInfo.Acceleration, AnimIns->AnimCharacterInfo.Velocity) > 0.0f)
	{
		const float MaxAcc = AnimIns->GetMovementComponentBase()->GetMaxAcceleration();
		return AnimIns->AnimCharacterInfo.CharacterActorRotation.UnrotateVector(
			AnimIns->AnimCharacterInfo.Acceleration.GetClampedToMaxSize(MaxAcc) / MaxAcc);
	}

	const float MaxBrakingDec = AnimIns->GetMovementComponentBase()->GetMaxBrakingDeceleration();
	return
		AnimIns->AnimCharacterInfo.CharacterActorRotation.UnrotateVector(
			AnimIns->AnimCharacterInfo.Acceleration.GetClampedToMaxSize(MaxBrakingDec) / MaxBrakingDec);
}

float UDLHumanAnimInsHelper::CalculateSpeedBlend(UDLHumanAnimInstance* AnimIns, FVector2D& TargetWalkRushBlend)
{
	float Speed;
	switch (AnimIns->AnimCharacterInfo.Gait)
	{
	case EAnimGaitType::Walking:
		Speed = 1.0f;
		break;
	case EAnimGaitType::Running:
		Speed = 2.0f;
		break;
	case EAnimGaitType::Sprinting:
		Speed = 3.0f;
		break;
	case EAnimGaitType::None:
	default:
		Speed = 0.0f;
	}

	if (AnimIns->AnimCharacterInfo.CharacterMainState.Locking())
	{
		FAnimVelocityBlend VelocityBlend;
		VelocityBlend.Forward = AnimIns->VelocityBlend.Forward;
		VelocityBlend.Backward = -AnimIns->VelocityBlend.Backward;
		VelocityBlend.Left = -AnimIns->VelocityBlend.Left;
		VelocityBlend.Right = AnimIns->VelocityBlend.Right;

		TargetWalkRushBlend = FVector2D(VelocityBlend.Forward + VelocityBlend.Backward
		, VelocityBlend.Left + VelocityBlend.Right) * Speed;
	}
	else
	{
		TargetWalkRushBlend = FVector2D(1.0f) * Speed;
	}

	return Speed;
}

float UDLHumanAnimInsHelper::CalculateStrideBlend(UDLHumanAnimInstance* AnimIns)
{
	// 计算StrideBlend。用来衡量blend space 中的Stride，使得角色能够以不同的速度跑或走。
// 也使得在移动速度匹配动画速度的情况下，走或跑的动画能够独立进行混合，防止出现一半走一半跑的情况。
	const float CurveTime = AnimIns->AnimCharacterInfo.Speed / AnimIns->GetOwningComponent()->GetComponentScale().Z;
	const float ClampedGait = AnimIns->GetAnimCurveClamped("W_Gait", -1.0, 0.0f, 1.0f);
	const float LerpedStrideBlend =
		FMath::Lerp(AnimIns->AnimConfig.MovementStateSettings.GetBlendCurveConfigByState(AnimIns->AnimCharacterInfo.CharacterMainState).StrideBlend_N_Walk->GetFloatValue(CurveTime)
			, AnimIns->AnimConfig.MovementStateSettings.GetBlendCurveConfigByState(AnimIns->AnimCharacterInfo.CharacterMainState).StrideBlend_N_Run->GetFloatValue(CurveTime)
			, ClampedGait);

	DL_ANIM_DEBUG_LOG(Warning
		, TEXT(R"(<CalculateStrideBlend>CurveTime:%f,ClampedGait:%f,
		WalkCurveValue:%f,RunCurveValue:%f,LerpedStrideBlend:%f)")
		, CurveTime, ClampedGait, AnimIns->AnimConfig.MovementStateSettings.GetBlendCurveConfigByState(AnimIns->AnimCharacterInfo.CharacterMainState)
			.StrideBlend_N_Walk->GetFloatValue(CurveTime)
		, (AnimIns->AnimConfig.MovementStateSettings.GetBlendCurveConfigByState(AnimIns->AnimCharacterInfo.CharacterMainState)
			.StrideBlend_N_Run->GetFloatValue(CurveTime), LerpedStrideBlend));
	return LerpedStrideBlend;
}

float UDLHumanAnimInsHelper::CalculateStandingPlayRate(UDLHumanAnimInstance* AnimIns)
{
	// 计算站立状态下的播放速率。计算表达式中加入了StrideBlend，使步伐变小时会有动画播放速度的提升，避免出现滑步的情况。
	const float LerpedSpeed = FMath::Lerp(AnimIns->AnimCharacterInfo.Speed / AnimIns->AnimConfig.AnimatedWalkSpeed,
										  AnimIns->AnimCharacterInfo.Speed / AnimIns->AnimConfig.AnimatedJogSpeed,
										  AnimIns->GetAnimCurveClamped("W_Gait", -1.0f, 0.0f, 1.0f));

	float Result;
	const float MaxSpeed = AnimIns->MovementComponentBase->GetMaxSpeed();
	if (AnimIns->AnimCharacterInfo.Gait.Sprinting())
	{
		const float SprintAffectedSpeed = FMath::Lerp(AnimIns->AnimCharacterInfo.Speed / MaxSpeed
			, MaxSpeed / AnimIns->AnimConfig.AnimatedSprintSpeed
			, AnimIns->GetAnimCurveClamped("W_Gait", -2.0f, 0.0f, 1.0f));

		Result = FMath::Clamp((SprintAffectedSpeed / AnimIns->GroundedValues.StrideBlend) / AnimIns->GetOwningComponent()->GetComponentScale().Z,
						0.0f, 3.0f);
		DL_ANIM_DEBUG_LOG(Log
		, TEXT("<CalculateStandingPlayRate  Sprinting>W_Gait:%f"), AnimIns->GetAnimCurveClamped("W_Gait", -2.0f, 0.0f, 1.0f));
		DL_ANIM_DEBUG_LOG(Log
		, TEXT("<CalculateStandingPlayRate  Sprinting>SprintAffectedSpeed:%f,StrideBlend:%f,Result:%f")
		, SprintAffectedSpeed, AnimIns->GroundedValues.StrideBlend, Result);
	}
	else
	{
		Result = FMath::Clamp((LerpedSpeed / AnimIns->GroundedValues.StrideBlend) / AnimIns->GetOwningComponent()->GetComponentScale().Z,
						0.0f, 1.5f);

		DL_ANIM_DEBUG_LOG(Log
		, TEXT("<CalculateStandingPlayRate>W_Gait:%f"), AnimIns->GetAnimCurveClamped("W_Gait", -1.0f, 0.0f, 1.0f));
		DL_ANIM_DEBUG_LOG(Log
			, TEXT("<CalculateStandingPlayRate>AnimCharacterInfo.Speed:%f,LerpedSpeed:%f,Result:%f")
			, AnimIns->AnimCharacterInfo.Speed, LerpedSpeed, Result);
	}



	return Result;
}

const float UDLHumanAnimInsHelper::CalculateLandPrediction(UDLHumanAnimInstance* AnimIns)
{
	// Calculate the land prediction weight by tracing in the velocity direction to find a walkable surface the character
	// is falling toward, and getting the 'Time' (range of 0-1, 1 being maximum, 0 being about to land) till impact.
	// The Land Prediction Curve is used to control how the time affects the final weight for a smooth blend. 
	if (AnimIns->InAirValues.FallSpeed >= -200.0f)
	{
		return 0.0f;
	}

	const ACharacter* Character = Cast<ACharacter>(AnimIns->TryGetPawnOwner());
	const UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent();
	const FVector& CapsuleWorldLoc = CapsuleComp->GetComponentLocation();
	const float VelocityZ = AnimIns->AnimCharacterInfo.Velocity.Z;
	FVector VelocityClamped = AnimIns->AnimCharacterInfo.Velocity;
	VelocityClamped.Z = FMath::Clamp(VelocityZ, -4000.0f, -200.0f);
	VelocityClamped.Normalize();

	const FVector TraceLength = VelocityClamped * FMath::GetMappedRangeValueClamped(
		{ 0.0f, -4000.0f }, { 50.0f, 2000.0f }, VelocityZ);

	UWorld* World = AnimIns->GetWorld();
	ensureMsgf(World, TEXT("World is invalid!!!"));

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	FHitResult HitResult;
	const FCollisionShape CapsuleCollisionShape = FCollisionShape::MakeCapsule(CapsuleComp->GetUnscaledCapsuleRadius(),
																			   CapsuleComp->
																			   GetUnscaledCapsuleHalfHeight());
	const float HalfHeight = 0.0f;
	const bool bHit = World->SweepSingleByChannel(HitResult, CapsuleWorldLoc, CapsuleWorldLoc + TraceLength,
												  FQuat::Identity,
												  ECC_Visibility, CapsuleCollisionShape, Params);

	// if (ALSDebugComponent && ALSDebugComponent->GetShowTraces())
	// {
	// 	UALSDebugComponent::DrawDebugCapsuleTraceSingle(World,
	// 													CapsuleWorldLoc,
	// 													CapsuleWorldLoc + TraceLength,
	// 													CapsuleCollisionShape,
	// 													EDrawDebugTrace::Type::ForOneFrame,
	// 													bHit,
	// 													HitResult,
	// 													FLinearColor::Red,
	// 													FLinearColor::Green,
	// 													5.0f);
	// }

	if (Character->GetCharacterMovement()->IsWalkable(HitResult))
	{
		return FMath::Lerp(AnimIns->AnimConfig.MovementStateSettings.GetBlendCurveConfigByState(AnimIns->AnimCharacterInfo.CharacterMainState)
			.LandPredictionCurve->GetFloatValue(HitResult.Time), 0.0f, AnimIns->GetCurveValue("Mask_LandPrediction"));
	}

	return 0.0f;
}

const FBodyLeanAmount UDLHumanAnimInsHelper::CalculateAirLeanAmount(UDLHumanAnimInstance* AnimIns)
{
	// 通过相对速度矢量来得出角色在空中的倾斜程度。
	FBodyLeanAmount CalcLeanAmount;
	const FVector& UnrotatedVel = AnimIns->AnimCharacterInfo.CharacterActorRotation.UnrotateVector(
		AnimIns->AnimCharacterInfo.Velocity) / 350.0f;
	FVector2D InversedVect(UnrotatedVel.Y, UnrotatedVel.X);
	InversedVect *= AnimIns->AnimConfig.MovementStateSettings.GetBlendCurveConfigByState(AnimIns->AnimCharacterInfo.CharacterMainState)
		.LeanInAirCurve->GetFloatValue(AnimIns->InAirValues.FallSpeed);
	CalcLeanAmount.LeftRight = InversedVect.X;
	CalcLeanAmount.ForwardBack = InversedVect.Y;
	return CalcLeanAmount;
}
