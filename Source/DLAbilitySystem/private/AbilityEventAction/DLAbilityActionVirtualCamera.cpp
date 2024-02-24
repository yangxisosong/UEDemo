#include "AbilityEventAction/DLAbilityActionVirtualCamera.h"

#include "DLGameplayAbilityBase.h"
#include "Interface/IDLVirtualCameraController.h"


TArray<FName> UDLAbilityActionActivateVirtualCamera::GetVirtualCameraDefIdOptions() const
{
	TArray<FName> Ret;
	if (const UDLGameplayAbilityBase* Ability = GetOuterAbility())
	{
		for (const auto& Info : Ability->GetVirtualCameraDefines())
		{
			Ret.Add(*Info.DefineId.ToString());
		}
	}
	return Ret;
}

void UDLAbilityActionActivateVirtualCamera::OnExec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event)
{
	Context.Ability->TriggerEventActiveVC(FGameplayTag::RequestGameplayTag(this->VirtualCameraDefId), ActivateTimeOutAction);
}

TArray<FName> UDLAbilityActionStopVirtualCamera::GetVirtualCameraDefIdOptions() const
{
	TArray<FName> Ret;
	if (const UDLGameplayAbilityBase* Ability = GetOuterAbility())
	{
		for (const auto& Info : Ability->GetVirtualCameraDefines())
		{
			Ret.Add(*Info.DefineId.ToString());
		}
	}
	return Ret;
}

void UDLAbilityActionStopVirtualCamera::OnExec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event)
{
	Context.Ability->TriggerEventStopVC(FGameplayTag::RequestGameplayTag(this->VirtualCameraDefId));
}
