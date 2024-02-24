// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PathControllerLibrary.generated.h"

class UPathControllerAsset;
class USplineComponent;
/**
 *
 */
UCLASS()
class DLGAMEPLAYPATHCONTROLLER_API UPathControllerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure)
		static float CalculateAverageValue(const UCurveFloat* CurveFloat, const float BeginKey, const float EndKey);
};
