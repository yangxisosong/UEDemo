#pragma once
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(DLAnimGraph, All, All);

class FDLAnimGraph : public IModuleInterface
{
public:

	/* This will get called when the editor loads the module */
	virtual void StartupModule() override;

	/* This will get called when the editor unloads the module */
	virtual void ShutdownModule() override;
};
