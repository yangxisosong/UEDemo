
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "DLDrawDebugDef.h"

struct FDLDrawDebugSystem : public IModuleInterface
{
};

IMPLEMENT_MODULE(FDLDrawDebugSystem, DLDrawDebugSystem);
DEFINE_LOG_CATEGORY(LogDL_DrawDebugSystem);
