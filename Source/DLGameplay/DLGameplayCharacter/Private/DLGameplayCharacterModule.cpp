
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "DLGamePlayCharacterDef.h"

struct FDLGameplayCharacter : public IModuleInterface
{
};

IMPLEMENT_MODULE(FDLGameplayCharacter, DLGameplayCharacter);

DEFINE_LOG_CATEGORY(LogDLGamePlayChracter);
DEFINE_LOG_CATEGORY(LogDLGamePlayPlayerController);
DEFINE_LOG_CATEGORY(LogDLGamePlayChracterAnim);
