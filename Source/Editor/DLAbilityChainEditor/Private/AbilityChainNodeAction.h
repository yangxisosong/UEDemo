#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"


class FAbilityChainNodeAssetTypeActions
	: public FAssetTypeActions_Base
{
public:
	explicit FAbilityChainNodeAssetTypeActions(uint32 InAssetCategoryBit);

	virtual uint32 GetCategories() override;
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	virtual void GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section) override;
	virtual TSharedPtr<SWidget> GetThumbnailOverlay(const FAssetData& AssetData) const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects,
	                             TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
	virtual UClass* GetSupportedClass() const override;

private:
	uint32 AssetCategoryBit = 0;
};
