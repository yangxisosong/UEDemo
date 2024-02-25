// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ConsoleVariablesEditorModule.h"
#include "Views/List/ConsoleVariablesEditorList.h"
#include "Widgets/SWidget.h"

class FConsoleVariablesEditorList;
class SConsoleVariablesEditorMainPanel;
class UConsoleVariablesAsset;

class FConsoleVariablesEditorMainPanel : public TSharedFromThis<FConsoleVariablesEditorMainPanel>
{
public:
	FConsoleVariablesEditorMainPanel();

	~FConsoleVariablesEditorMainPanel();

	TSharedRef<SWidget> GetOrCreateWidget();

	static FConsoleVariablesEditorModule& GetConsoleVariablesModule();
	static UConsoleVariablesAsset* GetEditingAsset();

	void AddConsoleObjectToCurrentPreset(
		const FString InConsoleCommand, const FString InValue,
		const bool bScrollToNewRow = false) const;

	FReply ValidateConsoleInputAndAddToCurrentPreset(const FText& CommittedText) const;

	/*
	 * Regenerate the list items and refresh the list. Call when adding or removing variables.
	 * @param bShouldCacheValues If true, the current list's current values will be cached and then restored when the list is rebuilt. Otherwise preset values will be used.
	 */
	void RebuildList(const FString InConsoleCommandToScrollTo = "", bool bShouldCacheValues = true) const;

	/**
	 * Refresh filters and sorting.
	 * Useful for when the list state has gone stale but the variable count has not changed.
	 */
	void RefreshList() const;
	
	void UpdatePresetValuesForSave(UConsoleVariablesAsset* InAsset) const;

	void RefreshMultiUserDetails() const;

	// Save / Load

	void SaveCurrentPreset();
	void SaveSpecificPreset( UConsoleVariablesAsset* InPreset) const;
	void SaveCurrentPresetAs();
	void SaveSpecificPresetAs( UConsoleVariablesAsset* InPreset) const;
	void ImportPreset(const FAssetData& InPresetAsset);
	void ImportPreset( UConsoleVariablesAsset* InPreset);

	TWeakObjectPtr<UConsoleVariablesAsset> GetReferenceAssetOnDisk() const
	{
		return ReferenceAssetOnDisk;
	}

	TWeakPtr<FConsoleVariablesEditorList> GetEditorList() const
	{
		return EditorList;
	}

	FConsoleVariablesEditorList::EConsoleVariablesEditorListMode GetEditorListMode() const
	{
		return EditorList->GetListMode();
	}

	UE::ConsoleVariables::MultiUser::Private::FManager& GetMultiUserManager()
	{
		return MultiUserManager;
	}

private:

	bool ImportPreset_Impl( UConsoleVariablesAsset* Preset,  UConsoleVariablesAsset* EditingAsset);

	TSharedPtr<SConsoleVariablesEditorMainPanel> MainPanelWidget;

	// The non-transient loaded asset from which we will copy to the transient asset for editing
	TWeakObjectPtr<UConsoleVariablesAsset> ReferenceAssetOnDisk;

	TSharedPtr<FConsoleVariablesEditorList> EditorList;

	static void OnConnectionChanged(EConcertConnectionStatus Status);
	static void OnRemoteCvarChange(const FString InName, const FString InValue);

	UE::ConsoleVariables::MultiUser::Private::FManager MultiUserManager;
	FDelegateHandle OnConnectionChangedHandle;
	FDelegateHandle OnRemoteCVarChangeHandle;
};
