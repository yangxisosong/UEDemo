#include "AbilityEventAction/DLAbilityEventBase.h"
#include "DLGameplayAbilityBase.h"
#include "AbilityEventAction/DLAbilityAction.h"
#include "AbilityEventAction/DLAbilityActionSelectTarget.h"
#include "AnimNotify/DLAbilityAnimNtf.h"


const UDLGameplayAbilityBase* UDLAbilityEventBase::GetOuterAbility() const
{
	UObject* Outer = GetOuter();
	while (Outer)
	{
		if (Cast<UDLGameplayAbilityBase>(Outer))
		{
			break;
		}

		Outer = Outer->GetOuter();
	}

	return Cast<UDLGameplayAbilityBase>(Outer);
}

void UDLAbilityEventBase::TriggerActionList(const FDLAbilityActionContext& Context)
{
	for (UDLAbilityAction* Action : ActionList)
	{
		Action->Exec(Context, this);
	}
}

bool UDLAbilityEventWeaponHitActor::Validation(TArray<FString>& OutErrMsg)
{
	return true;
}

FGameplayTagContainer UDLAbilityEventWeaponHitActor::GetHitEventTagsOption() const
{
	FGameplayTagContainer Ret;

	const auto Ability = GetOuterAbility();
	if (!Ability)
	{
		return Ret;
	}

	const auto& EventList = Ability->GetEventActionArray();
	for (const auto& Event : EventList)
	{
		if (Event && Event->ActionList.Num() > 0)
		{
			for (const auto& Action : Event->ActionList)
			{
				if (const auto UnitSelectAction = Cast<UDLAbilityActionSelectUnits>(Action))
				{
					Ret.AddTag(UnitSelectAction->OnHitEventTag);
				}
			}
		}
	}

	const auto& Animation = Ability->GetAnimationInfo();

	if (const UAnimMontage* Montage = Animation.AnimMontage.LoadSynchronous())
	{
		for (const auto& Ntf : Montage->Notifies)
		{
			if (const UDLAnimNS_WeaponAttack* WeaponNtf = Cast<UDLAnimNS_WeaponAttack>(Ntf.NotifyStateClass))
			{
				Ret.AddTag(WeaponNtf->WeaponAttackInfo.EventTag);
			}
		}
	}

	return Ret;
}


FGameplayAbilityTargetDataHandle UDLAbilityEventWeaponHitActor::GetTargetData() const
{
	return TargetDataHandle;
}

const TArray<FHitResult>& UDLAbilityEventWeaponHitActor::GetHitResults() const
{
	static TArray<FHitResult> Ret;
	Ret.Empty();

	for (const auto& Data : TargetDataHandle.Data)
	{
		if (Data->HasHitResult())
		{
			Ret.Add(*Data->GetHitResult());
		}
	}
	return Ret;
}

bool UDLAbilitySimpleEvent::Validation(TArray<FString>& OutErrMsg)
{
	bool Ret = true;

	const auto Ability = GetOuterAbility();
	if (!Ability)
	{
		return Ret;
	}


	if (!EventTags.IsValid())
	{
		Ret = false;
		OutErrMsg.Add(FString::Printf(TEXT("CustomEvent Is None")));
	}


	const auto& Animation = Ability->GetAnimationInfo();

	if (const UAnimMontage* Montage = Animation.AnimMontage.LoadSynchronous())
	{
		FGameplayTagContainer Tags;

		for (const auto& Ntf : Montage->Notifies)
		{
			if (Ntf.Notify && Ntf.Notify->GetClass()->GetName() == TEXT("AN_CustomEvent_C"))
			{
				const auto Class = Ntf.Notify->GetClass();
				if (Class)
				{
					const auto P = Class->FindPropertyByName(TEXT("CustomEventTag"));
					if (P)
					{
						const FGameplayTag* Tag = P->ContainerPtrToValuePtr<FGameplayTag>(Ntf.Notify);
						if (Tag)
						{
							Tags.AddTag(*Tag);
						}
					}
				}
			}
		}

		if(!Tags.HasAll(EventTags))
		{
			Ret = false;
			OutErrMsg.Add((FString::Printf(TEXT("CustomEvent %s Not Match Montage CustomEvent->CustomEventTag %s"), *EventTags.ToString(), *Tags.ToString())));
		}
	}

	return Ret;
}

void UDLAbilitySimpleEvent::TriggerAction(const FDLAbilityActionContext& Context)
{
	for (UDLAbilityAction* Action : ActionList)
	{
		Action->Exec(Context, this);
	}
}

void UDLAbilityStageChangeEvent::TriggerAction(const FDLAbilityActionContext& Context)
{
	for (UDLAbilityAction* Action : ActionList)
	{
		Action->Exec(Context, this);
	}
}
