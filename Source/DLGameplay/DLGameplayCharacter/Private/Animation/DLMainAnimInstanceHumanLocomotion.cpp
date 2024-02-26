// Fill out your copyright notice in the Description page of Project Settings.


#include "DLMainAnimInstanceHumanLocomotion.h"

#include "AnimationTestCharacter.h"
#include "DLAnimCommonLibrary.h"
#include "DLAnimHumanIKHelper.h"
#include "Components/CapsuleComponent.h"
#include "Components/DLAnimComponentHumanLocomotion.h"
#include "Curves/CurveVector.h"
#include "DLGameplayCharacter/Public/Animation/AnimDef/DLAnimStruct.h"
#include "DLGameplayCharacter/Public/Animation/AnimDef/DLAnimTag.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::ECharacterAnimState::WalkDirection, "Animation.CharacterAnimState");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::ECharacterAnimState::Walk_Turn, "Animation.CharacterAnimState");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::ECharacterAnimState::Walk_Stop, "Animation.CharacterAnimState");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::ECharacterAnimState::Locked_Walk_Direction, "Animation.CharacterAnimState");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::ECharacterAnimState::Jog_Direction, "Animation.CharacterAnimState");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::ECharacterAnimState::Jog_Turn, "Animation.CharacterAnimState");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::ECharacterAnimState::Jog_Stop, "Animation.CharacterAnimState");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::ECharacterAnimState::Locked_Jog_Direction, "Animation.CharacterAnimState");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::ECharacterAnimState::Accelerate, "Animation.CharacterAnimState");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::ECharacterAnimState::Sprint_Direction, "Animation.CharacterAnimState");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::ECharacterAnimState::Sprint_Turn, "Animation.CharacterAnimState");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::ECharacterAnimState::Sprint_Stop, "Animation.CharacterAnimState");

UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::EBoneName::ik_foot_l, "BoneName");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::EBoneName::ik_foot_r, "BoneName");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::EBoneName::VB_FootTarget_L, "BoneName");
UE_DEFINE_GAMEPLAY_TAG(EAnimTagDef::EBoneName::VB_FootTarget_R, "BoneName");

void UDLMainAnimInstanceHumanLocomotion::OnUpdateAnimation(float DeltaSeconds)
{
	if (FMath::IsNearlyZero(DeltaSeconds))
	{
		DL_ANIM_LOG_WITHOUT_VALUE(Warning, TEXT("<UDLMainAnimInstanceHumanLocomotion::OnUpdateAnimation> Delta seconds is zero"));
		return;
	}

	AnimCharacterInfo.Velocity = GetOwnerCharacter()->GetMovementComponent()->Velocity;
	AnimCharacterInfo.CharacterActorRotation = GetOwnerCharacter()->GetActorRotation();
	AnimCharacterInfo.AimingRotation = Cast<UDLAnimComponentHumanLocomotion>(GetAnimCompFromOwner())->AimingRotation;
	AnimCharacterInfo.MovementInput = Cast<UDLAnimComponentHumanLocomotion>(GetAnimCompFromOwner())->GetMovementInput();

	UpdateAimingValues(DeltaSeconds);
	UpdateLayerValues();
	DLAnimHumanIKHelper::UpdateFootIK(this, DeltaSeconds);

	if (AnimCharacterInfo.MovementState.Grounded())
	{
		const bool bPrevShouldMove = GroundedValues.bShouldMove;
		GroundedValues.bShouldMove = ShouldMoveCheck();
		if (bPrevShouldMove == false && GroundedValues.bShouldMove)
		{
			TurnInPlaceValues.ElapsedDelayTime = 0.0f;
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
			if (CanRotateInPlace())
			{
				RotateInPlaceCheck();
			}
			else
			{
				GroundedValues.bRotateL = false;
				GroundedValues.bRotateR = false;
			}

			if (CanTurnInPlace())
			{
				TurnInPlaceCheck(DeltaSeconds);
			}

			if (CanDynamicTransition())
			{
				DynamicTransitionCheck();
			}
		}
	}
	else if (AnimCharacterInfo.MovementState.InAir())
	{
		UpdateInAirValues(DeltaSeconds);
	}
	else if (AnimCharacterInfo.MovementState.Ragdoll())
	{
		UpdateRagRollValues();
	}

}

