// Fill out your copyright notice in the Description page of Project Settings.


#include "Copy_Curves.h"
#include "AnimationModifiers/Public/AnimationBlueprintLibrary.h"


void UCopy_Curves::OnApply_Implementation(UAnimSequence* AnimationSequence)
{
	Super::OnApply_Implementation(AnimationSequence);
	if (!AnimToCopyFrom)
	{
		UE_LOG(LogTemp, Error, TEXT("AnimToCopyFrom is not valid!!!!!!"));
		return;
	}
	TArray<FName> TargetCurves;
	if (bCopyAllCurves)
	{
		UAnimationBlueprintLibrary::GetAnimationCurveNames(AnimToCopyFrom, CurveType, TargetCurves);
	}
	else
	{
		TargetCurves = CurvesToCopy;
	}

	for (const auto& CurveName : TargetCurves)
	{
		if (!UAnimationBlueprintLibrary::DoesCurveExist(AnimToCopyFrom, CurveName, CurveType))
		{
			continue;
		}

		if (UAnimationBlueprintLibrary::DoesCurveExist(AnimationSequence, CurveName, CurveType))
		{
			UAnimationBlueprintLibrary::RemoveCurve(AnimationSequence, CurveName, false);
		}
		UAnimationBlueprintLibrary::AddCurve(AnimationSequence, CurveName, CurveType, false);

		TArray<float> Times;
		switch (CurveType)
		{
		case ERawCurveTrackTypes::RCT_Float:
		{
			TArray<float> Values;
			UAnimationBlueprintLibrary::GetFloatKeys(AnimToCopyFrom, CurveName, Times, Values);
			UAnimationBlueprintLibrary::AddFloatCurveKeys(AnimationSequence, CurveName, Times, Values);
		}
		break;
		case ERawCurveTrackTypes::RCT_Vector:
		{
			TArray<FVector> Values;
			UAnimationBlueprintLibrary::GetVectorKeys(AnimToCopyFrom, CurveName, Times, Values);
			UAnimationBlueprintLibrary::AddVectorCurveKeys(AnimationSequence, CurveName, Times, Values);
		}
		break;
		case ERawCurveTrackTypes::RCT_Transform:
		{
			TArray<FTransform> Values;
			UAnimationBlueprintLibrary::GetTransformationKeys(AnimToCopyFrom, CurveName, Times, Values);
			UAnimationBlueprintLibrary::AddTransformationCurveKeys(AnimationSequence, CurveName, Times, Values);
		}
		break;
		default:
			UE_LOG(LogTemp, Error, TEXT("未知的枚举类型！！！！"));
			break;
		}

	}
}
