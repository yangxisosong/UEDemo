// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "Calculate_RotationAmount.generated.h"

/**
 *
 */
UCLASS()
class DLANIMATIONEDITORTOOL_API UCalculate_RotationAmount : public UAnimationModifier
{
	GENERATED_BODY()

public:
	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName CurveName = "RotationAmount";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName RootBoneName = "Master";
};
