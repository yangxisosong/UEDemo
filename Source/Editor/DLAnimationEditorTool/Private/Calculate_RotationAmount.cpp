// Fill out your copyright notice in the Description page of Project Settings.


#include "Calculate_RotationAmount.h"
#include "AnimationBlueprintLibrary.h"

void UCalculate_RotationAmount::OnApply_Implementation(UAnimSequence* AnimationSequence)
{
	Super::OnApply_Implementation(AnimationSequence);

	if (UAnimationBlueprintLibrary::DoesCurveExist(AnimationSequence, CurveName, ERawCurveTrackTypes::RCT_Float))
	{
		UAnimationBlueprintLibrary::RemoveCurve(AnimationSequence, CurveName, false);
	}
	UAnimationBlueprintLibrary::AddCurve(AnimationSequence, CurveName, ERawCurveTrackTypes::RCT_Float, false);

	int32 LastIndex;
	UAnimationBlueprintLibrary::GetNumFrames(AnimationSequence, LastIndex);
	// const float Value = Degrees / (LastIndex + 1);

	//
	// FTransform FirstPose;
	// UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimationSequence, RootBoneName, 0, false, FirstPose);
	//
	// FTransform MiddlePose;
	// UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimationSequence, RootBoneName, LastIndex / 2 - 1, false, MiddlePose);
	//
	// const bool bRight = MiddlePose.Rotator().Yaw - FirstPose.Rotator().Yaw <= 0.0f;

	for (int i = 0; i < LastIndex; i++)
	{
		float RateScale;
		UAnimationBlueprintLibrary::GetRateScale(AnimationSequence, RateScale);

		const int32 Frame = i + (RateScale < 0.0f ? -1 : 1);

		FTransform NextPose;
		UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimationSequence, RootBoneName, Frame, false, NextPose);

		FTransform CurPose;
		UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimationSequence, RootBoneName, i, false, CurPose);
		const float NextZ = NextPose.Rotator().Yaw, CurZ = CurPose.Rotator().Yaw;
		float Value = FMath::Abs(RateScale) * (NextZ - CurZ);
		float Time;
		UAnimationBlueprintLibrary::GetTimeAtFrame(AnimationSequence, i, Time);

		UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, CurveName, Time, Value);
	}
}