void UDLMainAnimInstanceHumanLocomotion::UpdateAimingValues(float DeltaSeconds)
{
	// 对瞄准时的旋转进行丝滑的插值处理。
// 在计算角度之前进行旋转插值是为了确保插值不被actor的旋转所影响，同样也实现了在角色快速旋转时有较慢的瞄准旋转速度。

	AimingValues.SmoothedAimingRotation = FMath::RInterpTo(AimingValues.SmoothedAimingRotation,
														   AnimCharacterInfo.AimingRotation, DeltaSeconds,
														   AnimConfig.SmoothedAimingRotationInterpSpeed);

	// 通过瞄准旋转值和actor旋转的差值计算瞄准角度和平滑过渡的瞄准角度。
	FRotator Delta = AnimCharacterInfo.AimingRotation - AnimCharacterInfo.CharacterActorRotation;
	Delta.Normalize();
	AimingValues.AimingAngle.X = Delta.Yaw;
	AimingValues.AimingAngle.Y = Delta.Pitch;

	Delta = AimingValues.SmoothedAimingRotation - AnimCharacterInfo.CharacterActorRotation;
	Delta.Normalize();
	AimingValues.SmoothedAimingAngle.X = Delta.Yaw;
	AimingValues.SmoothedAimingAngle.Y = Delta.Pitch;

	if (!AnimCharacterInfo.CharacterMainState.Normal())
	{

		AimingValues.AimSweepTime = FMath::GetMappedRangeValueClamped(FVector2f{ -90.0f, 90.0f }, FVector2f{ 1.0f, 0.0f },
																	  AimingValues.AimingAngle.Y);
		// 计算脊柱的旋转来对齐摄像机方向。
		AimingValues.SpineRotation.Roll = 0.0f;
		AimingValues.SpineRotation.Pitch = 0.0f;
		AimingValues.SpineRotation.Yaw = AimingValues.AimingAngle.X / 4.0f;
	}
	else if (AnimCharacterInfo.bHasMovementInput)
	{
		// 通过获取移动输入旋转和actor旋转的差值来调整瞄准偏移，以使角色始终朝向移动输入方向。
		Delta = AnimCharacterInfo.MovementInput.ToOrientationRotator() - AnimCharacterInfo.CharacterActorRotation;
		Delta.Normalize();
		const float InterpTarget = FMath::GetMappedRangeValueClamped(FVector2f{ -180.0f, 180.0f }, FVector2f{ 0.0f, 1.0f }, Delta.Yaw);

		AimingValues.InputYawOffsetTime = FMath::FInterpTo(AimingValues.InputYawOffsetTime, InterpTarget,
														   DeltaSeconds, AnimConfig.InputYawOffsetInterpSpeed);
	}

	// 计算出不同方向的Yaw值。这是为了优化围绕角色旋转时混合瞄准偏移的问题。这样也能够在保持瞄准响应的同时能够丝滑的像各个方向
	// 做旋转的插值混合。
	AimingValues.LeftYawTime = FMath::GetMappedRangeValueClamped(FVector2f{ 0.0f, 180.0f }, FVector2f{ 0.5f, 0.0f },
																 FMath::Abs(AimingValues.SmoothedAimingAngle.X));
	AimingValues.RightYawTime = FMath::GetMappedRangeValueClamped(FVector2f{ 0.0f, 180.0f }, FVector2f{ 0.5f, 1.0f },
																  FMath::Abs(AimingValues.SmoothedAimingAngle.X));
	AimingValues.ForwardYawTime = FMath::GetMappedRangeValueClamped(FVector2f{ -180.0f, 180.0f }, FVector2f{ 0.0f, 1.0f },
																	AimingValues.SmoothedAimingAngle.X);
}

void UDLMainAnimInstanceHumanLocomotion::UpdateLayerValues()
{
	// 通过Aim Offset Mask 曲线获取对应的瞄准偏移权重。
	LayerBlendingValues.EnableAimOffset = FMath::Lerp(1.0f, 0.0f, GetCurveValueByEnum(EDLAnimCurveName::Mask_AimOffset));
	// 设置基础姿势权重。
	LayerBlendingValues.BasePose_N = GetCurveValueByEnum(EDLAnimCurveName::BasePoseStanding);
	// 为身体部位设置叠加动画权重。
	LayerBlendingValues.Spine_Add = GetCurveValueByEnum(EDLAnimCurveName::Layering_Spine_Add);
	LayerBlendingValues.Head_Add = GetCurveValueByEnum(EDLAnimCurveName::Layering_Head_Add);
	LayerBlendingValues.Arm_L_Add = GetCurveValueByEnum(EDLAnimCurveName::Layering_Arm_L_Add);
	LayerBlendingValues.Arm_R_Add = GetCurveValueByEnum(EDLAnimCurveName::Layering_Arm_R_Add);
	// 设置手部姿势叠加权重。
	LayerBlendingValues.Hand_R = GetCurveValueByEnum(EDLAnimCurveName::Layering_Hand_R);
	LayerBlendingValues.Hand_L = GetCurveValueByEnum(EDLAnimCurveName::Layering_Hand_L);
	// 混合并设置手部IK权重来确保只被手臂的层动画所使用。
	LayerBlendingValues.EnableHandIK_L = FMath::Lerp(0.0f, GetCurveValueByEnum(EDLAnimCurveName::Enable_HandIK_L),
													GetCurveValueByEnum(EDLAnimCurveName::Layering_Arm_L));
	LayerBlendingValues.EnableHandIK_R = FMath::Lerp(0.0f, GetCurveValueByEnum(EDLAnimCurveName::Enable_HandIK_R),
													GetCurveValueByEnum(EDLAnimCurveName::Layering_Arm_R));
	// 设置手臂是使用mesh space 或者 local space 进行姿势混合。
	// mesh space 权重总是为1，除非local space 曲线为最大值。
	LayerBlendingValues.Arm_L_LS = GetCurveValueByEnum(EDLAnimCurveName::Layering_Arm_L_LS);
	LayerBlendingValues.Arm_L_MS = static_cast<float>(1 - FMath::FloorToInt(LayerBlendingValues.Arm_L_LS));
	LayerBlendingValues.Arm_R_LS = GetCurveValueByEnum(EDLAnimCurveName::Layering_Arm_R_LS);
	LayerBlendingValues.Arm_R_MS = static_cast<float>(1 - FMath::FloorToInt(LayerBlendingValues.Arm_R_LS));
}

