
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "DLSubObjectLogDef.h"

struct FDLGameplaySubObject : public IModuleInterface
{
};

IMPLEMENT_MODULE(FDLGameplaySubObject, DLGameplaySubObject);


DEFINE_LOG_CATEGORY(LogDLSubObj);
