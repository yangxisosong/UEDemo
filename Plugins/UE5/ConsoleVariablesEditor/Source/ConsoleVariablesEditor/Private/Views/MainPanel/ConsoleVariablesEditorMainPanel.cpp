// Copyright Epic Games, Inc. All Rights Reserved.

#include "Views/MainPanel/ConsoleVariablesEditorMainPanel.h"

#include "ConcertMessages.h"
#include "ConsoleVariablesAsset.h"
#include "ConsoleVariablesEditorCommandInfo.h"
#include "ConsoleVariablesEditorLog.h"
#include "ConsoleVariablesEditorModule.h"
#include "Views/List/ConsoleVariablesEditorList.h"
#include "Views/MainPanel/SConsoleVariablesEditorMainPanel.h"

#include "FileHelpers.h"
#include "Framework/Application/SlateApplication.h"
#include "MultiUser/ConsoleVariableSyncData.h"


FConsoleVariablesEditorMainPanel::FConsoleVariablesEditorMainPanel()
{
	EditorList = MakeShared<FConsoleVariablesEditorList>();
	
	OnConnectionChangedHandle = MultiUserManager.OnConnectionChange().AddStatic(
		&FConsoleVariablesEditorMainPanel::OnConnectionChanged);
	OnRemoteCVarChangeHandle = MultiUserManager.OnRemoteCVarChange().AddStatic(
		&FConsoleVariablesEditorMainPanel::OnRemoteCvarChange);
}

FConsoleVariablesEditorMainPanel::~FConsoleVariablesEditorMainPanel()
{
	MainPanelWidget.Reset();
	EditorList.Reset();
	MultiUserManager.OnConnectionChange().Remove(OnConnectionChangedHandle);
	MultiUserManager.OnRemoteCVarChange().Remove(OnRemoteCVarChangeHandle);
}

TSharedRef<SWidget> FConsoleVariablesEditorMainPanel::GetOrCreateWidget()
{
	if (!MainPanelWidget.IsValid())
	{
		SAssignNew(MainPanelWidget, SConsoleVariablesEditorMainPanel, SharedThis(this));
	}

	return MainPanelWidget.ToSharedRef();
}

FConsoleVariablesEditorModule& FConsoleVariablesEditorMainPanel::GetConsoleVariablesModule()
{
	return FConsoleVariablesEditorModule::Get();
}

UConsoleVariablesAsset* FConsoleVariablesEditorMainPanel::GetEditingAsset()
{
	return GetConsoleVariablesModule().GetPresetAsset();
}

void FConsoleVariablesEditorMainPanel::AddConsoleObjectToCurrentPreset(
	const FString InConsoleCommand, const FString InValue, const bool bScrollToNewRow) const
{
	if ( UConsoleVariablesAsset* Asset = GetEditingAsset())
	{
		Asset->AddOrSetConsoleObjectSavedData(
			{
				InConsoleCommand.TrimStartAndEnd(),
				InValue,
				ECheckBoxState::Checked
			}
		);

		if (GetEditorList().Pin()->GetListMode() == FConsoleVariablesEditorList::EConsoleVariablesEditorListMode::Preset)
		{
			RebuildList(bScrollToNewRow ? InConsoleCommand : "");
		}
	}
}

