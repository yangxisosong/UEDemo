#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "AbilityChainAsset2.generated.h"

UENUM(BlueprintType)
enum class EPrimaryInputID2 : uint8
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
ENUM_RANGE_BY_COUNT(EPrimaryInputID2, EPrimaryInputID2::Count);

/**
 * @brief 技能连招资产，用来桥接  Editor 与 Runtime 
 */
UCLASS()
class UAbilityChainAsset2 : public UObject
{
	GENERATED_BODY()
public:

	/**
	* @brief 存储技能连招数据，由编辑器生成后设置，并且会被存盘
	*/
	UPROPERTY()
		FString	AbilityChainData;


#if WITH_EDITORONLY_DATA

	/**
	 * @brief 用来过滤编辑器可以支持选择那些目录下的 Ability
	 */
	UPROPERTY(EditAnywhere, meta = (RelativeToGameContentDir, LongPackageName))
		TArray<FDirectoryPath> AbilityDirectoryArray;

	UPROPERTY()
		UEdGraph* EdGraph;
#endif


};
