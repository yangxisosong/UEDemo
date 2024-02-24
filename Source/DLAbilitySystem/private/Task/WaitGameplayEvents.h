#pragma once


#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "UObject/ObjectMacros.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WaitGameplayEvents.generated.h"

class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitGameplayEventsDelegate, FGameplayEventData, Payload);

UCLASS()
class UAbilityTask_WaitGameplayEvents : public UAbilityTask
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
		FWaitGameplayEventsDelegate	EventReceived;


	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAbilityTask_WaitGameplayEvents* WaitGameplayEvents(UGameplayAbility* OwningAbility, FGameplayTagContainer EventTags, AActor* OptionalExternalTarget = nullptr, bool OnlyTriggerOnce = false)
	{
		UAbilityTask_WaitGameplayEvents* MyObj = NewAbilityTask<UAbilityTask_WaitGameplayEvents>(OwningAbility);
		MyObj->EventTags = EventTags;
		MyObj->SetExternalTarget(OptionalExternalTarget);
		MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
		return MyObj;
	}

	void SetExternalTarget(const AActor* Actor)
	{
		if (Actor)
		{
			UseExternalTarget = true;
			OptionalExternalTarget = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
		}
	}

	UAbilitySystemComponent* GetTargetASC() const
	{
		if (UseExternalTarget)
		{
			return OptionalExternalTarget;
		}

		return AbilitySystemComponent;
	}

	virtual void Activate() override
	{
		UAbilitySystemComponent* ASC = GetTargetASC();
		if (ASC)
		{
			MyHandle = ASC->AddGameplayEventTagContainerDelegate(EventTags,
				FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
					this,
					&UAbilityTask_WaitGameplayEvents::GameplayEventContainerCallback)
			);
		}

		Super::Activate();
	}


	virtual void GameplayEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			FGameplayEventData TempPayload = *Payload;
			TempPayload.EventTag = MatchingTag;
			EventReceived.Broadcast(MoveTemp(TempPayload));
		}
		if (OnlyTriggerOnce)
		{
			EndTask();
		}
	}

	virtual void OnDestroy(bool AbilityEnding) override
	{
		UAbilitySystemComponent* ASC = GetTargetASC();
		if (ASC && MyHandle.IsValid())
		{
			ASC->RemoveGameplayEventTagContainerDelegate(EventTags, MyHandle);
		}

		Super::OnDestroy(AbilityEnding);
	}

	FGameplayTagContainer EventTags;

	UPROPERTY()
		UAbilitySystemComponent* OptionalExternalTarget;

	bool UseExternalTarget;
	bool OnlyTriggerOnce;

	FDelegateHandle MyHandle;
};