void UDLMainAnimInstanceHumanLocomotion::UpdateMovementValues(float DeltaSeconds)
{
	const FVelocityBlend& TargetBlend = CalculateVelocityBlend();

	DL_ANIM_LOG(Warning, TEXT("<UDLMainAnimInstanceHumanLocomotion::UpdateMovementValues> OldVelocityBlend:%s")
		, *UDLAnimCommonLibrary::ConvertUStructToJson<FVelocityBlend>(VelocityBlend));
	VelocityBlend.Forward = FMath::FInterpTo(VelocityBlend.Forward, TargetBlend.Forward, DeltaSeconds, AnimConfig.VelocityBlendInterpSpeed);
	VelocityBlend.Backward = FMath::FInterpTo(VelocityBlend.Backward, TargetBlend.Backward, DeltaSeconds, AnimConfig.VelocityBlendInterpSpeed);
	VelocityBlend.Left = FMath::FInterpTo(VelocityBlend.Left, TargetBlend.Left, DeltaSeconds, AnimConfig.VelocityBlendInterpSpeed);
	VelocityBlend.Right = FMath::FInterpTo(VelocityBlend.Right, TargetBlend.Right, DeltaSeconds, AnimConfig.VelocityBlendInterpSpeed);

	DL_ANIM_LOG(Warning, TEXT("<UDLMainAnimInstanceHumanLocomotion::UpdateMovementValues> NewVelocityBlend:%s \tTargetBlend:%s")
		, *UDLAnimCommonLibrary::ConvertUStructToJson<FVelocityBlend>(VelocityBlend)
		, *UDLAnimCommonLibrary::ConvertUStructToJson<FVelocityBlend>(TargetBlend));

	GroundedValues.DiagonalScaleAmount = CalculateDiagonalScaleAmount();

	RelativeAccelerationAmount = CalculateRelativeAccelerationAmount();
	LeanAmount.LeftRight = FMath::FInterpTo(LeanAmount.LeftRight, RelativeAccelerationAmount.Y, DeltaSeconds,
									 AnimConfig.GroundedLeanInterpSpeed);
	LeanAmount.ForwardBack = FMath::FInterpTo(LeanAmount.ForwardBack, RelativeAccelerationAmount.X, DeltaSeconds,
									 AnimConfig.GroundedLeanInterpSpeed);

	GroundedValues.WalkRunBlend = CalculateWalkRunBlend();

	GroundedValues.StrideBlend = CalculateStrideBlend();

	GroundedValues.StandingPlayRate = CalculateStandingPlayRate();
}

void UDLMainAnimInstanceHumanLocomotion::UpdateRotationValues()
{
	MovementDirection = CalculateMovementDirection();

	// YawOffset动画曲线是为了让人物的旋转有更加自然的效果。
	FRotator Delta = AnimCharacterInfo.Velocity.ToOrientationRotator() - AnimCharacterInfo.AimingRotation;
	Delta.Normalize();
	const FVector& FBOffset = AnimConfig.GetBlendCurveConfigByState(AnimCharacterInfo.CharacterMainState)
		.YawOffset_FB->GetVectorValue(Delta.Yaw);
	GroundedValues.ForwardYaw = FBOffset.X;
	GroundedValues.BackwardYaw = FBOffset.Y;
	const FVector& LROffset = AnimConfig.GetBlendCurveConfigByState(AnimCharacterInfo.CharacterMainState)
		.YawOffset_LR->GetVectorValue(Delta.Yaw);
	GroundedValues.LeftYaw = LROffset.X;
	GroundedValues.RightYaw = LROffset.Y;
}

