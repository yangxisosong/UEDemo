// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerareSlowlyStopTimeCurve.h"

#include "AnimationModifiers/Public/AnimationBlueprintLibrary.h"

void UGenerateSlowlyStopTimeCurve::OnApply_Implementation(UAnimSequence* AnimationSequence)
{
	Super::OnApply_Implementation(AnimationSequence);
	static const FName TimeCurveName = "TimeCurve";
	static const FName DistanceCurveName = "DistanceCurve";
	UAnimationBlueprintLibrary::RemoveCurve(AnimationSequence, TimeCurveName);
	UAnimationBlueprintLibrary::AddCurve(AnimationSequence, TimeCurveName, ERawCurveTrackTypes::RCT_Float);
	TArray<float> Times, Values, NewTimes;
	UAnimationBlueprintLibrary::GetFloatKeys(AnimationSequence, DistanceCurveName, Times, Values);

	for (const float Value : Values)
	{
		const float Tmp = 0.0f - Value;
		NewTimes.Emplace(Tmp);
	}
	UAnimationBlueprintLibrary::AddFloatCurveKeys(AnimationSequence, TimeCurveName, NewTimes, Times);
}
