// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "UObject/Object.h"
#include "CachePlayerInputVectorTask.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCacheInputVec, const FVector&, InputVec);

UCLASS()
class DLABILITYSYSTEM_API UCachePlayerInputVectorTask
	: public UAbilityTask
{
	GENERATED_BODY()
public:


	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UCachePlayerInputVectorTask* CachePlayerInputVector(
			UGameplayAbility* OwningAbility,
			FName TaskInstanceName,
			bool InJustCacheNonzeroVec = true
		);

	UFUNCTION(BlueprintCallable)
		FVector GetCacheValue();


	UPROPERTY(BlueprintAssignable)
		FOnCacheInputVec OnCacheInputVec;

protected:

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;


private:

	FVector CacheValue = FVector::ZeroVector;
	bool JustCacheNonzeroVec = true;
};
