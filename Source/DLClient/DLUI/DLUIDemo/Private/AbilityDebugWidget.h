// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASC/DLUnitAbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "AbilityDebugWidget.generated.h"

/**
 * 
 */
UCLASS()
class DLUIDEMO_API UAbilityDebugWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintImplementableEvent)
		void OnUpdateInfo(const FDLDebugAbilityInfo& Info);
};
