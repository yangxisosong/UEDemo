#include "DLNPCSaved.h"

#include "DataRegistrySubsystem.h"

static FDataRegistryType DRTypeForNPCSetup(TEXT("NPCSetup"));

inline FDataRegistryId CreateNPCSetupInfoRegistryId(const FName CharacterId)
{
	FDataRegistryId ID;
	ID.RegistryType = DRTypeForNPCSetup;
	ID.ItemName = CharacterId;
	return ID;
}


bool UDLNPCSaved::ReadSetupInfo(const FName CharacterID, FDTNPCSetupInfo& OutSetupInfo)
{
	const auto DataRegistry = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>();

	const FDTNPCSetupInfoRow* Data = DataRegistry->GetCachedItem<FDTNPCSetupInfoRow>(
		CreateNPCSetupInfoRegistryId(CharacterID));

	if (Data)
	{
		OutSetupInfo.SetupAbility = Data->SetupAbility;
		OutSetupInfo.SetupGameplayEffect = Data->SetupGameplayEffect;

		for (const auto& Weapon : Data->SetupWeaponArray)
		{
			auto& WeaponInfo = OutSetupInfo.WeaponInfoArray.AddZeroed_GetRef();
			WeaponInfo.WeaponAssetId = Weapon.WeaponAssetId;

			for (const auto& Info : Weapon.WeaponAttachInfos)
			{
				auto& AddInfo = WeaponInfo.WeaponAttachInfos.AddZeroed_GetRef();
				AddInfo.WeaponId = Info.WeaponId;
				AddInfo.BoneSocketTag = Info.BoneSocketTag;
			}
		}

		return true;
	}

	return false;
}

