#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "AbilityChainNodeEditorStyle.h"

class FAbilityChainNodeEditorCommands : public TCommands<FAbilityChainNodeEditorCommands>
{
public:

	FAbilityChainNodeEditorCommands()
		: TCommands<FAbilityChainNodeEditorCommands>(TEXT("EditorCommands"), 
			NSLOCTEXT("AbilityChainNodeEditor.Common", "Common", "Common"), NAME_None, 
			FAbilityChainNodeEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	//注册命令
	TSharedPtr< FUICommandInfo > FileToJson;

	TSharedPtr< FUICommandInfo > Build;

	TSharedPtr< FUICommandInfo > TestCommands;
};
