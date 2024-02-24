#include "DLGameplayAbilityLib.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"


void UDLGameplayAbilityLib::SendAbilityEventToActor(AActor* Actor, FGameplayTag EventTag, FDLAbilityEventData Payload)
{
	if (IsValid(Actor))
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor);
		if (AbilitySystemInterface != nullptr)
		{
			UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
			if (IsValid(AbilitySystemComponent))
			{
				// 保证 Event Tag 赋值
				Payload.EventTag = EventTag;

				// Send Event
				AbilitySystemComponent->HandleGameplayEvent(EventTag, &Payload);
			}
		}
	}
}

UDLGameplayAbilityBase* UDLGameplayAbilityLib::GetPrimaryAbilityInstanceFromHandle(
	UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
		if (AbilitySpec)
		{
			return Cast<UDLGameplayAbilityBase>(AbilitySpec->GetPrimaryInstance());
		}
	}

	return nullptr;
}

FGameplayAbilityTargetDataHandle UDLGameplayAbilityLib::MakeEmptyGameplayAbilityTargetData()
{
	FGameplayAbilityTargetDataHandle Handle;
	return Handle;
}

void UDLGameplayAbilityLib::GetDLAbilityAISetting(const TSubclassOf<UDLGameplayAbilityBase> AbilityClass,
	FDLAbilityAISetting& AISetting)
{
	if (!ensureMsgf(AbilityClass, TEXT("CastGAG_v2 中的技能不能为空！！！")))
	{
		return;
	}
	AISetting = AbilityClass.GetDefaultObject()->GetAISetting();
}
