#include "PathControllerAction.h"

#include "DLPathControllerEditorModule.h"
#include "PathControllerEditor.h"
#include "PathControllerTraceAsset.h"


FPathControllerAssetTypeActions::FPathControllerAssetTypeActions(uint32 InAssetCategoryBit)
{
	AssetCategoryBit = InAssetCategoryBit;
}

uint32 FPathControllerAssetTypeActions::GetCategories()
{
	return AssetCategoryBit;
}

FText FPathControllerAssetTypeActions::GetName() const
{
	return FText::FromString(TEXT("PathControll"));
}

FColor FPathControllerAssetTypeActions::GetTypeColor() const
{
	return FColor::Blue;
}

void FPathControllerAssetTypeActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);
}

void FPathControllerAssetTypeActions::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
{
	FAssetTypeActions_Base::GetActions(InObjects, Section);
}

TSharedPtr<SWidget> FPathControllerAssetTypeActions::GetThumbnailOverlay(const FAssetData& AssetData) const
{
	return FAssetTypeActions_Base::GetThumbnailOverlay(AssetData);
}

void FPathControllerAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                                      TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	for (UObject* Obj : InObjects)
	{
		const auto TargetSelector = Cast<UPathControllerTraceAsset>(Obj);
		if (TargetSelector)
		{
			bool bIsFind = false;


			const FPathControllerEditor* Editor = static_cast<FPathControllerEditor*>(GEditor->GetEditorSubsystem<
				UAssetEditorSubsystem>()->FindEditorForAsset(TargetSelector, false));
			if (Editor)
			{
				bIsFind = true;
			}


			if (!bIsFind)
			{
				IPathControllerModule::Get().CreatePathControllerEditor(EditWithinLevelEditor, TargetSelector);
			}
		}
	}
}

UClass* FPathControllerAssetTypeActions::GetSupportedClass() const
{
	return UPathControllerTraceAsset::StaticClass();
}
