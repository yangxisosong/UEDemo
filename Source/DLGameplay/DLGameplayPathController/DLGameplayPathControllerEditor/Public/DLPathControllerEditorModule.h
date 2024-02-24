#pragma once
#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"

class IPathControllerEditor;


DECLARE_LOG_CATEGORY_EXTERN(DLGameplayPathControllerEditor, All, All);


class IPathControllerModule
{
public:
	virtual ~IPathControllerModule() = default;
	virtual TSharedRef<IPathControllerEditor> CreatePathControllerEditor(
		const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Object) = 0;


	static IPathControllerModule& Get();
};
