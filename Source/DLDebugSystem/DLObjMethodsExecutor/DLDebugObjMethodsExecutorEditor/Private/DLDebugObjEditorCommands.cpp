#include "DLDebugObjEditorCommands.h"

#define LOCTEXT_NAMESPACE "FDLDebugObjMethodsExecutorEditor"

void FDLDebugObjEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenDebugWindow, "DLDebugObjEditor", "Open Debug window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE