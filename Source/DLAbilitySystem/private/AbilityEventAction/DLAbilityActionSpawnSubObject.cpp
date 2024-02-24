
#include "AbilityEventAction/DLAbilityActionSpawnSubObject.h"

#include "DLGameplayAbilityBase.h"

TArray<FName> UDLAbilityActionSpawnSubObject::GetSubObjectDefNameOptions() const
{
	TArray<FName> Ret;
	if (const UDLGameplayAbilityBase* Ability = GetOuterAbility())
	{
		for (const auto& Info : Ability->GetSubObjectDefines())
		{
			Ret.Add(*Info.DefineId.ToString());
		}
	}
	return Ret;
}

void UDLAbilityActionSpawnSubObject::OnExec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event)
{

	if (!Context.Ability->K2_HasAuthority())
	{
		return;
	}

	if (!ensureAlwaysMsgf(SpawnCoordinateSelector, TEXT("选择器应该不是空的")))
	{
		return;
	}

	Context.Ability->CreateSubObjectWithDefName(FGameplayTag::RequestGameplayTag(SubObjectDefId), SpawnCoordinateSelector, SpawnCount);

	return;
}
