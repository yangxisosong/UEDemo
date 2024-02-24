// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "Create_LayeringCurves.generated.h"

/**
 *
 */
UCLASS()
class DLANIMATIONEDITORTOOL_API UCreate_LayeringCurves : public UAnimationModifier
{
	GENERATED_BODY()
public:
	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FName> CurvesToCreate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DefaultValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool KeyEachFrame = true;
};
