// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#include "GenerateCharacter.h"
#include "DLPlayerSetupInfo.h"
#include "EditorAssetLibrary.h"
#include "IAssetTools.h"
#include "Types/DLCharacterAssetDef.h"

bool UDLGenerateCharacterBuilder::Build()
{
	if (CharacterName.IsEmpty())
	{
		return false;
	}


	const FString DesDir = TEXT("/Game/DL/Character/") / CharacterName;
	const FString TemplateDir = TEXT("/Game/DL/Character/Template/");
	const bool IsDuplicateOk = UEditorAssetLibrary::DuplicateDirectory(TemplateDir, DesDir);
	if (!IsDuplicateOk)
	{
		return false;
	}

	{
		if (!bAsNPC)
		{
			auto AssetList = UEditorAssetLibrary::ListAssets(DesDir);
			for (auto AssetPath : AssetList)
			{
				if (AssetPath.Contains(TEXT("NPC")))
				{
					UEditorAssetLibrary::DeleteAsset(AssetPath);
				}
			}
		}
	}

	{
		auto AssetList = UEditorAssetLibrary::ListAssets(DesDir);
		for (auto AssetPath : AssetList)
		{
			if (AssetPath.Contains(TEXT("TName")))
			{
				UEditorAssetLibrary::RenameAsset(AssetPath, AssetPath.Replace(TEXT("TName"), *CharacterName));
			}
		}
	}

	UEditorAssetLibrary::SaveDirectory(DesDir);

	TArray<FAssetData> AssetDataList;

	auto AssetList = UEditorAssetLibrary::ListAssets(DesDir);
	for (auto AssetPath : AssetList)
	{
		AssetDataList.Add(UEditorAssetLibrary::FindAssetData(AssetPath));
	}

	FAssetData DA_Asset_Player;
	FAssetData DA_Asset_NPC;

	FAssetData CBP_Asset_Player;
	FAssetData CBP_Asset_NPC;

	FAssetData SD_Asset_Default;
	FAssetData GE_Asset_Setup;

	auto Do_BuildAssetData = [](const FAssetData& Data, const FString& Start, const FString& End, FAssetData& Out)->bool
	{
		const auto& Name = Data.AssetName.ToString();
		if (Name.StartsWith(Start) && Name.EndsWith(End))
		{
			Out = Data;
			return true;
		}
		return false;
	};


	for (const auto& AssetData : AssetDataList)
	{
		const auto& Name = AssetData.AssetName.ToString();

		bool IsOk = false;
		IsOk = Do_BuildAssetData(AssetData, TEXT("DA_"), TEXT("_Player"), DA_Asset_Player);
		if (IsOk)
		{
			continue;
		}

		IsOk = Do_BuildAssetData(AssetData, TEXT("DA_"), TEXT("_NPC"), DA_Asset_NPC);
		if (IsOk)
		{
			continue;
		}

		IsOk = Do_BuildAssetData(AssetData, TEXT("CBP_"), TEXT("_Player"), CBP_Asset_Player);
		if (IsOk)
		{
			continue;
		}

		IsOk = Do_BuildAssetData(AssetData, TEXT("CBP_"), TEXT("_NPC"), CBP_Asset_NPC);
		if (IsOk)
		{
			continue;
		}

		IsOk = Do_BuildAssetData(AssetData, TEXT("SD_"), TEXT("_Default"), SD_Asset_Default);
		if (IsOk)
		{
			continue;
		}

		IsOk = Do_BuildAssetData(AssetData, TEXT("GE_"), TEXT("_Setup"), GE_Asset_Setup);
		if (IsOk)
		{
			continue;
		}
	}

	// 处理 DataAsset 
	auto DoSetDAData = [](const FAssetData& DA, const FAssetData& CBP)
	{
		UObject* Object = DA.GetAsset();
		if (UDLCharacterAsset* Asset = Cast<UDLCharacterAsset>(Object))
		{
			if(const UBlueprint* CBPClass = Cast<UBlueprint>(CBP.GetAsset()))
			{
				Asset->CharacterClass = CBPClass->GeneratedClass;
			}
		}
	};

	DoSetDAData(DA_Asset_Player, CBP_Asset_Player);

	if (bAsNPC)
	{
		DoSetDAData(DA_Asset_NPC, CBP_Asset_NPC);
	}

	// 处理 SD
	UObject* SDObject = SD_Asset_Default.GetAsset();
	if (SDObject)
	{
		UBlueprint* BP = Cast<UBlueprint>(SDObject);
		if (BP)
		{
			auto SDClass = BP->GeneratedClass;
			auto P = CastField<FStructProperty>(SDClass->FindPropertyByName(TEXT("SetupInfo")));
			if (P)
			{
				FDLPlayerSetupInfo* Data = P->ContainerPtrToValuePtr<FDLPlayerSetupInfo>(SDClass->GetDefaultObject());
				auto& Info = Data->StartupGameplayEffects.AddZeroed_GetRef();
				Info.EffectClass = GE_Asset_Setup.GetAsset();
			}
		}
	}

	UEditorAssetLibrary::SaveDirectory(DesDir);

	return true;
}
