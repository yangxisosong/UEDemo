#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"


class FGameTestModule
	: public IModuleInterface
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};


IMPLEMENT_MODULE(FGameTestModule, GameTestModule)