bool UDLMainAnimInstanceHumanLocomotion::CanRotateInPlace()
{
	return AnimCharacterInfo.CharacterMainState.Aiming();
}

void UDLMainAnimInstanceHumanLocomotion::RotateInPlaceCheck()
{
	// 检查瞄准的角度是否到达了角色旋转的阈值。
	GroundedValues.bRotateL = AimingValues.AimingAngle.X < RotateInPlaceValues.RotateMinThreshold;
	GroundedValues.bRotateR = AimingValues.AimingAngle.X > RotateInPlaceValues.RotateMaxThreshold;

	// 如果角色可以旋转，使旋转速率缩放到瞄准的Yaw值速率。
	if (GroundedValues.bRotateL || GroundedValues.bRotateR)
	{
		GroundedValues.RotateRate = FMath::GetMappedRangeValueClamped(
			FVector2f{ RotateInPlaceValues.AimYawRateMinRange, RotateInPlaceValues.AimYawRateMaxRange },
			FVector2f{ RotateInPlaceValues.MinPlayRate, RotateInPlaceValues.MaxPlayRate },
			AnimCharacterInfo.AimYawRate);
	}
}

bool UDLMainAnimInstanceHumanLocomotion::CanTurnInPlace()
{
	return AnimCharacterInfo.CharacterMainState.Locking()
		&& GetCurveValueByEnum(EDLAnimCurveName::EnableTransition) > 0.0f;
}

void UDLMainAnimInstanceHumanLocomotion::TurnInPlaceCheck(float DeltaSeconds)
{
	// 如果瞄准Yaw值速率小于瞄准Yaw值速率限制，检查瞄准角度是否达到了转身的阈值。
	// 对ElapsedDelayTime进行调整以保证在转身前能一直通过上述判断。
	if (FMath::Abs(AimingValues.AimingAngle.X) <= AnimConfig.TurnInPlaceConfig.TurnCheckMinAngle ||
		AnimCharacterInfo.AimYawRate >= AnimConfig.TurnInPlaceConfig.AimYawRateLimit)
	{
		TurnInPlaceValues.ElapsedDelayTime = 0.0f;
		return;
	}

	TurnInPlaceValues.ElapsedDelayTime += DeltaSeconds;
	const float ClampedAimAngle = FMath::GetMappedRangeValueClamped(
		FVector2f{ AnimConfig.TurnInPlaceConfig.TurnCheckMinAngle, 180.0f }
		, FVector2f{ AnimConfig.TurnInPlaceConfig.MinAngleDelay,AnimConfig.TurnInPlaceConfig.MaxAngleDelay }
		, AimingValues.AimingAngle.X
	);

	// 检查ElapsedDelayTime是否超过了设置的延迟时间，如果是，则触发转身。
	if (TurnInPlaceValues.ElapsedDelayTime > ClampedAimAngle)
	{
		FRotator TurnInPlaceYawRot = AnimCharacterInfo.AimingRotation;
		TurnInPlaceYawRot.Roll = 0.0f;
		TurnInPlaceYawRot.Pitch = 0.0f;
		TurnInPlace(TurnInPlaceYawRot, 1.0f, 0.0f, false);
	}
}

void UDLMainAnimInstanceHumanLocomotion::TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime,
	bool OverrideCurrent)
{
	FRotator Delta = TargetRotation - AnimCharacterInfo.CharacterActorRotation;
	Delta.Normalize();
	const float TurnAngle = Delta.Yaw;

	FTurnInPlaceAsset TargetTurnAsset;


	if (FMath::Abs(TurnAngle) < AnimConfig.TurnInPlaceConfig.Turn180Threshold)
	{
		TargetTurnAsset = TurnAngle < 0.0f
			? AnimConfig.TurnInPlaceConfig.N_TurnIP_L90
			: AnimConfig.TurnInPlaceConfig.N_TurnIP_R90;
	}
	else
	{
		TargetTurnAsset = TurnAngle < 0.0f
			? AnimConfig.TurnInPlaceConfig.N_TurnIP_L180
			: AnimConfig.TurnInPlaceConfig.N_TurnIP_R180;
	}

	if (!OverrideCurrent && IsPlayingSlotAnimation(TargetTurnAsset.Animation, TargetTurnAsset.SlotName))
	{
		return;
	}
	PlaySlotAnimationAsDynamicMontage(TargetTurnAsset.Animation, TargetTurnAsset.SlotName, 0.2f, 0.2f,
									  TargetTurnAsset.PlayRate * PlayRateScale, 1, 0.0f, StartTime);

	if (TargetTurnAsset.ScaleTurnAngle)
	{
		GroundedValues.RotationScale = (TurnAngle / TargetTurnAsset.AnimatedAngle) * TargetTurnAsset.PlayRate * PlayRateScale;
	}
	else
	{
		GroundedValues.RotationScale = TargetTurnAsset.PlayRate * PlayRateScale;
	}
}

