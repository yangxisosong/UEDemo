// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "DLDebugObjEditorStyle.h"

class FDLDebugObjEditorCommands : public TCommands<FDLDebugObjEditorCommands>
{
public:

	FDLDebugObjEditorCommands()
		: TCommands<FDLDebugObjEditorCommands>(TEXT("MyTest"), NSLOCTEXT("Contexts", "MyTest", "MyTest Plugin"), NAME_None, FDLDebugObjEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	//注册命令
	TSharedPtr< FUICommandInfo > OpenDebugWindow;
};