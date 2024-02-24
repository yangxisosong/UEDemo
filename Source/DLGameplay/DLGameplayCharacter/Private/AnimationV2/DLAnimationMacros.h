#pragma once
#include "CoreMinimal.h"
#include "DLGamePlayCharacterDef.h"
#include "NativeGameplayTags.h"
#include "GameplayTagContainer.h"

static TAutoConsoleVariable<bool> CVarAnimationDebugLogEnable(
   TEXT("DL.Animation.EnableLog"),
   false,
   TEXT("显示动画调试日志"),
   ECVF_Cheat
);

#define IS_ANIM_DEBUG_LOG_ENABLED() CVarAnimationDebugLogEnable.GetValueOnGameThread()

#define DL_ANIM_DEBUG_LOG(Verbosity, Format, ...) \
{\
	if(IS_ANIM_DEBUG_LOG_ENABLED()) \
	{\
		UE_LOG(LogDLGamePlayChracterAnim, Verbosity, Format, __VA_ARGS__);\
	}\
}

#define DL_ANIM_DEBUG_LOG_WITHOUT_VALUE(Verbosity, Format) \
{\
	if(IS_ANIM_DEBUG_LOG_ENABLED()) \
	{\
		UE_LOG(LogDLGamePlayChracterAnim, Verbosity, Format);\
	}\
}

inline FName DLTagBuild(FString Scope, const FString& Tag)
{
	Scope.AppendChar('.').Append(Tag);
	return FName(Scope);
}

#define DECLARE_ANIM_TAG(Scope,Tag) \
		UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag, DLTagBuild(#Scope, #Tag));

#define CHECK_ANIM_INS(AnimIns) \
{ \
	if (!ensure(AnimIns)) \
		return; \
}

DECLARE_CYCLE_STAT(TEXT("DLAnimInstance_CopyNewAnimCharacterInfo"), STAT_DLAnimInstance_CopyNewAnimCharacterInfo, STATGROUP_Anim);

