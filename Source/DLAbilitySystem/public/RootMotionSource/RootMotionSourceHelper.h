// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RootMotionDataAsset.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotion_Base.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RootMotionSourceHelper.generated.h"

/**
 * 
 */
UCLASS()
class URootMotionSourceHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
	meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_ApplyRootMotion_Base* ApplyRootMotionBP(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		UDataAsset_RootMotion* RootMotionDataAsset
		);
};
