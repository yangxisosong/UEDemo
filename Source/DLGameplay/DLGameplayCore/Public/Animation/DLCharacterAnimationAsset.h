// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLAnimBaseDef.h"

#include "DLCharacterAnimationAsset.generated.h"

/**
 *
 */
UCLASS(Abstract)
class DLGAMEPLAYCORE_API UDLCharacterAnimationAssetBase : public UDataAsset
{
	GENERATED_BODY()
public:

	virtual const FAnimConfigBase& GetAnimConfig()
	{
		ensureMsgf(false, TEXT("必须实现该函数！！！"));
		static FAnimConfigBase Tmp{};
		return Tmp;
	}

};
