// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "UObject/Object.h"
#include "ListenLockTarget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLockTarget, class UDLLockPointComponent*, LockPoint);


UCLASS()
class DLABILITYSYSTEM_API UListenLockTarget
		: public UAbilityTask
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UListenLockTarget* ListenLockTarget(
			UGameplayAbility* OwningAbility,
			FName TaskInstanceName
		);


	UPROPERTY(BlueprintAssignable)
	FOnLockTarget OnLockTarget;

protected:

	virtual void TickTask(float DeltaTime) override;
};
