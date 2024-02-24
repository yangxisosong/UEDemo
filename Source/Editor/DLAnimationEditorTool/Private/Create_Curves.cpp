// Fill out your copyright notice in the Description page of Project Settings.


#include "Create_Curves.h"

#include "DLAnimModiferDefine.h"

void UCreate_Curves::OnApply_Implementation(UAnimSequence* AnimationSequence)
{
	Super::OnApply_Implementation(AnimationSequence);
	for (const auto& it : CurvesToCreate)
	{
		if (UAnimationBlueprintLibrary::DoesCurveExist(AnimationSequence, it.CurveName, ERawCurveTrackTypes::RCT_Float))
		{
			UAnimationBlueprintLibrary::RemoveCurve(AnimationSequence, it.CurveName, false);
		}
		UAnimationBlueprintLibrary::AddCurve(AnimationSequence, it.CurveName, ERawCurveTrackTypes::RCT_Float, false);

		if (it.KeyEachFrame)
		{
			int32 NumFrames;
			UAnimationBlueprintLibrary::GetNumFrames(AnimationSequence, NumFrames);
			for (int i = 0; i < NumFrames - 2; i++)
			{
				float Time;
				UAnimationBlueprintLibrary::GetTimeAtFrame(AnimationSequence, i, Time);
				UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, it.CurveName, Time, 0.0f);

			}
		}
		else
		{
			for (const auto& Frame : it.Keys)
			{
				float Time;
				UAnimationBlueprintLibrary::GetTimeAtFrame(AnimationSequence, Frame.FrameNumber, Time);
				UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, it.CurveName, Time, Frame.CurveValue);
			}
		}
	}
}
