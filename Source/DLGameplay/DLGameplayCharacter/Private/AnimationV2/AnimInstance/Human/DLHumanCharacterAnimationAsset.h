// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLHumanAnimDef.h"
#include "Animation/DLCharacterAnimationAsset.h"
#include "DLHumanCharacterAnimationAsset.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class DLGAMEPLAYCHARACTER_API UDLHumanCharacterAnimationAsset : public UDLCharacterAnimationAssetBase
{
	GENERATED_BODY()
public:
	virtual const FAnimConfigBase& GetAnimConfig() override
	{
		return AnimConfig;
	}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FHumanAnimConfig AnimConfig;
};