bool UDLMainAnimInstanceHumanLocomotion::CanDynamicTransition()
{
	return GetCurveValueByEnum(EDLAnimCurveName::EnableTransition) >= 0.99f;
}

void UDLMainAnimInstanceHumanLocomotion::DynamicTransitionCheck()
{
	// 检查每个脚的位置的IK_Foot骨骼位置和它的目标位置差值是否在阈值内。
// 如果在阈值内，在脚部播放叠加过渡动画。
// 这里将播放开始时间设置为从中间开始是为了只让单只脚移动。因为只有IK_Foot骨骼会被锁定。
	FTransform SocketTransformA = GetOwningComponent()->GetSocketTransform(
		GetBoneByTag(EAnimTagDef::EBoneName::ik_foot_l), RTS_Component);
	FTransform SocketTransformB = GetOwningComponent()->GetSocketTransform(
		GetBoneByTag(EAnimTagDef::EBoneName::VB_FootTarget_L), RTS_Component);
	float Distance = (SocketTransformB.GetLocation() - SocketTransformA.GetLocation()).Size();
	if (Distance > AnimConfig.DynamicTransitionThreshold)
	{
		FDynamicMontageParams Params;
		Params.Animation = AnimConfig.TransitionSeqConfig.TransitionAnim_R;
		Params.BlendInTime = 0.2f;
		Params.BlendOutTime = 0.2f;
		Params.PlayRate = 1.5f;
		Params.StartTime = 0.8f;
		PlayDynamicTransition(0.1f, Params);
	}

	SocketTransformA = GetOwningComponent()->GetSocketTransform(
		GetBoneByTag(EAnimTagDef::EBoneName::ik_foot_r), RTS_Component);
	SocketTransformB = GetOwningComponent()->GetSocketTransform(
		GetBoneByTag(EAnimTagDef::EBoneName::VB_FootTarget_R), RTS_Component);
	Distance = (SocketTransformB.GetLocation() - SocketTransformA.GetLocation()).Size();
	if (Distance > AnimConfig.DynamicTransitionThreshold)
	{
		FDynamicMontageParams Params;
		Params.Animation = AnimConfig.TransitionSeqConfig.TransitionAnim_L;
		Params.BlendInTime = 0.2f;
		Params.BlendOutTime = 0.2f;
		Params.PlayRate = 1.5f;
		Params.StartTime = 0.8f;
		PlayDynamicTransition(0.1f, Params);
	}
}

void UDLMainAnimInstanceHumanLocomotion::UpdateInAirValues(float DeltaSeconds)
{
	// 仅在空中时设置下落速度。
	InAirValues.FallSpeed = AnimCharacterInfo.Velocity.Z;

	InAirValues.LandPrediction = CalculateLandPrediction();

	const FLeanAmount& InAirLeanAmount = CalculateAirLeanAmount();
	LeanAmount.LeftRight = FMath::FInterpTo(LeanAmount.LeftRight, InAirLeanAmount.LeftRight, DeltaSeconds, AnimConfig.GroundedLeanInterpSpeed);
	LeanAmount.ForwardBack = FMath::FInterpTo(LeanAmount.ForwardBack, InAirLeanAmount.ForwardBack, DeltaSeconds, AnimConfig.GroundedLeanInterpSpeed);

}

void UDLMainAnimInstanceHumanLocomotion::UpdateRagRollValues()
{
	// 速度越快，布娃娃效果就会越强烈。
	const float VelocityLength = GetOwningComponent()->GetPhysicsLinearVelocity(
		GetBoneByTag(EAnimTagDef::EBoneName::Root)
	).Size();
	FlailRate = FMath::GetMappedRangeValueClamped(
		FVector2f{ 0.0f, 1000.0f }, FVector2f{ 0.0f, 1.0f }, VelocityLength);
}

bool UDLMainAnimInstanceHumanLocomotion::ShouldMoveCheck()
{
	//TODO:暂时不用Tag
	// return (!AnimCharacterInfo.HasTag(EAnimTagDef::ECharacterAnimState::Idle) && AnimCharacterInfo.bHasMovementInput)
	// 	|| AnimCharacterInfo.Speed > 150.0f;
	return (AnimCharacterInfo.bHasMovementInput)
		|| AnimCharacterInfo.Speed > 150.0f;
}

void UDLMainAnimInstanceHumanLocomotion::PlayTransition_Implementation(const FDynamicMontageParams& Params)
{
	PlaySlotAnimationAsDynamicMontage(Params.Animation
		, UDLAnimCommonLibrary::GetEnumerationToName<EDLAnimSlot>(EDLAnimSlot::GroundedSlot)
		, Params.BlendInTime, Params.BlendOutTime, Params.PlayRate, 1, 0.0f, Params.StartTime);
}

