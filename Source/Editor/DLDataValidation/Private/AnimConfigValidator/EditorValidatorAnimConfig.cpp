#include "EditorValidatorAnimConfig.h"

#include "DLGameplayCharacter/Public/Animation/AnimDef/DLAnimStruct.h"
#include "Engine/DataTable.h"

UEditorValidatorAnimConfig::UEditorValidatorAnimConfig()
{
}

bool UEditorValidatorAnimConfig::CanValidateAsset_Implementation(UObject* InAsset) const
{
	return  InAsset->GetName().Equals("DL_AnimConfig");
}

EDataValidationResult UEditorValidatorAnimConfig::ValidateLoadedAsset_Implementation(UObject* InAsset,
	TArray<FText>& ValidationErrors)
{
	const UDataTable* AnimConfig = Cast<UDataTable>(InAsset);
	TArray<FAnimConfig*> AnimConfigs;
	AnimConfig->GetAllRows<FAnimConfig>(TEXT(""), AnimConfigs);
	TArray<FName> RowNames = AnimConfig->GetRowNames();

	bool HasError = false;
	for (int i = 0; i < AnimConfigs.Num(); i++)
	{
		const FAnimConfig* Config = AnimConfigs[i];
		const FName& RowName = RowNames[i];
		ValidationErrors.Add(FText::FromString(FString::Printf(TEXT("%s has invalid config"), *RowName.ToString())));
		if (!Config->IsValidConfig(ValidationErrors))
		{
			HasError = true;
		}
	}
	if (HasError)
	{
		AssetFails(InAsset, FText::FromString(TEXT("AnimConfig has invalid row")), ValidationErrors);
		return EDataValidationResult::Invalid;
	}
	ValidationErrors.Reserve(1);
	ValidationErrors.Add(FText::FromString(TEXT("Validate AnimConfig Successful!")));
	AssetPasses(InAsset);
	return EDataValidationResult::Valid;
}



