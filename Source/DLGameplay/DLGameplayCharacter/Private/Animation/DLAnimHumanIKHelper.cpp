#include "DLAnimHumanIKHelper.h"

#include "DLAnimCommonLibrary.h"
#include "DLMainAnimInstanceHumanLocomotion.h"
#include "DrawDebugHelpers.h"
#include "DLGameplayCharacter/Public/Animation/AnimDef/DLAnimTag.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void DLAnimHumanIKHelper::UpdateFootIK(UDLMainAnimInstanceHumanLocomotion* AnimIns, float DeltaSeconds)
{
	FVector FootOffsetLTarget = FVector::ZeroVector;
	FVector FootOffsetRTarget = FVector::ZeroVector;

	// 更新Foot Lock 参数。
	SetFootLocking(AnimIns, DeltaSeconds, EDLAnimCurveName::EnableFootIkLeft, EDLAnimCurveName::FootLockLeft,
		EAnimTagDef::EBoneName::L_Foot, AnimIns->FootIKValues.FootLock_L_Alpha, AnimIns->FootIKValues.UseFootLockCurve_L,
AnimIns->FootIKValues.FootLock_L_Location, AnimIns->FootIKValues.FootLock_L_Rotation);
	SetFootLocking(AnimIns, DeltaSeconds, EDLAnimCurveName::EnableFootIkRight, EDLAnimCurveName::FootLockRight,
				   EAnimTagDef::EBoneName::R_Foot, AnimIns->FootIKValues.FootLock_R_Alpha, AnimIns->FootIKValues.UseFootLockCurve_R,
				   AnimIns->FootIKValues.FootLock_R_Location, AnimIns->FootIKValues.FootLock_R_Rotation);

	if (AnimIns->AnimCharacterInfo.MovementState.InAir())
	{
		// 如果在空中，则重设IK 偏移。
		SetPelvisIKOffset(AnimIns, DeltaSeconds, FVector::ZeroVector, FVector::ZeroVector);
		ResetIKOffsets(AnimIns, DeltaSeconds);
	}
	else if (AnimIns->AnimCharacterInfo.MovementState.Ragdoll())
	{
		// 不在空中时并且不处于布娃娃状态时，更新Foot Lock 和脚部偏移
		SetFootOffsets(AnimIns, DeltaSeconds, EDLAnimCurveName::EnableFootIkLeft, EAnimTagDef::EBoneName::L_Foot, EAnimTagDef::EBoneName::Root,
					   FootOffsetLTarget,
					   AnimIns->FootIKValues.FootOffset_L_Location, AnimIns->FootIKValues.FootOffset_L_Rotation);
		SetFootOffsets(AnimIns, DeltaSeconds, EDLAnimCurveName::EnableFootIkRight, EAnimTagDef::EBoneName::R_Foot, EAnimTagDef::EBoneName::Root,
					   FootOffsetRTarget,
					   AnimIns->FootIKValues.FootOffset_R_Location, AnimIns->FootIKValues.FootOffset_R_Rotation);
		SetPelvisIKOffset(AnimIns, DeltaSeconds, FootOffsetLTarget, FootOffsetRTarget);
	}
}

void DLAnimHumanIKHelper::SetFootLocking(UDLMainAnimInstanceHumanLocomotion* AnimIns, float DeltaSeconds,
	EDLAnimCurveName EnableFootIKCurve, EDLAnimCurveName FootLockCurve, FGameplayTag IKFootBone, float& CurFootLockAlpha,
	bool& UseFootLockCurve, FVector& CurFootLockLoc, FRotator& CurFootLockRot)
{
	if (UDLAnimCommonLibrary::GetCurveValueByEnum(AnimIns, EnableFootIKCurve) <= 0.0f)
	{
		return;
	}

	// 第一步：设置Foot Lock 曲线值
	float FootLockCurveVal;

	if (UseFootLockCurve)
	{
		UseFootLockCurve = FMath::Abs(UDLAnimCommonLibrary::GetCurveValueByEnum(AnimIns, EDLAnimCurveName::RotationAmount)) <= 0.001f;
		FootLockCurveVal = UDLAnimCommonLibrary::GetCurveValueByEnum(AnimIns, FootLockCurve) * (1.f / AnimIns->GetSkelMeshComponent()->AnimUpdateRateParams->
			UpdateRate);
	}
	else
	{
		UseFootLockCurve = UDLAnimCommonLibrary::GetCurveValueByEnum(AnimIns, FootLockCurve) >= 0.99f;
		FootLockCurveVal = 0.0f;
	}

	// 第二步：当Foot Lock 曲线值小于当前FootLock差值或者等于一时，更新FootLock差值。
	// 这样能够确保脚只会到达一个锁定的位置或者锁定到一个新的预期位置。
	if (FootLockCurveVal >= 0.99f || FootLockCurveVal < CurFootLockAlpha)
	{
		CurFootLockAlpha = FootLockCurveVal;
	}

	// 第三步：如果Foot Lock 曲线值等于1,保存新的锁定坐标和旋转。
	if (CurFootLockAlpha >= 0.99f)
	{
		const FTransform& OwnerTransform =
			AnimIns->GetOwningComponent()->GetSocketTransform(AnimIns->GetBoneByTag(IKFootBone), RTS_Component);
		CurFootLockLoc = OwnerTransform.GetLocation();
		CurFootLockRot = OwnerTransform.Rotator();
	}

	// 第四步：如果CurFootLockAlpha大于0，更新Foot Lock 偏移来保持脚始终在地面上
	if (CurFootLockAlpha > 0.0f)
	{
		SetFootLockOffsets(AnimIns, DeltaSeconds, CurFootLockLoc, CurFootLockRot);
	}
}

