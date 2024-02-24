// Fill out your copyright notice in the Description page of Project Settings.


#include "UAnimInstanceHelper.h"

#include "ConvertString.h"
#include "DLAnimationMacros.h"
#include "Animation/DLAnimInstanceBaseV2.h"
#include "AnimInstance/Human/DLHumanAnimDef.h"
#include "Component/DLCharacterMovementComponentBase.h"

void DLAnimInstanceHelper::CalculateVelocityBlend(const UDLAnimInstanceBaseV2* AnimIns,
	FAnimVelocityBlend& VelocityBlend, const FRotator& CharacterActorRotation, const FVector& Velocity)
{
	// Velocity Blend代表了每个方向的速度。使用它是为了让blend space 有更好的效果。
	const FVector LocRelativeVelocityDir =
		CharacterActorRotation.UnrotateVector(Velocity.GetSafeNormal(0.1f));
	DL_ANIM_DEBUG_LOG(Warning, TEXT("<DLAnimInstanceHelper::CalculateVelocityBlend> LocRelativeVelocityDir:%s"), *LocRelativeVelocityDir.ToString());
	const float Sum = FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y) +
		FMath::Abs(LocRelativeVelocityDir.Z);
	const FVector RelativeDir = LocRelativeVelocityDir / Sum;
	VelocityBlend.Forward = FMath::Clamp(RelativeDir.X, 0.0f, 1.0f);
	VelocityBlend.Backward = FMath::Abs(FMath::Clamp(RelativeDir.X, -1.0f, 0.0f));
	VelocityBlend.Left = FMath::Abs(FMath::Clamp(RelativeDir.Y, -1.0f, 0.0f));
	VelocityBlend.Right = FMath::Clamp(RelativeDir.Y, 0.0f, 1.0f);
}

FVector DLAnimInstanceHelper::CalculateRelativeAccelerationAmount(const UDLAnimInstanceBaseV2* AnimIns,
	const FRotator& CharacterActorRotation, const FVector& Velocity, const FVector& Acceleration)
{
	// Relative Acceleration Amount 表示相对于角色旋转的加速度。
	if (FVector::DotProduct(Acceleration, Velocity) > 0.0f)
	{
		const float MaxAcc = AnimIns->GetMovementComponentBase()->GetMaxAcceleration();
		return CharacterActorRotation.UnrotateVector(
			Acceleration.GetClampedToMaxSize(MaxAcc) / MaxAcc);
	}

	const float MaxBrakingDec = AnimIns->GetMovementComponentBase()->GetMaxBrakingDeceleration();
	return
		CharacterActorRotation.UnrotateVector(
			Acceleration.GetClampedToMaxSize(MaxBrakingDec) / MaxBrakingDec);
}

float DLAnimInstanceHelper::CalculateStrideBlend(const UDLAnimInstanceBaseV2* AnimIns, const float Speed,
	const FAnimBlendCurves& BlendCurveConfig)
{
	// 计算StrideBlend。用来衡量blend space 中的Stride，使得角色能够以不同的速度跑或走。
// 也使得在移动速度匹配动画速度的情况下，走或跑的动画能够独立进行混合，防止出现一半走一半跑的情况。
	const float CurveTime = Speed / AnimIns->GetOwningComponent()->GetComponentScale().Z;
	const float ClampedGait = AnimIns->GetAnimCurveClamped("W_Gait", -1.0, 0.0f, 1.0f);
	const float LerpedStrideBlend =
		FMath::Lerp(BlendCurveConfig.StrideBlend_N_Walk->GetFloatValue(CurveTime)
			, BlendCurveConfig.StrideBlend_N_Run->GetFloatValue(CurveTime)
			, ClampedGait);

	DL_ANIM_DEBUG_LOG(Warning
		, TEXT(R"(<DLAnimInstanceHelper::CalculateStrideBlend>CurveTime:%f,ClampedGait:%f,
		WalkCurveValue:%f,RunCurveValue:%f,LerpedStrideBlend:%f)")
		, CurveTime, ClampedGait, BlendCurveConfig.StrideBlend_N_Walk->GetFloatValue(CurveTime)
		, (BlendCurveConfig.StrideBlend_N_Run->GetFloatValue(CurveTime), LerpedStrideBlend));
	return LerpedStrideBlend;

}

