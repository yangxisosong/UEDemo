
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

struct FDLAssetManager : public IModuleInterface
{
};

IMPLEMENT_MODULE(FDLAssetManager, DLAssetManager);