void DLAnimHumanIKHelper::SetFootLockOffsets(UDLMainAnimInstanceHumanLocomotion* AnimIns, float DeltaSeconds,
	FVector& LocalLoc, FRotator& LocalRot)
{
	FRotator RotationDifference = FRotator::ZeroRotator;
	const ACharacter* Character = Cast<ACharacter>(AnimIns->GetOwningActor());
	// 使用当前旋转和上一次旋转的差值得出新的旋转值，以保持脚在地面上
	if (Character->GetCharacterMovement()->IsMovingOnGround())
	{
		RotationDifference = AnimIns->AnimCharacterInfo.CharacterActorRotation -
			Character->GetCharacterMovement()->GetLastUpdateRotation();
		RotationDifference.Normalize();
	}

	// 通过计算两帧间的相对于mesh旋转的移动距离来得出脚需要有多少偏移来锁定在地面上
	const FVector& LocationDifference = AnimIns->GetOwningComponent()->GetComponentRotation().UnrotateVector(
		AnimIns->AnimCharacterInfo.Velocity * DeltaSeconds);

	// 先计算出位置的差值，再计算它绕DownVector进行RotationDifference.Yaw度的旋转值
	LocalLoc = (LocalLoc - LocationDifference).RotateAngleAxis(RotationDifference.Yaw, FVector::DownVector);

	// 计算新的旋转值
	FRotator Delta = LocalRot - RotationDifference;
	Delta.Normalize();
	LocalRot = Delta;
}

void DLAnimHumanIKHelper::SetPelvisIKOffset(UDLMainAnimInstanceHumanLocomotion* AnimIns, float DeltaSeconds,
	FVector FootOffsetLTarget, FVector FootOffsetRTarget)
{
	// 通过计算左脚和右脚的IK权重的平均值来得到盆骨的变化值，如果值为0，清除偏移量。
	AnimIns->FootIKValues.PelvisAlpha =
		(UDLAnimCommonLibrary::GetCurveValueByEnum(AnimIns, EDLAnimCurveName::EnableFootIkLeft)
		+ UDLAnimCommonLibrary::GetCurveValueByEnum(AnimIns, EDLAnimCurveName::EnableFootIkRight)) / 2.0f;

	if (AnimIns->FootIKValues.PelvisAlpha > 0.0f)
	{
		//第一步：设置新的盆骨目标位置为脚部偏移量两种较小的那个
		const FVector PelvisTarget = FootOffsetLTarget.Z < FootOffsetRTarget.Z ? FootOffsetLTarget : FootOffsetRTarget;

		// 第二步：对新值进行插值，不同的速度会有不同的插值量
		const float InterpSpeed = PelvisTarget.Z > AnimIns->FootIKValues.PelvisOffset.Z ? 10.0f : 15.0f;
		AnimIns->FootIKValues.PelvisOffset =
			FMath::VInterpTo(AnimIns->FootIKValues.PelvisOffset, PelvisTarget, DeltaSeconds, InterpSpeed);
	}
	else
	{
		AnimIns->FootIKValues.PelvisOffset = FVector::ZeroVector;
	}
}

void DLAnimHumanIKHelper::ResetIKOffsets(UDLMainAnimInstanceHumanLocomotion* AnimIns, float DeltaSeconds)
{
	// 重设Foot IK 偏移
	AnimIns->FootIKValues.FootOffset_L_Location = FMath::VInterpTo(AnimIns->FootIKValues.FootOffset_L_Location,
														  FVector::ZeroVector, DeltaSeconds, 15.0f);
	AnimIns->FootIKValues.FootOffset_R_Location = FMath::VInterpTo(AnimIns->FootIKValues.FootOffset_R_Location,
														  FVector::ZeroVector, DeltaSeconds, 15.0f);
	AnimIns->FootIKValues.FootOffset_L_Rotation = FMath::RInterpTo(AnimIns->FootIKValues.FootOffset_L_Rotation,
														  FRotator::ZeroRotator, DeltaSeconds, 15.0f);
	AnimIns->FootIKValues.FootOffset_R_Rotation = FMath::RInterpTo(AnimIns->FootIKValues.FootOffset_R_Rotation,
														  FRotator::ZeroRotator, DeltaSeconds, 15.0f);
}

