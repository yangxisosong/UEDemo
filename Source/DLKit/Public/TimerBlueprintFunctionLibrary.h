// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TimerBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DLKIT_API UTimerBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category="Utilities|FlowControl", meta=(Latent, WorldContext="WorldContextObject", LatentInfo="LatentInfo", Keywords="sleep"))
	static void	DelayOneFrame(const UObject* WorldContextObject, struct FLatentActionInfo LatentInfo );
};
