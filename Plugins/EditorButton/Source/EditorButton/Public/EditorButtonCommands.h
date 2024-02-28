// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorButtonStyle.h"

class FEditorButtonCommands : public TCommands<FEditorButtonCommands>
{
public:

	FEditorButtonCommands()
		: TCommands<FEditorButtonCommands>(TEXT("EditorButton"), NSLOCTEXT("Contexts", "EditorButton", "EditorButton Plugin"), NAME_None, FEditorButtonStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
