// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IConsoleVariablesEditorListFilter.h"

#define LOCTEXT_NAMESPACE "ConsoleVariablesEditor"

class ConsoleVariablesEditorListFilter_ShowOnlyVariables : public IConsoleVariablesEditorListFilter
{
public:

	ConsoleVariablesEditorListFilter_ShowOnlyVariables()
	{
		// This filter should be off by default
		SetFilterActive(false);
		SetFilterMatchType(EConsoleVariablesEditorListFilterMatchType::MatchAll);
	}

	virtual FString GetFilterName() override
	{
		return "Show Only Variables";
	}

	virtual FText GetFilterButtonLabel() override
	{
		return LOCTEXT("ShowOnlyVariablesFilter", "Show Only Variables");
	}

	virtual FText GetFilterButtonToolTip() override
	{
		return LOCTEXT("ShowOnlyVariablesFilterTooltip", "Show only rows that represent console variables.");
	}

	virtual bool DoesItemPassFilter(const FConsoleVariablesEditorListRowPtr& InItem) override
	{
		if (InItem.IsValid())
		{
			if (const TSharedPtr<FConsoleVariablesEditorCommandInfo> PinnedCommand = InItem->GetCommandInfo().Pin())
			{
				return PinnedCommand->ObjectType == FConsoleVariablesEditorCommandInfo::EConsoleObjectType::Variable;
			}
		}

		return false;
	}
};

#undef LOCTEXT_NAMESPACE
