// Fill out your copyright notice in the Description page of Project Settings.


#include "DLTitanApeAnimDef.h"

bool FTitanApeAnimConfig::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;
	if (!MovementStateSettings.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("MovementStateSettings in AnimConfig is invalid!")));
		Result = false;
	}

	if (!LandConfig.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("LandConfig in AnimConfig is invalid!")));
		Result = false;
	}

	if (!TurnInPlaceConfig_Stage_1.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("TurnInPlaceConfig_Stage_1 in AnimConfig is invalid!")));
		Result = false;
	}

	if (!TurnInPlaceConfig_Stage_2_3.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("TurnInPlaceConfig_Stage_2_3 in AnimConfig is invalid!")));
		Result = false;
	}

	return Result;
}
