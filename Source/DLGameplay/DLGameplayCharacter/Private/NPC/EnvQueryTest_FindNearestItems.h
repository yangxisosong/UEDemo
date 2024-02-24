// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_FindNearestItems.generated.h"

/**
 * 
 */
UCLASS()
class UEnvQueryTest_FindNearestItems : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()
	
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	UPROPERTY(EditDefaultsOnly, Category=Distance)
	TSubclassOf<UEnvQueryContext> DistanceTo;
	
	UPROPERTY(EditDefaultsOnly, Category=Distance)
	int32 FilterNum;
};
