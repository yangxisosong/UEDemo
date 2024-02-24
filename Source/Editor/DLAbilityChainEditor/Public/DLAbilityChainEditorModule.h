#pragma once
#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"

class IDLAbilityChainEditor;

DECLARE_LOG_CATEGORY_EXTERN(DLTreeNodeEditor, All, All);

class IAbilityChainNodeModule
{
public:
	virtual ~IAbilityChainNodeModule() = default;
	virtual TSharedRef<IDLAbilityChainEditor> CreateAbilityChainNodeEditor(
		const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Object) = 0;


	static IAbilityChainNodeModule& Get();
};