#if WITH_EDITOR
void DrawDebugLineTraceSingle(const UWorld* World, const FVector& Start, const FVector& End,
												  EDrawDebugTrace::Type
												  DrawDebugType,
												  bool bHit,
												  const FHitResult& OutHit,
												  FLinearColor TraceColor,
												  FLinearColor TraceHitColor,
												  float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHit.bBlockingHit)
		{
			::DrawDebugLine(World, Start, OutHit.ImpactPoint, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, OutHit.ImpactPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugPoint(World, OutHit.ImpactPoint, 16.0f, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
}
#endif


void DLAnimHumanIKHelper::SetFootOffsets(UDLMainAnimInstanceHumanLocomotion* AnimIns, float DeltaSeconds,
	EDLAnimCurveName EnableFootIKCurve, FGameplayTag IKFootBone, FGameplayTag RootBone, FVector& CurLocationTarget,
	FVector& CurLocationOffset, FRotator& CurRotationOffset)
{
	// 计算脚部偏移的方法。
	//R点是骨架的root位置，F点是骨架的foot位置，取R点的z值以及F点的x,y值获得P点，P点其实就是F在R所在水平面（地面）上的投影点。
	//从P的正上方A向P的正下方B进行射线检测，I点就是碰撞点，I加上一个沿碰撞法线方向上的常量（足部的高度）获得T点，
	//P点也加上垂直向上的常量FootHeight获得O点。那么向量OT就是偏移向量了。

	// 只有当Foot IK曲线有权重才会更新Foot IK偏移，否则清空偏移量
	if (UDLAnimCommonLibrary::GetCurveValueByEnum(AnimIns, EnableFootIKCurve) <= 0.0f)
	{
		CurLocationOffset = FVector::ZeroVector;
		CurRotationOffset = FRotator::ZeroRotator;
		return;
	}

	// 第一步：从脚部位置向下发射射线，检测地面是否可行走
	USkeletalMeshComponent* OwnerComp = AnimIns->GetOwningComponent();
	FVector IKFootFloorLoc = OwnerComp->GetSocketLocation(AnimIns->GetBoneByTag(IKFootBone));
	IKFootFloorLoc.Z = OwnerComp->GetSocketLocation(AnimIns->GetBoneByTag(RootBone)).Z;

	UWorld* World = AnimIns->GetWorld();
	check(World);

	FCollisionQueryParams Params;
	const ACharacter* Character = Cast<ACharacter>(AnimIns->GetOwningActor());
	Params.AddIgnoredActor(Character);

	const FVector TraceStart = IKFootFloorLoc + FVector(0.0, 0.0, AnimIns->AnimConfig.IK_TraceDistanceAboveFoot);
	const FVector TraceEnd = IKFootFloorLoc - FVector(0.0, 0.0, AnimIns->AnimConfig.IK_TraceDistanceBelowFoot);

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByProfile(HitResult, TraceStart, TraceEnd, "Default", Params);

#if WITH_EDITOR
	DrawDebugLineTraceSingle(World, TraceStart, TraceEnd, EDrawDebugTrace::Type::ForOneFrame
		, bHit, HitResult, FLinearColor::Red, FLinearColor::Green, 5.0f);
#endif

	FRotator TargetRotOffset = FRotator::ZeroRotator;
	if (Character->GetCharacterMovement()->IsWalkable(HitResult))
	{
		FVector ImpactPoint = HitResult.ImpactPoint;
		FVector ImpactNormal = HitResult.ImpactNormal;

		// 计算碰撞点和脚部ik点的差值。
		CurLocationTarget = (ImpactPoint + ImpactNormal * AnimIns->AnimConfig.FootHeight) -
			(IKFootFloorLoc + FVector(0, 0, AnimIns->AnimConfig.FootHeight));

		// Step 1.2: Calculate the Rotation offset by getting the Atan2 of the Impact Normal.
		TargetRotOffset.Pitch = -FMath::RadiansToDegrees(FMath::Atan2(ImpactNormal.X, ImpactNormal.Z));
		TargetRotOffset.Roll = FMath::RadiansToDegrees(FMath::Atan2(ImpactNormal.Y, ImpactNormal.Z));
	}

	// 通过碰撞点的发现计算旋转偏移量。
	const float InterpSpeed = CurLocationOffset.Z > CurLocationTarget.Z ? 30.f : 15.0f;
	CurLocationOffset = FMath::VInterpTo(CurLocationOffset, CurLocationTarget, DeltaSeconds, InterpSpeed);

	// 丝滑的差值处理
	CurRotationOffset = FMath::RInterpTo(CurRotationOffset, TargetRotOffset, DeltaSeconds, 30.0f);
}
