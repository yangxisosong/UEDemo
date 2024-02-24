#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "DLGamePlayCharacterDef.h"

#define IS_ANIM_LOG_ENABLED() false

#define DL_ANIM_LOG(Verbosity, Format, ...) \
{\
	if(IS_ANIM_LOG_ENABLED()) \
	{\
		UE_LOG(LogDLGamePlayChracterAnim, Verbosity, Format, __VA_ARGS__);\
	}\
}

#define DL_ANIM_LOG_WITHOUT_VALUE(Verbosity, Format) \
{\
	if(IS_ANIM_LOG_ENABLED()) \
	{\
		UE_LOG(LogDLGamePlayChracterAnim, Verbosity, Format);\
	}\
}
