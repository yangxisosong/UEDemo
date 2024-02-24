// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLTitanApeAnimDef.h"
#include "Animation/DLCharacterAnimationAsset.h"
#include "DLTitanApeCharacterAnimationAsset.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class DLGAMEPLAYCHARACTER_API UDLTitanApeCharacterAnimationAsset : public UDLCharacterAnimationAssetBase
{
	GENERATED_BODY()
public:
	virtual const FAnimConfigBase& GetAnimConfig() override
	{
		return AnimConfig;
	}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FTitanApeAnimConfig AnimConfig;
};