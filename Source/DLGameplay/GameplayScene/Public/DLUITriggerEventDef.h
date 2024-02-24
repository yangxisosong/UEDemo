#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "DLUITriggerEventDef.generated.h"

namespace EDLGameplayTriggerMsg
{
	GAMEPLAYSCENE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayTrigger_ShowButtonTips);
	GAMEPLAYSCENE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayTrigger_HideButtonTips);
	GAMEPLAYSCENE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayTrigger_ButtonClick);
}

USTRUCT(BlueprintType)
struct GAMEPLAYSCENE_API FDLUIShowButtonTipsMsgBody
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FKey Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText TipsText;
};

USTRUCT(BlueprintType)
struct GAMEPLAYSCENE_API FDLUITriggerButtonClickMsgBody
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FKey Key;
};