void UDLMainAnimInstanceHumanLocomotion::PlayTransitionChecked_Implementation(const FDynamicMontageParams& Params)
{
	if (GroundedValues.bShouldMove)
	{
		PlayTransition(Params);
	}
}

void UDLMainAnimInstanceHumanLocomotion::PlayDynamicTransition_Implementation(float ReTriggerDelay, FDynamicMontageParams Params)
{
	if (bCanPlayDynamicTransition)
	{
		bCanPlayDynamicTransition = false;

		PlayTransition(Params);

		UWorld* World = GetWorld();
		ensure(World);
		World->GetTimerManager().SetTimer(PlayDynamicTransitionTimer, this,
										  &UDLMainAnimInstanceHumanLocomotion::PlayDynamicTransitionDelay,
										  ReTriggerDelay, false);
	}
}

FVelocityBlend UDLMainAnimInstanceHumanLocomotion::CalculateVelocityBlend()
{
	// Velocity Blend代表了每个方向的速度。使用它是为了让blend space 有更好的效果。
	const FVector LocRelativeVelocityDir =
		AnimCharacterInfo.CharacterActorRotation.UnrotateVector(AnimCharacterInfo.Velocity.GetSafeNormal(0.1f));
	DL_ANIM_LOG(Warning, TEXT("<CalculateVelocityBlend>  LocRelativeVelocityDir:%s"), *LocRelativeVelocityDir.ToString());
	const float Sum = FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y) +
		FMath::Abs(LocRelativeVelocityDir.Z);
	const FVector RelativeDir = LocRelativeVelocityDir / Sum;
	FVelocityBlend Result;
	Result.Forward = FMath::Clamp(RelativeDir.X, 0.0f, 1.0f);
	Result.Backward = FMath::Abs(FMath::Clamp(RelativeDir.X, -1.0f, 0.0f));
	Result.Left = FMath::Abs(FMath::Clamp(RelativeDir.Y, -1.0f, 0.0f));
	Result.Right = FMath::Clamp(RelativeDir.Y, 0.0f, 1.0f);
	return Result;
}

FVector UDLMainAnimInstanceHumanLocomotion::CalculateRelativeAccelerationAmount()
{
	// Relative Acceleration Amount 表示相对于角色旋转的加速度。
	if (FVector::DotProduct(AnimCharacterInfo.Acceleration, AnimCharacterInfo.Velocity) > 0.0f)
	{
		const float MaxAcc = GetOwnerCharacter()->GetCharacterMovement()->GetMaxAcceleration();
		return AnimCharacterInfo.CharacterActorRotation.UnrotateVector(
			AnimCharacterInfo.Acceleration.GetClampedToMaxSize(MaxAcc) / MaxAcc);
	}

	const float MaxBrakingDec = GetOwnerCharacter()->GetCharacterMovement()->GetMaxBrakingDeceleration();
	return
		AnimCharacterInfo.CharacterActorRotation.UnrotateVector(
			AnimCharacterInfo.Acceleration.GetClampedToMaxSize(MaxBrakingDec) / MaxBrakingDec);
}

float UDLMainAnimInstanceHumanLocomotion::CalculateStrideBlend()
{
	// 计算StrideBlend。用来衡量blend space 中的Stride，使得角色能够以不同的速度跑或走。
	// 也使得在移动速度匹配动画速度的情况下，走或跑的动画能够独立进行混合，防止出现一半走一半跑的情况。
	const float CurveTime = AnimCharacterInfo.Speed / GetOwningComponent()->GetComponentScale().Z;
	const float ClampedGait = GetAnimCurveClamped(EDLAnimCurveName::W_Gait, -1.0, 0.0f, 1.0f);
	const float LerpedStrideBlend =
		FMath::Lerp(AnimConfig.GetBlendCurveConfigByState(AnimCharacterInfo.CharacterMainState).StrideBlend_N_Walk->GetFloatValue(CurveTime)
			, AnimConfig.GetBlendCurveConfigByState(AnimCharacterInfo.CharacterMainState).StrideBlend_N_Run->GetFloatValue(CurveTime)
			, ClampedGait);

	DL_ANIM_LOG(Warning
		, TEXT(R"(<UDLMainAnimInstanceHumanLocomotion::CalculateStrideBlend>CurveTime:%f,ClampedGait:%f,
		WalkCurveValue:%f,RunCurveValue:%f,LerpedStrideBlend:%f)")
		, CurveTime, ClampedGait, AnimConfig.GetBlendCurveConfigByState(AnimCharacterInfo.CharacterMainState)
			.StrideBlend_N_Walk->GetFloatValue(CurveTime)
		, (AnimConfig.GetBlendCurveConfigByState(AnimCharacterInfo.CharacterMainState)
			.StrideBlend_N_Run->GetFloatValue(CurveTime), LerpedStrideBlend));
	return LerpedStrideBlend;
}

