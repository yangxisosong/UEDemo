// Fill out your copyright notice in the Description page of Project Settings.


#include "Create_LayeringCurves.h"

void UCreate_LayeringCurves::OnApply_Implementation(UAnimSequence* AnimationSequence)
{
	Super::OnApply_Implementation(AnimationSequence);
	for (const auto& it : CurvesToCreate)
	{
		if (UAnimationBlueprintLibrary::DoesCurveExist(AnimationSequence, it, ERawCurveTrackTypes::RCT_Float))
		{
			UAnimationBlueprintLibrary::RemoveCurve(AnimationSequence, it, false);
		}

		UAnimationBlueprintLibrary::AddCurve(AnimationSequence, it, ERawCurveTrackTypes::RCT_Float, false);
		if (KeyEachFrame)
		{
			int32 NumFrames;
			UAnimationBlueprintLibrary::GetNumFrames(AnimationSequence, NumFrames);
			for (int i = 0; i < NumFrames - 2; i++)
			{
				float Time;
				UAnimationBlueprintLibrary::GetTimeAtFrame(AnimationSequence, i, Time);
				UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, it, Time, DefaultValue);
			}
		}
		else
		{
			float Time;
			UAnimationBlueprintLibrary::GetTimeAtFrame(AnimationSequence, 0, Time);
			UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, it, Time, DefaultValue);
		}
	}
}
