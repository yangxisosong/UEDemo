#pragma once


#include "CoreMinimal.h"
#include "AIController.h"
#include "DLGameplayAbilityBase.h"
#include "GameplayTask.h"
#include "Tasks/GameplayTask_SpawnActor.h"

#include "GameplayTask_AbilityGroup_v2.generated.h"


UCLASS(Blueprintable)
class UGameplayTask_AbilityGroup_v2
	: public UGameplayTask
{
	GENERATED_BODY()

public:

	UPROPERTY()
	AAIController* Controller;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<TSubclassOf<UDLGameplayAbilityBase>> AbilityCombo;
	
	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate OnFailed;

	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate OnComplete;
public:
	UFUNCTION(BlueprintCallable, Category = "GameplayTask",
		meta = (AdvancedDisplay = "TaskOwner", BlueprintInternalUseOnly = "TRUE"))
	static UGameplayTask_AbilityGroup_v2* TaskServerActiveAbilityGroup_V2(
		const TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
		AAIController* AIController,
		const TSubclassOf<UGameplayTask_AbilityGroup_v2> AbilityGroupClass,
		TArray<TSubclassOf<UDLGameplayAbilityBase>> InAbilityCombo)
	{
		if (TaskOwner.GetInterface())
		{
			UGameplayTask_AbilityGroup_v2* AbilityGroup = NewObject<UGameplayTask_AbilityGroup_v2>(
				(UObject*)GetTransientPackage(), AbilityGroupClass);
			if (AbilityGroup)
			{
				AbilityGroup->Controller = AIController;
				AbilityGroup->AbilityCombo = InAbilityCombo;
				AbilityGroup->InitTask(*TaskOwner, TaskOwner->GetGameplayTaskDefaultPriority());
				return AbilityGroup;
			}
		}

		return nullptr;
	}


	virtual void Activate() override
	{
		Activate_BP(TaskOwner.ToScriptInterface(),Controller,Controller->GetPawn());
	}

	UFUNCTION(BlueprintImplementableEvent)
	void Activate_BP(const TScriptInterface<IGameplayTaskOwnerInterface>& InTaskOwner,AAIController* AIController,APawn* ControlledPawn);


	UFUNCTION(BlueprintCallable)
	void FinishTask(bool Success = true)
	{
		if (Success)
		{
			OnComplete.Broadcast();
		}
		else
		{
			OnFailed.Broadcast();
		}
		EndTask();
	};
	
};
