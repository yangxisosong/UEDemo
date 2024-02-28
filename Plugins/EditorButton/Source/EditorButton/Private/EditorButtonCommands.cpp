// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorButtonCommands.h"

#define LOCTEXT_NAMESPACE "FEditorButtonModule"

void FEditorButtonCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "EditorButton", "Execute EditorButton action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
