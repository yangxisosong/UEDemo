
#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Modules/ModuleInterface.h"
#include "AppFrameworkMessageDef.h"

struct FDLAppFramework : public IModuleInterface
{
};

IMPLEMENT_MODULE(FDLAppFramework, DLAppFramework);

namespace  EFrameworkMsgType
{
	UE_DEFINE_GAMEPLAY_TAG(GameInstanceInitPost, "DLMsgType.Framework.GameInstanceInitPost");
	UE_DEFINE_GAMEPLAY_TAG(LocalPlayerInitPost, "DLMsgType.Framework.LocalPlayerInitPost");
	UE_DEFINE_GAMEPLAY_TAG(LocalPlayerGameplayReady, "DLMsgType.Framework.LocalPlayerGameplayReady");
	UE_DEFINE_GAMEPLAY_TAG(FrontEndDeinit, "DLMsgType.Framework.FrontEndDeinit");
	UE_DEFINE_GAMEPLAY_TAG(FrontEndInit, "DLMsgType.Framework.FrontEndInit");
};