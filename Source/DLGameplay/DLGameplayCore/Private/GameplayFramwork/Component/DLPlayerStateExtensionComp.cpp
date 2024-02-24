
#include "GameplayFramwork/Component/DLPlayerStateExtensionComp.h"

void UDLPlayerStateExtensionComp::OnPlayerBaseInfoSet_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate Delegate)
{
	if (bPlayerBaseInfoReady)
	{
		Delegate.ExecuteIfBound(GetPlayerStateBase());
	}
	else
	{
		OnPlayerBaseInfoSet.Add(Delegate);
	}
}

void UDLPlayerStateExtensionComp::OnPlayerBaseInfoSet_Unregister(const UObject* BindObject)
{
	OnPlayerBaseInfoSet.RemoveAll(BindObject);
}

void UDLPlayerStateExtensionComp::OnPlayerInitDataReady_CallAndRegister(FOnPlayerInitDataReady::FDelegate Delegate)
{
	if (CheckPlayerInitDataReady())
	{
		Delegate.ExecuteIfBound(GetPlayerStateBase(), InitData);
	}
	else
	{
		OnPlayerInitDataReady.Add(Delegate);
	}
}

void UDLPlayerStateExtensionComp::OnPlayerInitDataReady_Unregister(const UObject* BindObject)
{
	OnPlayerInitDataReady.RemoveAll(BindObject);
}

void UDLPlayerStateExtensionComp::OnPlayerPawnInitReady_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate Delegate)
{
	if (bPlayerPawnInitReady)
	{
		Delegate.ExecuteIfBound(GetPlayerStateBase());
	}
	else
	{
		OnPlayerPawnInitReady.Add(Delegate);
	}
}

void UDLPlayerStateExtensionComp::OnPlayerPawnInitReady_Unregister(const UObject* BindObject)
{
	OnPlayerPawnInitReady.RemoveAll(BindObject);
}

void UDLPlayerStateExtensionComp::OnPlayerAbilitySystemInit_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate Delegate)
{
	if (bPlayerAbilitySystemInit)
	{
		Delegate.ExecuteIfBound(GetPlayerStateBase());
	}
	else
	{
		OnAbilitySystemInit.Add(Delegate);
	}
}

void UDLPlayerStateExtensionComp::OnPlayerAbilitySystemInit_Unregister(const UObject* BindObject)
{
	OnAbilitySystemInit.RemoveAll(BindObject);
}

void UDLPlayerStateExtensionComp::OnPlayerPawnRemove_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate Delegate)
{
	if (bPlayerAbilitySystemInit && bPlayerPawnRemove)
	{
		Delegate.ExecuteIfBound(GetPlayerStateBase());
	}
	else
	{
		OnPlayerPawnRemove.Add(Delegate);
	}
}

void UDLPlayerStateExtensionComp::OnPlayerPawnRemove_Unregister(const UObject* BindObject)
{
	OnPlayerPawnRemove.RemoveAll(BindObject);
}

bool UDLPlayerStateExtensionComp::CheckPlayerInitDataReady() const
{
	return bLoadPlayerAssetComplate && bPlayerBaseInfoReady;
}

void UDLPlayerStateExtensionComp::TryBroadcastPlayerInitDataReady() const
{
	if (CheckPlayerInitDataReady())
	{
		OnPlayerInitDataReady.Broadcast(GetPlayerStateBase(), InitData);
	}
}

void UDLPlayerStateExtensionComp::LoadPlayerAssetComplate(UDLCharacterAsset* CharacterAsset)
{
	bLoadPlayerAssetComplate = true;
	InitData.CharacterAsset = CharacterAsset;

	this->TryBroadcastPlayerInitDataReady();
}

void UDLPlayerStateExtensionComp::PlayerBaseInfoReady(const FCharacterInfoBase& InfoBase)
{
	bPlayerBaseInfoReady = true;

	InitData.InfoBase = InfoBase;

	OnPlayerBaseInfoSet.Broadcast(GetPlayerStateBase());

	this->TryBroadcastPlayerInitDataReady();
}

void UDLPlayerStateExtensionComp::PlayerPawnInitReady()
{
	bPlayerPawnInitReady = true;

	OnPlayerPawnInitReady.Broadcast(GetPlayerStateBase());
}

void UDLPlayerStateExtensionComp::PlayerAbilitySystemInit()
{
	bPlayerAbilitySystemInit = true;
	bPlayerPawnRemove = false;
	OnAbilitySystemInit.Broadcast(GetPlayerStateBase());
}

void UDLPlayerStateExtensionComp::PlayerPawnRemove()
{
	bPlayerPawnRemove = true;
	OnPlayerPawnRemove.Broadcast(GetPlayerStateBase());
}

ADLPlayerStateBase* UDLPlayerStateExtensionComp::GetPlayerStateBase() const
{
	return Cast<ADLPlayerStateBase>(GetOwner());
}
