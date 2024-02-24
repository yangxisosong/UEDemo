#include "AbilityChainNodeAction.h"
#include "AbilityChainAsset.h"
#include "DLAbilityChainEditorModule.h"
#include "AbilityChainNodeEditor.h"
#include "AbilityChainNodeEditorStyle.h"


FAbilityChainNodeAssetTypeActions::FAbilityChainNodeAssetTypeActions(uint32 InAssetCategoryBit)
{
	AssetCategoryBit = InAssetCategoryBit;
}

uint32 FAbilityChainNodeAssetTypeActions::GetCategories()
{
	return AssetCategoryBit;
}

FText FAbilityChainNodeAssetTypeActions::GetName() const
{
	return FText::FromString("TreeNodeAsset");
}

FColor FAbilityChainNodeAssetTypeActions::GetTypeColor() const
{
	return FColor::Turquoise;
}

void FAbilityChainNodeAssetTypeActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);
}

void FAbilityChainNodeAssetTypeActions::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
{
	FAssetTypeActions_Base::GetActions(InObjects, Section);
}

TSharedPtr<SWidget> FAbilityChainNodeAssetTypeActions::GetThumbnailOverlay(const FAssetData& AssetData) const
{
	return FAssetTypeActions_Base::GetThumbnailOverlay(AssetData);
	const auto SlateIcon = FSlateIcon(FAbilityChainNodeEditorStyle::GetStyleSetName(), "TreeNodeEditorStyle.NodeIcon");
	const FSlateBrush* Icon = SlateIcon.GetIcon();
	auto Color = FSlateColor(FLinearColor(1, 0, 0));
	return SNew(SBorder)
		.Visibility(EVisibility::Visible)
		.Padding(FMargin(0.0f, 0.0f, 0.0f, 3.0f))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.Image(Icon)
		];
}

void FAbilityChainNodeAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                                      TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	for (UObject* Obj : InObjects)
	{
		const auto TargetSelector = Cast<UAbilityChainAsset>(Obj);
		if (TargetSelector)
		{
			bool bIsFind = false;


			const FAbilityChainNodeEditor* Editor = static_cast<FAbilityChainNodeEditor*>(GEditor->GetEditorSubsystem<
				UAssetEditorSubsystem>()->FindEditorForAsset(TargetSelector, false));
			if (Editor)
			{
				bIsFind = true;
			}


			if (!bIsFind)
			{
				IAbilityChainNodeModule::Get().CreateAbilityChainNodeEditor(EditWithinLevelEditor, TargetSelector);
			}
		}
	}
}

UClass* FAbilityChainNodeAssetTypeActions::GetSupportedClass() const
{
	return UAbilityChainAsset::StaticClass();
}
