// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "GenerareSlowlyStopTimeCurve.generated.h"

UCLASS()
class DLANIMATIONEDITORTOOL_API UGenerateSlowlyStopTimeCurve : public UAnimationModifier
{
	GENERATED_BODY()

public:
	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence) override;
};
