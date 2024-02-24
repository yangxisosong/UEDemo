// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "DLAnimModiferDefine.h"
#include "Create_Curves.generated.h"

/**
 * 
 */
UCLASS()
class DLANIMATIONEDITORTOOL_API UCreate_Curves : public UAnimationModifier
{
	GENERATED_BODY()
public:
	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FAnimCurveCreationParams> CurvesToCreate;
};
