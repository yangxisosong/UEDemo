// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "Copy_Curves.generated.h"

/**
 *
 */
UCLASS()
class DLANIMATIONEDITORTOOL_API UCopy_Curves : public UAnimationModifier
{
	GENERATED_BODY()
public:
	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence) override;

public:
	UPROPERTY(EditAnywhere)
		UAnimSequence* AnimToCopyFrom = nullptr;

	UPROPERTY(EditAnywhere)
		bool bCopyAllCurves = false;

	UPROPERTY(EditAnywhere)
		TArray<FName> CurvesToCopy;

	UPROPERTY(EditAnywhere)
		ERawCurveTrackTypes CurveType = ERawCurveTrackTypes::RCT_Float;

};
