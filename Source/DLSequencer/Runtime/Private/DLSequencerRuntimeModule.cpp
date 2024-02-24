
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

struct FDLSequencerRuntime : public IModuleInterface
{
};

IMPLEMENT_MODULE(FDLSequencerRuntime, DLSequencerRuntime);
