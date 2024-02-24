// Fill out your copyright notice in the Description page of Project Settings.


#include "DLHumanAnimDef.h"

bool FAnimLandConfig::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;
	if (!LandRollMontage)
	{
		Errors.Add(FText::FromString(TEXT("MovementStateSettings in AnimLandConfig is invalid!")));
		Result = false;
	}

	return Result;
}

bool FDLTurnInPlaceAsset::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;
	if (!Animation)
	{
		Errors.Add(FText::FromString(TEXT("Animation in TurnInPlaceAsset is invalid!")));
		Result = false;
	}

	return Result;
}

bool FDLAnimTurnInPlaceConfig::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;
	if (!N_TurnIP_L90.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("N_TurnIP_L90 in TurnInPlaceConfig is invalid!")));
		Result = false;
	}
	if (!N_TurnIP_R90.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("N_TurnIP_R90 in TurnInPlaceConfig is invalid!")));
		Result = false;
	}
	if (!N_TurnIP_L180.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("N_TurnIP_L180 in TurnInPlaceConfig is invalid!")));
		Result = false;
	}
	if (!N_TurnIP_R180.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("N_TurnIP_R180 in TurnInPlaceConfig is invalid!")));
		Result = false;
	}

	return Result;
}

bool FHumanAnimConfig::IsValidConfig(TArray<FText>& Errors) const
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

	if (!TurnInPlaceConfig.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("TurnInPlaceConfig in AnimConfig is invalid!")));
		Result = false;
	}

	return Result;
}
