// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTable/DLGameplayDataTableHelper.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "DataRegistrySubsystem.h"
#include "DataTable/DTSceneInfoRow.h"

const FDTPlayerCharacterInfoRow* FDLGameplayDataTableHelper::FindPlayerCharacterInfo(FName CharacterId, bool EnsureFind)
{
	const auto DataRegistrySubSys = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>();
	if (!ensureAlwaysMsgf(DataRegistrySubSys, TEXT("FDLGameplayDataTableHelper::FindPlayerCharacterInfo DataRegistry Is None CharacterId %s"), *CharacterId.ToString()))
	{
		return nullptr;
	}

	const auto Ret = DataRegistrySubSys->GetCachedItem<FDTPlayerCharacterInfoRow>(DRCharacter::CreatePlayerDataRegistryId(CharacterId));
	ensureAlwaysMsgf(Ret, TEXT("FDLGameplayDataTableHelper::FindPlayerCharacterInfo Not Find %s"), *CharacterId.ToString());

	return Ret;
}

const FDTNPCCharacterInfoRow* FDLGameplayDataTableHelper::FindNPCCharacterInfo(FName CharacterId, bool EnsureFind)
{
	const auto DataRegistrySubSys =GEngine->GetEngineSubsystem<UDataRegistrySubsystem>();
	if (!ensureAlwaysMsgf(DataRegistrySubSys, TEXT("FDLGameplayDataTableHelper::FindNPCCharacterInfo DataRegistry Is None CharacterId %s"), *CharacterId.ToString()))
	{
		return nullptr;
	}

	const auto Ret = DataRegistrySubSys->GetCachedItem<FDTNPCCharacterInfoRow>(DRCharacter::CreateNPCDataRegistryId(CharacterId));

	if (EnsureFind)
	{
		ensureAlwaysMsgf(Ret, TEXT("FDLGameplayDataTableHelper::FindNPCCharacterInfo Not Find %s"), *CharacterId.ToString());
	}

	return Ret;
}

const FDTSceneInfoRow* FDLGameplayDataTableHelper::FindSceneInfo(FName Id, bool EnsureFind)
{
	const auto DataRegistrySubSys =GEngine->GetEngineSubsystem<UDataRegistrySubsystem>();
	if (!ensureAlwaysMsgf(DataRegistrySubSys, TEXT("FDLGameplayDataTableHelper::FindSceneInfo DataRegistry Is None Id %s"), *Id.ToString()))
	{
		return nullptr;
	}

	const auto Ret = DataRegistrySubSys->GetCachedItem<FDTSceneInfoRow>(DRSceneInfo::CreateSceneInfoDataRegistryId(Id));

	if (EnsureFind)
	{
		ensureAlwaysMsgf(Ret, TEXT("FDLGameplayDataTableHelper::FindSceneInfo Not Find %s"), *Id.ToString());
	}

	return Ret;
}

TArray<const FDTNPCCharacterInfoRow*> FDLGameplayDataTableHelper::GetAllNPCCharacterInfo()
{
	if (!GEngine)
	{
		return {};
	}

	const auto DataRegistrySubSys =GEngine->GetEngineSubsystem<UDataRegistrySubsystem>();
	if (!DataRegistrySubSys)
	{
		return {};
	}

	const auto Reg = DataRegistrySubSys->GetRegistryForType(DRCharacter::DRTypeForNPC);
	if (Reg)
	{
		TMap<FDataRegistryId, const uint8*> CachedItems;
		const UScriptStruct* OutStruct = nullptr;
		const FDataRegistryCacheGetResult Result = Reg->GetAllCachedItems(CachedItems, OutStruct);

		if (Result.GetItemStatus() != EDataRegistryCacheGetStatus::NotFound)
		{
			TArray<const FDTNPCCharacterInfoRow*> Ret;
			for (const auto& V : CachedItems)
			{
				const FDTNPCCharacterInfoRow* Info = reinterpret_cast<const FDTNPCCharacterInfoRow*>(V.Value);
				Ret.Add(Info);
			}

			return Ret;
		}
	}

	return {};
}
