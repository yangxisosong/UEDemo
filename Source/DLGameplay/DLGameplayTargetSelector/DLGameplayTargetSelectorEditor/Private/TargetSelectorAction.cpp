#include "TargetSelectorAction.h"

#include "DLAbilityTargetSelector.h"
#include "DLTargetSelectorEditorModule.h"
#include "TargetSelectorEditor.h"


FTargetSelectorAssetTypeActions::FTargetSelectorAssetTypeActions(uint32 InAssetCategoryBit)
{
	AssetCategoryBit = InAssetCategoryBit;
}

uint32 FTargetSelectorAssetTypeActions::GetCategories()
{
	return AssetCategoryBit;
}

FText FTargetSelectorAssetTypeActions::GetName() const
{
	return FText::FromString(TEXT("TargetSelector"));
}

FColor FTargetSelectorAssetTypeActions::GetTypeColor() const
{
	return FColor(118, 119, 118);
}

void FTargetSelectorAssetTypeActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);
}

void FTargetSelectorAssetTypeActions::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
{
	FAssetTypeActions_Base::GetActions(InObjects, Section);
}

TSharedPtr<SWidget> FTargetSelectorAssetTypeActions::GetThumbnailOverlay(const FAssetData& AssetData) const
{
	return FAssetTypeActions_Base::GetThumbnailOverlay(AssetData);
}

void FTargetSelectorAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                                      TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	for (UObject* Obj : InObjects)
	{
		const auto TargetSelector = Cast<UDLGameplayTargetSelectorBase>(Obj);
		if (TargetSelector)
		{
			bool bIsFind = false;


			FTargetSelectorEditor* Editor = static_cast<FTargetSelectorEditor*>(GEditor->GetEditorSubsystem<
				UAssetEditorSubsystem>()->FindEditorForAsset(TargetSelector, false));
			if (Editor)
			{
				bIsFind = true;
			}


			if (!bIsFind)
			{
				ITargetSelectorModule::Get().CreateTargetSelectorEditor(EditWithinLevelEditor, TargetSelector);
			}
		}
	}
}

UClass* FTargetSelectorAssetTypeActions::GetSupportedClass() const
{
	return UDLGameplayTargetSelectorBase::StaticClass();
}
