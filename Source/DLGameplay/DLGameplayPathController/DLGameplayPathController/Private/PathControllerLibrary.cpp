// Fill out your copyright notice in the Description page of Project Settings.


#include "PathControllerLibrary.h"

float UPathControllerLibrary::CalculateAverageValue(const UCurveFloat* CurveFloat, const float BeginKey,
	const float EndKey)
{
	if (!ensureAlwaysMsgf(CurveFloat, TEXT("CurveFloat is invalid!!!!")))
	{
		return 0.0f;
	}

	if (!ensureAlwaysMsgf(BeginKey <= EndKey, TEXT("BeginKey or EndKey is invalid!!!")))
	{
		return 0.0f;
	}

	const float Diff = EndKey - BeginKey;
	if (FMath::IsNearlyZero(Diff))
	{
		return 0.0f;
	}

	constexpr int IterateCount = 5;
	const float IterateDiff = 1.0f / Diff;
	float Total = 0.0f;
	for (int i = 0; i < IterateCount; ++i)
	{
		Total += CurveFloat->GetFloatValue(FMath::Clamp(BeginKey + IterateDiff * i, BeginKey, EndKey));
	}
	return Total / float(IterateCount);
}
