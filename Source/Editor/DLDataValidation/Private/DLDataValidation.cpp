#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"


class FDLDataValidation : public IModuleInterface
{
public:

	virtual void StartupModule() override
	{
		// TODO 一些加载项
	}
	
	virtual void ShutdownModule() override
	{
		
	}
};


IMPLEMENT_MODULE(FDLDataValidation, DLDataValidation)