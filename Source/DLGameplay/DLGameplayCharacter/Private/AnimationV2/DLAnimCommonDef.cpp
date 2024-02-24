// Fill out your copyright notice in the Description page of Project Settings.


#include "DLAnimCommonDef.h"

#include "DLAnimationMacros.h"

bool FAnimBlendCurves::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;

	if (!StrideBlend_N_Walk)
	{
		Errors.Add(FText::FromString(TEXT("StrideBlend_N_Walk in AnimBlendCurvesConfig is invalid!")));
		Result = false;
	}

	if (!StrideBlend_N_Run)
	{
		Errors.Add(FText::FromString(TEXT("StrideBlend_N_Run in AnimBlendCurvesConfig is invalid!")));
		Result = false;
	}

	if (!LandPredictionCurve)
	{
		Errors.Add(FText::FromString(TEXT("LandPredictionCurve in AnimBlendCurvesConfig is invalid!")));
		Result = false;
	}

	if (!LeanInAirCurve)
	{
		Errors.Add(FText::FromString(TEXT("LeanInAirCurve in AnimBlendCurvesConfig is invalid!")));
		Result = false;
	}

	if (!YawOffset_FB)
	{
		Errors.Add(FText::FromString(TEXT("YawOffset_FB in AnimBlendCurvesConfig is invalid!")));
		Result = false;
	}

	if (!YawOffset_LR)
	{
		Errors.Add(FText::FromString(TEXT("YawOffset_LR in AnimBlendCurvesConfig is invalid!")));
		Result = false;
	}

	return Result;
}

bool FDLAnimMovementSetting::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;

	if (!BlendCurvesConfig.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("BlendCurvesConfig in MovementSettings is invalid!")));
		Result = false;
	}

	return Result;
}

bool FDLAnimMovementSettings::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;
	if (!Normal.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("Walk in MovementStateSettings is invalid!")));
		Result = false;
	}

	if (!Locking.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("Locking in MovementStateSettings is invalid!")));
		Result = false;
	}

	return Result;
}

const FAnimBlendCurves& FDLAnimMovementSettings::GetBlendCurveConfigByState(const EAnimCharacterMainState State) const
{
	switch (State)
	{
	case EAnimCharacterMainState::Normal:
		return Normal.BlendCurvesConfig;
	case EAnimCharacterMainState::Locking:
		return Locking.BlendCurvesConfig;
		// case ECharacterMainState::Aiming:
		// 	return MovementStateSettings.Aiming.BlendCurvesConfig;
	default:
		ensureMsgf(false, TEXT("居然还有其他的类型！！！！"));
		return Normal.BlendCurvesConfig;
	}
}
