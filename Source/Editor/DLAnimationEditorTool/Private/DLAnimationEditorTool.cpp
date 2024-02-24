#pragma once

#include "DLAnimationEditorTool.h"

DEFINE_LOG_CATEGORY(DLAnimationEditorTool);

#define LOCTEXT_NAMESPACE "FDLAnimationEditorTool"

void FDLAnimationEditorTool::StartupModule()
{
	UE_LOG(DLAnimationEditorTool, Warning, TEXT("DLAnimationEditorTool module has started!"));
}

void FDLAnimationEditorTool::ShutdownModule()
{
	UE_LOG(DLAnimationEditorTool, Warning, TEXT("DLAnimationEditorTool module has shut down"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDLAnimationEditorTool, DLAnimationEditorTool)