float UDLMainAnimInstanceHumanLocomotion::CalculateWalkRunBlend()
{
	//TODO:后续处理
	// if (AnimCharacterInfo.HasAnyTags(FGameplayTagContainer::CreateFromArray<TInlineAllocator<8>>({
	// 	   EAnimTagDef::ECharacterAnimState::Jog_Direction
	// 	   ,EAnimTagDef::ECharacterAnimState::Jog_Turn
	// 	   ,EAnimTagDef::ECharacterAnimState::Jog_Stop
	// 	   ,EAnimTagDef::ECharacterAnimState::Locked_Jog_Direction
	// 	   ,EAnimTagDef::ECharacterAnimState::Accelerate
	// 	   ,EAnimTagDef::ECharacterAnimState::Sprint_Direction
	// 	   ,EAnimTagDef::ECharacterAnimState::Sprint_Turn
	// 	   ,EAnimTagDef::ECharacterAnimState::Sprint_Stop
	// 	})))
	// {
	return  AnimCharacterInfo.Gait.Walking() ? 0.0f : 1.0f;
	// }
	//
	// return 0.0f;
}

float UDLMainAnimInstanceHumanLocomotion::CalculateStandingPlayRate()
{
	// 计算站立状态下的播放速率。计算表达式中加入了StrideBlend，使步伐变小时会有动画播放速度的提升，避免出现滑步的情况。
	const float LerpedSpeed = FMath::Lerp(AnimCharacterInfo.Speed / AnimConfig.AnimatedWalkSpeed,
										  AnimCharacterInfo.Speed / AnimConfig.AnimatedJogSpeed,
										  GetAnimCurveClamped(EDLAnimCurveName::W_Gait, -1.0f, 0.0f, 1.0f));

	float Result;
	if (AnimCharacterInfo.Gait.Sprinting())
	{
		const float SprintAffectedSpeed = FMath::Lerp(LerpedSpeed, AnimCharacterInfo.Speed / AnimConfig.AnimatedSprintSpeed,
												  GetAnimCurveClamped(EDLAnimCurveName::W_Gait, -2.0f, 0.0f, 1.0f));
		Result = FMath::Clamp((SprintAffectedSpeed / GroundedValues.StrideBlend) / GetOwningComponent()->GetComponentScale().Z,
						0.0f, 1.5f);

		DL_ANIM_LOG(Warning
		, TEXT("<UDLMainAnimInstanceHumanLocomotion::CalculateStandingPlayRate>SprintAffectedSpeed:%f")
		, SprintAffectedSpeed);
	}
	else
	{
		Result = FMath::Clamp((LerpedSpeed / GroundedValues.StrideBlend) / GetOwningComponent()->GetComponentScale().Z,
						0.0f, 1.5f);
	}

	DL_ANIM_LOG(Warning
	, TEXT("<UDLMainAnimInstanceHumanLocomotion::CalculateStandingPlayRate>AnimCharacterInfo.Speed:%f,LerpedSpeed:%f")
	, AnimCharacterInfo.Speed, LerpedSpeed);

	DL_ANIM_LOG(Warning
	, TEXT("<UDLMainAnimInstanceHumanLocomotion::CalculateStandingPlayRate>GroundedValues.StrideBlend:%f,Result:%f\n\n")
	, GroundedValues.StrideBlend, Result);

	return Result;
}

float UDLMainAnimInstanceHumanLocomotion::CalculateDiagonalScaleAmount()
{
	// 计算对角线缩放。这个缩放值是为了让角色在在对角线方向移动时有更大可移动距离。
	return AnimConfig.GetBlendCurveConfigByState(AnimCharacterInfo.CharacterMainState)
		.DiagonalScaleAmountCurve->GetFloatValue(FMath::Abs(VelocityBlend.Forward + VelocityBlend.Backward));
}

float UDLMainAnimInstanceHumanLocomotion::CalculateLandPrediction()
{
	// Calculate the land prediction weight by tracing in the velocity direction to find a walkable surface the character
	// is falling toward, and getting the 'Time' (range of 0-1, 1 being maximum, 0 being about to land) till impact.
	// The Land Prediction Curve is used to control how the time affects the final weight for a smooth blend. 
	if (InAirValues.FallSpeed >= -200.0f)
	{
		return 0.0f;
	}

	const UCapsuleComponent* CapsuleComp = GetOwnerCharacter()->GetCapsuleComponent();
	const FVector& CapsuleWorldLoc = CapsuleComp->GetComponentLocation();
	const float VelocityZ = AnimCharacterInfo.Velocity.Z;
	FVector VelocityClamped = AnimCharacterInfo.Velocity;
	VelocityClamped.Z = FMath::Clamp(VelocityZ, -4000.0f, -200.0f);
	VelocityClamped.Normalize();

	const FVector TraceLength = VelocityClamped * FMath::GetMappedRangeValueClamped(
		FVector2f{ 0.0f, -4000.0f }, FVector2f{ 50.0f, 2000.0f }, VelocityZ);

	UWorld* World = GetWorld();
	check(World);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwnerCharacter());

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

	if (GetOwnerCharacter()->GetCharacterMovement()->IsWalkable(HitResult))
	{
		return FMath::Lerp(AnimConfig.GetBlendCurveConfigByState(AnimCharacterInfo.CharacterMainState)
			.LandPredictionCurve->GetFloatValue(HitResult.Time), 0.0f,
GetCurveValueByEnum(EDLAnimCurveName::Mask_LandPrediction));
	}

	return 0.0f;
}

FLeanAmount UDLMainAnimInstanceHumanLocomotion::CalculateAirLeanAmount()
{
	// 通过相对速度矢量来得出角色在空中的倾斜程度。
	FLeanAmount CalcLeanAmount;
	const FVector& UnrotatedVel = AnimCharacterInfo.CharacterActorRotation.UnrotateVector(
		AnimCharacterInfo.Velocity) / 350.0f;
	FVector2D InversedVect(UnrotatedVel.Y, UnrotatedVel.X);
	InversedVect *= AnimConfig.GetBlendCurveConfigByState(AnimCharacterInfo.CharacterMainState)
		.LeanInAirCurve->GetFloatValue(InAirValues.FallSpeed);
	CalcLeanAmount.LeftRight = InversedVect.X;
	CalcLeanAmount.ForwardBack = InversedVect.Y;
	return CalcLeanAmount;
}

EMovementDirection UDLMainAnimInstanceHumanLocomotion::CalculateMovementDirection()
{
	// 计算移动方向。用于角色在观察和瞄准状态下的相对于摄像机的移动方向。
	if (GetIsSprinting() || AnimCharacterInfo.CharacterMainState.Normal())
	{
		return EMovementDirection::Forward;
	}

	FRotator Delta = AnimCharacterInfo.Velocity.ToOrientationRotator() - AnimCharacterInfo.AimingRotation;
	Delta.Normalize();
	return UDLAnimCommonLibrary::CalculateQuadrant(MovementDirection.MovementDirection, 70.0f, -70.0f, 110.0f, -110.0f, 5.0f, Delta.Yaw);

}

float UDLMainAnimInstanceHumanLocomotion::GetAnimCurveClamped(const EDLAnimCurveName Name, float Bias, float ClampMin,
	float ClampMax) const
{
	return FMath::Clamp(GetCurveValueByEnum(Name) + Bias, ClampMin, ClampMax);
}

bool UDLMainAnimInstanceHumanLocomotion::GetIsWalking() const
{
	return AnimCharacterInfo.HasAnyTags(FGameplayTagContainer::CreateFromArray<TInlineAllocator<4>>({
		  EAnimTagDef::ECharacterAnimState::WalkDirection
		  ,EAnimTagDef::ECharacterAnimState::Walk_Turn
		  ,EAnimTagDef::ECharacterAnimState::Walk_Stop
		  ,EAnimTagDef::ECharacterAnimState::Locked_Walk_Direction
		}));
}

bool UDLMainAnimInstanceHumanLocomotion::GetIsJogging() const
{
	return AnimCharacterInfo.HasAnyTags(FGameplayTagContainer::CreateFromArray<TInlineAllocator<4>>({
			  EAnimTagDef::ECharacterAnimState::Jog_Direction
			  ,EAnimTagDef::ECharacterAnimState::Jog_Turn
			  ,EAnimTagDef::ECharacterAnimState::Jog_Stop
			  ,EAnimTagDef::ECharacterAnimState::Locked_Jog_Direction
		}));
}

bool UDLMainAnimInstanceHumanLocomotion::GetIsSprinting() const
{
	return AnimCharacterInfo.HasAnyTags(FGameplayTagContainer::CreateFromArray<TInlineAllocator<4>>({
			   EAnimTagDef::ECharacterAnimState::Accelerate
			   ,EAnimTagDef::ECharacterAnimState::Sprint_Direction
			   ,EAnimTagDef::ECharacterAnimState::Sprint_Turn
			   ,EAnimTagDef::ECharacterAnimState::Sprint_Stop
		}));
}


void UDLMainAnimInstanceHumanLocomotion::PlayDynamicTransitionDelay()
{
	bCanPlayDynamicTransition = true;
}
