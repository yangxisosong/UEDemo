#include "DLAbilitySystemGlobal.h"

#include "DLAbilityActorInfo.h"
#include "DLGameplayEffectContext.h"

FGameplayEffectContext* UDLAbilitySystemGlobal::AllocGameplayEffectContext() const
{
	return new FDLGameplayEffectContext();
}

FGameplayAbilityActorInfo* UDLAbilitySystemGlobal::AllocAbilityActorInfo() const
{
	return new FDLAbilityActorInfo();
}

void UDLAbilitySystemGlobal::InitGlobalTags()
{
	Super::InitGlobalTags();

	if (ActivateFailGEStackCountName != NAME_None)
	{
		ActivateFailGEStackCountTag = FGameplayTag::RequestGameplayTag(ActivateFailGEStackCountName);
	}
}
