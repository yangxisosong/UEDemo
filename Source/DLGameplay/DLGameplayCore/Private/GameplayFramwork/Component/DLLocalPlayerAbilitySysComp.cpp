#include "GameplayFramwork/Component/DLLocalPlayerAbilitySysComp.h"

#include "AbilitySystemComponent.h"
#include "DLGameplayAbilityBase.h"
#include "ASC/DLUnitAbilitySystemComponent.h"


UDLLocalPlayerAbilitySysComp::UDLLocalPlayerAbilitySysComp()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UDLLocalPlayerAbilitySysComp::AbilitySystemReady(UAbilitySystemComponent* InASC)
{
	ASC = InASC;

	if (const auto MyASC = Cast<UDLUnitAbilitySystemComponent>(ASC))
	{
		MyASC->OnActivateAbilitiesChange.AddWeakLambda(this, [this](){ OnActivateAbilitiesChange().Broadcast(); });
	}
}


void UDLLocalPlayerAbilitySysComp::BeginPlay()
{
	Super::BeginPlay();

}

bool UDLLocalPlayerAbilitySysComp::FindActiveAbilityDesc(FName AbilityId, FDLAbilityDesc& OutDesc)
{
	TArray<FDLAbilityDesc> Ret;
	this->GetAllActiveAbilityDesc(Ret);
	if (Ret.Contains(AbilityId))
	{
		OutDesc = *Ret.FindByKey(AbilityId);
		return true;
	}
	return false;
}

void UDLLocalPlayerAbilitySysComp::GetAllActiveAbilityDesc(TArray<FDLAbilityDesc>& AbilityDescArray)
{
	const auto& ActiveAbility = ASC->GetActivatableAbilities();
	for (const auto& AbilitySpec : ActiveAbility)
	{
		if (const auto& Ability = Cast<UDLGameplayAbilityBase>(AbilitySpec.Ability))
		{
			auto& Desc = AbilityDescArray.AddZeroed_GetRef();

			Desc.AbilityId = Ability->AbilityId;
			Desc.AbilityDescText = Ability->AbilityDesc;
			Desc.AbilityNameText = Ability->AbilityName;
			Desc.AbilityUISlot = Ability->AbilityTags.Filter(FGameplayTag::RequestGameplayTag("UI.SkillSlotID").GetSingleTagContainer()).First();
		}
	}
}

FSimpleMulticastDelegate& UDLLocalPlayerAbilitySysComp::OnActivateAbilitiesChange()
{
	return OnActivateAbilitiesChangeDel;
}

