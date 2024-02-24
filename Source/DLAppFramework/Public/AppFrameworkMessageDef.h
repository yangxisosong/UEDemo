#pragma once
#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "AppFrameworkMessageDef.generated.h"

namespace EFrameworkMsgType
{
	DLAPPFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameInstanceInitPost);
	DLAPPFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LocalPlayerInitPost);
	DLAPPFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LocalPlayerGameplayReady);
	DLAPPFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FrontEndInit);
	DLAPPFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FrontEndDeinit);
};

// 程序框架 使用的消息数据
USTRUCT(BlueprintType)
struct FDLAppFrameworkMsg
{
	GENERATED_BODY()
public:


};
