#pragma once
#include "CoreMinimal.h"
#include "DLUnitSelectorFitler.h"
#include "GameplayTags.h"
#include "WeaponTypes.generated.h"





// 武器攻击的上下文信息
// 这部分数据显然是动态给武器指定的，以改变武器的一些行为
USTRUCT(BlueprintType)
struct FWeaponAttackContext
{
	GENERATED_BODY()

public:

	/**
	 * @brief 过滤器的原型对象
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UDLUnitSelectorFilterBase* SelectorFilterPrototype;

	/**
	 * @brief 覆盖默认的判定器
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
		class UDLGameplayTargetSelectorShapeTraceBase* OverrideWeaponSelector;

	/**
	 * @brief 携带各种攻击的 Tag 信息
	 */
	UPROPERTY(BlueprintReadWrite)
		FGameplayTagContainer AttackTags;
};