// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AnimationV2/DLAnimCommonDef.h"
#include "DLHumanAnimInsHelper.generated.h"

class UDLHumanAnimInstance;

/**
 *
 */
UCLASS()
class DLGAMEPLAYCHARACTER_API UDLHumanAnimInsHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "AnimInsHelper | Human")
		static void UpdateLayerValues(UDLHumanAnimInstance* AnimIns);

	UFUNCTION(BlueprintCallable, Category = "AnimInsHelper | Human")
		static void UpdateMovementValues(UDLHumanAnimInstance* AnimIns, const float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "AnimInsHelper | Human")
		static void UpdateRotationValues(UDLHumanAnimInstance* AnimIns);

	UFUNCTION(BlueprintCallable, Category = "AnimInsHelper | Human")
		static void UpdateInAirValues(UDLHumanAnimInstance* AnimIns, const float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category = "AnimInsHelper | Human")
		static void CalculateVelocityBlend(UDLHumanAnimInstance* AnimIns, FAnimVelocityBlend& VelocityBlend);

	UFUNCTION(BlueprintPure, Category = "AnimInsHelper | Human")
		static FVector CalculateRelativeAccelerationAmount(UDLHumanAnimInstance* AnimIns);

	UFUNCTION(BlueprintPure, Category = "AnimInsHelper | Human")
		static float CalculateSpeedBlend(UDLHumanAnimInstance* AnimIns,FVector2D& TargetWalkRushBlend);

	UFUNCTION(BlueprintPure, Category = "AnimInsHelper | Human")
		static float CalculateStrideBlend(UDLHumanAnimInstance* AnimIns);

	UFUNCTION(BlueprintPure, Category = "AnimInsHelper | Human")
		static float CalculateStandingPlayRate(UDLHumanAnimInstance* AnimIns);

	UFUNCTION(BlueprintPure, Category = "AnimInsHelper | Human")
		static const float CalculateLandPrediction(UDLHumanAnimInstance* AnimIns);

	UFUNCTION(BlueprintPure, Category = "AnimInsHelper | Human")
		static const FBodyLeanAmount CalculateAirLeanAmount(UDLHumanAnimInstance* AnimIns);
};