float DLAnimInstanceHelper::CalculateStandingPlayRate(const float Speed, const float AnimatedWalkSpeed,
	const float AnimatedJogSpeed, const float AnimatedSprintSpeed, const UDLAnimInstanceBaseV2* AnimIns,
	const FAnimGaitType& Gait, const FAnimGraphGroundedValues& GroundedValues)
{
	// 计算站立状态下的播放速率。计算表达式中加入了StrideBlend，使步伐变小时会有动画播放速度的提升，避免出现滑步的情况。
	const float LerpedSpeed = FMath::Lerp(Speed / AnimatedWalkSpeed,
										 Speed / AnimatedJogSpeed,
										 AnimIns->GetAnimCurveClamped("W_Gait", -1.0f, 0.0f, 1.0f));

	float Result;
	const float MaxSpeed = AnimIns->GetMovementComponentBase()->GetMaxSpeed();
	if (Gait.Sprinting())
	{
		const float SprintAffectedSpeed = FMath::Lerp(Speed / MaxSpeed
			, MaxSpeed / AnimatedSprintSpeed
			, AnimIns->GetAnimCurveClamped("W_Gait", -2.0f, 0.0f, 1.0f));

		Result = FMath::Clamp((SprintAffectedSpeed / GroundedValues.StrideBlend) / AnimIns->GetOwningComponent()->GetComponentScale().Z,
						0.0f, 3.0f);
		DL_ANIM_DEBUG_LOG(Log
		, TEXT("<DLAnimInstanceHelper::CalculateStandingPlayRate  Sprinting>W_Gait:%f"), AnimIns->GetAnimCurveClamped("W_Gait", -2.0f, 0.0f, 1.0f));
		DL_ANIM_DEBUG_LOG(Log
		, TEXT("<DLAnimInstanceHelper::CalculateStandingPlayRate  Sprinting>SprintAffectedSpeed:%f,StrideBlend:%f,Result:%f")
		, SprintAffectedSpeed, GroundedValues.StrideBlend, Result);
	}
	else
	{
		Result = FMath::Clamp((LerpedSpeed / GroundedValues.StrideBlend) / AnimIns->GetOwningComponent()->GetComponentScale().Z,
						0.0f, 1.5f);

		DL_ANIM_DEBUG_LOG(Log
		, TEXT("<DLAnimInstanceHelper::CalculateStandingPlayRate>W_Gait:%f"), AnimIns->GetAnimCurveClamped("W_Gait", -1.0f, 0.0f, 1.0f));
		DL_ANIM_DEBUG_LOG(Log
			, TEXT("<DLAnimInstanceHelper::CalculateStandingPlayRate>AnimCharacterInfo.Speed:%f,LerpedSpeed:%f,Result:%f")
			, Speed, LerpedSpeed, Result);
	}
	return Result;

}

float DLAnimInstanceHelper::CalculateSpeedBlend(const UDLAnimInstanceBaseV2* AnimIns, FVector2D& TargetWalkRushBlend,
	const FAnimGaitType& Gait, const FAnimVelocityBlend& InVelocityBlend, const FAnimCharacterMainState& CharacterMainState)
{
	float Speed;
	switch (Gait)
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

	if (CharacterMainState.Locking())
	{
		FAnimVelocityBlend VelocityBlend;
		VelocityBlend.Forward = InVelocityBlend.Forward;
		VelocityBlend.Backward = -InVelocityBlend.Backward;
		VelocityBlend.Left = -InVelocityBlend.Left;
		VelocityBlend.Right = InVelocityBlend.Right;

		TargetWalkRushBlend = FVector2D(VelocityBlend.Forward + VelocityBlend.Backward
		, VelocityBlend.Left + VelocityBlend.Right) * Speed;
	}
	else
	{
		TargetWalkRushBlend = FVector2D(1.0f) * Speed;
	}

	return Speed;
}
