#pragma once

#include "CoreMinimal.h"

enum class EDLAnimCurveName : uint8;
class UDLMainAnimInstanceHumanLocomotion;
struct FGameplayTag;

/*
 * 人类角色的IK大体类似，所以可以单独抽成Helper
 */
class DLAnimHumanIKHelper
{
public:
	static void UpdateFootIK(UDLMainAnimInstanceHumanLocomotion* AnimIns, float DeltaSeconds);

private:
	static void SetFootLocking(UDLMainAnimInstanceHumanLocomotion* AnimIns, float DeltaSeconds, EDLAnimCurveName EnableFootIKCurve, EDLAnimCurveName FootLockCurve
	, FGameplayTag IKFootBone, float& CurFootLockAlpha, bool& UseFootLockCurve, FVector& CurFootLockLoc, FRotator& CurFootLockRot);

	static void SetFootLockOffsets(UDLMainAnimInstanceHumanLocomotion* AnimIns, float DeltaSeconds, FVector& LocalLoc, FRotator& LocalRot);

	static void SetPelvisIKOffset(UDLMainAnimInstanceHumanLocomotion* AnimIns, float DeltaSeconds, FVector FootOffsetLTarget, FVector FootOffsetRTarget);

	static void ResetIKOffsets(UDLMainAnimInstanceHumanLocomotion* AnimIns, float DeltaSeconds);

	static void SetFootOffsets(UDLMainAnimInstanceHumanLocomotion* AnimIns, float DeltaSeconds, EDLAnimCurveName EnableFootIKCurve, FGameplayTag IKFootBone, FGameplayTag RootBone,
						  FVector& CurLocationTarget, FVector& CurLocationOffset, FRotator& CurRotationOffset);

};
