#include "AbilityChainNodeEditorCommands.h"

#define LOCTEXT_NAMESPACE "TreeNodeEditorCommands"

void FAbilityChainNodeEditorCommands::RegisterCommands()
{
	UI_COMMAND(FileToJson, "FileToJson", "File To Json", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(Build, "Build Json", "Build Json To Asset", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(TestCommands, "TestCommands", "Test Commands", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE