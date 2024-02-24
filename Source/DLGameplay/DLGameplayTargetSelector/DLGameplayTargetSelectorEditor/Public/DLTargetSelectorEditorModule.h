#pragma once
#include "CoreMinimal.h"

class ITargetSelectorEditor;


DECLARE_LOG_CATEGORY_EXTERN(DLTargetSelectorEditor, All, All);


class ITargetSelectorModule
{
public:
	virtual ~ITargetSelectorModule() = default;
	virtual TSharedRef<ITargetSelectorEditor> CreateTargetSelectorEditor(
		const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Object) = 0;


	static ITargetSelectorModule& Get();
};
