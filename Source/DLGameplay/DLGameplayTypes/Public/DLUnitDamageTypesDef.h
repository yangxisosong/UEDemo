#pragma once
#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "DLUnitDamageTypesDef.generated.h"

UCLASS(Abstract, MinimalAPI)
class UDLDamageTypeBase : public UDamageType
{
	GENERATED_BODY()
};


/*
 *  定义 Actor 中处理伤害的 伤害类型
 *
 *	可以参看   AActor::TakeDamage
 *			  AActor::OnTakeAnyDamage
 */


/**
 * @brief 标准的伤害类型，没有除去护盾等可以当作生命值机制抵消的部分
 */
UCLASS(MinimalAPI)
class UDLDamage : public UDLDamageTypeBase
{
	GENERATED_BODY()
};


/**
 * @brief 真正扣除血量的伤害
 */
UCLASS(MinimalAPI)
class UDLRealDamage : public UDLDamageTypeBase
{
	GENERATED_BODY()
};

