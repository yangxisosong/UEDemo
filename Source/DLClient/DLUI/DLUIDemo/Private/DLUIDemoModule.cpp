
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

struct FDLUIDemo : public IModuleInterface
{
	virtual void ShutdownModule() override {};
	virtual void StartupModule() override {};
};

IMPLEMENT_MODULE(FDLUIDemo, DLUIDemo);
