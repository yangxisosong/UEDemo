#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameAbilitySysDef.h"
#include "UObject/Interface.h"
#include "IDLAbilityEventTrigger.generated.h"


UINTERFACE(Meta = (CannotImplementInterfaceInBlueprint), MinimalAPI)
class UDLAbilityEventTrigger
	: public UInterface
{
	GENERATED_BODY()
};



class IDLAbilityEventTrigger
	: public IInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerEventHitUnit(const FGameplayTag& HitEventTag, const FGameplayAbilityTargetDataHandle& TargetDataHandle) {};

	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerEventSimple(FGameplayTag EventTag, bool NeedClientPrediction = false) {};

	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerEventAbilityStageChange(EDLAbilityStage NewStage, bool NeedClientPrediction = false) {};

	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerEventActiveVC(FGameplayTag VCDefId, EVCTimeOutAction VCTimeOutAction) {};

	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerEventStopVC(FGameplayTag VCDefId) {};

	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerEventExecInputCmd() {};

	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerCustomStateEvent(bool IsBeginState, const FGameplayTag& Tag, bool NeedClientPrediction = false) {};

	UFUNCTION(BlueprintCallable, Category= GATriggerEvent)
		virtual void AddWittingEndNtfState(UObject* AnimNtfState) {};

	UFUNCTION(BlueprintCallable, Category=GATriggerEvent)
		virtual void RemoveWittingEndNtfState(UObject* AnimNtfState) {};

	UFUNCTION(BlueprintCallable, Category=GATriggerEvent)
		virtual bool CanTriggerEventByAnimNtf(UAnimationAsset* Anim, UAnimNotifyState* EndAnimNtfStateObj = nullptr) { return false; };
};