FReply FConsoleVariablesEditorMainPanel::ValidateConsoleInputAndAddToCurrentPreset(const FText& CommittedText) const
{
	// Clear Search
	if (const TSharedPtr<FConsoleVariablesEditorList> PinnedList = GetEditorList().Pin())
	{
		PinnedList->SetSearchString("");
	}
	
	const FString CommandString = CommittedText.ToString().TrimStartAndEnd();

	if (CommandString.IsEmpty())
	{
		UE_LOG(LogConsoleVariablesEditor, Warning, TEXT("%hs: Input is blank."), __FUNCTION__);

		return FReply::Unhandled();
	}
	
	FString CommandKey; // The actual variable name, like r.ScreenPercentage
	FString ValueString; // The variable value, usually after the CommandKey and a single space
	FString AdditionalParams; // Some variables have multiple parameters between the CommandKey and ValueString

	if (CommandString.Contains(" "))
	{
		// We want to treat the last string after the last space as the value
		// This allows commands with multiple parameters to be treated as separate list items
		const int32 IndexOfLastSpace = CommandString.Find(" ", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		CommandKey = CommandString.Left(IndexOfLastSpace).TrimStartAndEnd();
		ValueString = CommandString.RightChop(IndexOfLastSpace).TrimStartAndEnd();

		// If true, this variable has additional parameters
		if (CommandKey.Contains(" "))
		{
			const int32 IndexOfFirstSpace = CommandString.Find(" ", ESearchCase::IgnoreCase, ESearchDir::FromStart);
			AdditionalParams = CommandKey.RightChop(IndexOfFirstSpace).TrimStartAndEnd();
			CommandKey = CommandKey.Left(IndexOfFirstSpace).TrimStartAndEnd();
		}
	}
	else
	{
		CommandKey = CommandString;
	}

	if (IConsoleObject* ConsoleObject = IConsoleManager::Get().FindConsoleObject(*CommandKey)) 
	{
		if (ValueString.IsEmpty())
		{
			if (const IConsoleVariable* AsVariable = ConsoleObject->AsVariable())
			{
				ValueString = AsVariable->GetString();
			}
		}
		
		AddConsoleObjectToCurrentPreset(
			CommandKey + " " + AdditionalParams,
			ValueString,
			true
		);
	}
	// Try to execute the whole command. Some commands are not registered, but are parsed externally so they won't be found by IConsoleManager::FindConsoleObject
	else if (GEngine->Exec(FConsoleVariablesEditorCommandInfo::GetCurrentWorld(), *CommandString))
	{
		AddConsoleObjectToCurrentPreset(
			CommandString,
			"",
			true
			);
	}
	else
	{
		UE_LOG(LogConsoleVariablesEditor, Warning,
			TEXT("%hs: Input '%s' is not a recognized console variable or command."),
			__FUNCTION__, *CommandString);

		return FReply::Unhandled();
	}

	return FReply::Handled();
}

void FConsoleVariablesEditorMainPanel::RebuildList(const FString InConsoleCommandToScrollTo, bool bShouldCacheValues) const
{
	if (EditorList.IsValid())
	{
		EditorList->RebuildList(InConsoleCommandToScrollTo);
	}
}

void FConsoleVariablesEditorMainPanel::RefreshList() const
{
	if (EditorList.IsValid())
	{
		EditorList->RefreshList();
	}
}

void FConsoleVariablesEditorMainPanel::UpdatePresetValuesForSave(UConsoleVariablesAsset* InAsset) const
{
	if (EditorList.IsValid())
	{
		EditorList->UpdatePresetValuesForSave(InAsset);
	}
}

void FConsoleVariablesEditorMainPanel::RefreshMultiUserDetails() const
{
	if (MainPanelWidget.IsValid())
	{
		MainPanelWidget->RefreshMultiUserDetails();
	}
}

void FConsoleVariablesEditorMainPanel::SaveCurrentPreset()
{const TWeakObjectPtr<UConsoleVariablesAsset> EditingAsset = GetEditingAsset();
	if (
		EditingAsset.IsValid() && ReferenceAssetOnDisk.IsValid())
	{
		UpdatePresetValuesForSave(EditingAsset.Get());
		
		if (UPackage* ReferencePackage = ReferenceAssetOnDisk.Get()->GetPackage())
		{
			ReferenceAssetOnDisk->CopyFrom(EditingAsset.Get());

			UEditorLoadingAndSavingUtils::SavePackages({ ReferencePackage }, false);

			return;
		}
	}

	// Fallback
	SaveCurrentPresetAs();
}

void FConsoleVariablesEditorMainPanel::SaveSpecificPreset( UConsoleVariablesAsset* InPreset) const
{
	if (InPreset)
	{		
		if (UPackage* ReferencePackage = InPreset->GetPackage())
		{
			UEditorLoadingAndSavingUtils::SavePackages({ ReferencePackage }, false);

			return;
		}
	}

	// Fallback
	SaveSpecificPresetAs(InPreset);
}

void FConsoleVariablesEditorMainPanel::SaveCurrentPresetAs()
{
	const TWeakObjectPtr<UConsoleVariablesAsset> EditingAsset = GetEditingAsset(); 
	if (EditingAsset.IsValid())
	{
		UpdatePresetValuesForSave(EditingAsset.Get());
			
		TArray<UObject*> SavedAssets;
		FEditorFileUtils::SaveAssetsAs({ EditingAsset.Get() }, SavedAssets);

		if (SavedAssets.Num())
		{
			UConsoleVariablesAsset* SavedAsset = Cast<UConsoleVariablesAsset>(SavedAssets[0]);

			if (ensure(SavedAsset))
			{
				ReferenceAssetOnDisk = SavedAsset;
			}
		}
	}
}

void FConsoleVariablesEditorMainPanel::SaveSpecificPresetAs( UConsoleVariablesAsset* InPreset) const
{
	if (InPreset)
	{
		TArray<UObject*> SavedAssets;
		FEditorFileUtils::SaveAssetsAs({ InPreset }, SavedAssets);
	}
}

void FConsoleVariablesEditorMainPanel::ImportPreset(const FAssetData& InPresetAsset)
{
	FSlateApplication::Get().DismissAllMenus();
	
	if (UConsoleVariablesAsset* Preset = CastChecked<UConsoleVariablesAsset>(InPresetAsset.GetAsset()))
	{
		UConsoleVariablesAsset* EditingAsset = GetEditingAsset(); 
		if (ImportPreset_Impl(Preset, EditingAsset))
		{
			EditorList->RebuildList("", false);
		}
	}
}

void FConsoleVariablesEditorMainPanel::ImportPreset( UConsoleVariablesAsset* InPreset)
{
	FSlateApplication::Get().DismissAllMenus();
	 UConsoleVariablesAsset* EditingAsset = GetEditingAsset();
	if ( ImportPreset_Impl(InPreset, EditingAsset))
	{
		EditorList->RebuildList("", false);
	}
}

bool FConsoleVariablesEditorMainPanel::ImportPreset_Impl(
	 UConsoleVariablesAsset* Preset,  UConsoleVariablesAsset* EditingAsset)
{
	if (Preset && EditingAsset)
	{
		ReferenceAssetOnDisk = Preset;

		EditingAsset->Modify();
		EditingAsset->CopyFrom(Preset);

		return true;
	}

	return false;
}

void FConsoleVariablesEditorMainPanel::OnConnectionChanged(EConcertConnectionStatus Status)
{
	switch (Status)
	{
		case EConcertConnectionStatus::Connected:
			UE_LOG(LogConsoleVariablesEditor, VeryVerbose, TEXT("Multi-user has connected to a session."));
			break;
		case EConcertConnectionStatus::Disconnected:
			UE_LOG(LogConsoleVariablesEditor, VeryVerbose, TEXT("Multi-user has disconnected from session."));
			break;
		default:
			break;
	}
}

void FConsoleVariablesEditorMainPanel::OnRemoteCvarChange(const FString InName, const FString InValue)
{
	FConsoleVariablesEditorModule& ConsoleVariablesEditorModule = FConsoleVariablesEditorModule::Get();

	ConsoleVariablesEditorModule.OnRemoteCvarChanged(InName, InValue);
}
