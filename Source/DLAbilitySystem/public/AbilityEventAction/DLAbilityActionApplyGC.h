#pragma once

#pragma once
#include "AbilitySystemComponent.h"
#include "DLAbilityAction.h"
#include "DLGameplayAbilityBase.h"
#include "DLGameplayEffectContext.h"
#include "DLAbilityActionApplyGC.generated.h"


UCLASS(DisplayName = ApplyGC)
class UDLAbilityActionApplyGC
	: public UDLAbilityAction
{
	GENERATED_BODY()

protected:

	virtual void OnExec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event) override
	{
		const auto Ability = Context.Ability;
		//
		//UAbilitySystemComponent* const AbilitySystemComponent = Ability->GetAbilitySystemComponentFromActorInfo_Checked();
		//AbilitySystemComponent->ExecuteGameplayCue(GameplayCueTag, Context);

	}


public:

	UPROPERTY(EditDefaultsOnly)
		bool bIsLocalApply = false;

	UPROPERTY(EditDefaultsOnly)
		FGameplayTag GameplayCueTag;
};