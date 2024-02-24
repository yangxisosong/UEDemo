
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "DLGameplayPredictTaskLog.h"

struct FDLGameplayPredictionTask : public IModuleInterface
{
};

IMPLEMENT_MODULE(FDLGameplayPredictionTask, DLGameplayPredictionTask);

DEFINE_LOG_CATEGORY(LogDLGameplayPredictTask);
