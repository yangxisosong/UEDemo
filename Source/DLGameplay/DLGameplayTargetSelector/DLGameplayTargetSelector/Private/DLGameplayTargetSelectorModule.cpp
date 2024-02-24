#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"


struct FDLGameplayTargetSelectorModule : public IModuleInterface
{
	virtual void StartupModule() override
	{
	}

	virtual void ShutdownModule() override
	{
	}
};

IMPLEMENT_MODULE(FDLGameplayTargetSelectorModule, DLGameplayTargetSelector);
