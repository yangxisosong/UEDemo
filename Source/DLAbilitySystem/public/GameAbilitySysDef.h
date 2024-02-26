#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySysDef.generated.h"


UENUM(BlueprintType)
enum class EPrimaryInputID : uint8
{
	None,

	KeyOne UMETA(DisplayName = "攻击 1 鼠标左"),
	KeyTwo UMETA(DisplayName = "攻击 2 鼠标右"),

	KeyFunction1 UMETA(DisplayName = "数字键 1"),
	KeyFunction2 UMETA(DisplayName = "数字键 2"),
	KeyFunction3 UMETA(DisplayName = "数字键 3"),
	KeyFunction4 UMETA(DisplayName = "数字键 4"),
	KeyFunction5 UMETA(DisplayName = "数字键 5"),

	KeyCombOne UMETA(DisplayName = "组合按键 1 鼠标左 + Shift"),
	KeyCombTwo UMETA(DisplayName = "组合按键 2 待定"),
	
	DebugKeyBegin UMETA(Hidden),
	DebugKey1 UMETA(DisplayName = "调试按键 1"),
	DebugKey2 UMETA(DisplayName = "调试按键 2"),
	DebugKey3 UMETA(DisplayName = "调试按键 3"),
	DebugKey4 UMETA(DisplayName = "调试按键 4"),
	DebugKey5 UMETA(DisplayName = "调试按键 5"),
	DebugKey6 UMETA(DisplayName = "调试按键 6"),
	DebugKey7 UMETA(DisplayName = "调试按键 7"),
	DebugKey8 UMETA(DisplayName = "调试按键 8"),
	DebugKey9 UMETA(DisplayName = "调试按键 9"),
	DebugKillSelf UMETA(DisplayName = "调试按键长按P[自杀]"),
	DebugKillOtherNPC UMETA(DisplayName = "调试按键长按L[杀其他人]"),
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EPrimaryInputID, EPrimaryInputID::Count);

inline bool IsValidPrimaryInputId(const EPrimaryInputID Id)
{
	return Id != EPrimaryInputID::None; 
}

inline FString ToString(EPrimaryInputID Id)
{
	return StaticEnum<EPrimaryInputID>()->GetNameStringByValue(static_cast<int32>(Id));
}

UENUM(BlueprintType)
enum class EDLAbilityStage : uint8
{
	None,

	// 引导阶段前
	ChannelPre,

	// 引导阶段中
	Channel,

	// 引导阶段后
	ChannelPost,

	// 前摇阶段
	Pre,
	// 施法阶段
	Spell,
	// 后摇阶段
	Post,
};

inline FString ToString(EDLAbilityStage Stage)
{
	return StaticEnum<EDLAbilityStage>()->GetNameStringByValue(static_cast<int32>(Stage));
}