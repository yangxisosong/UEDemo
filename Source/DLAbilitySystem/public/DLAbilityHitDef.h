#pragma once

#include "CoreMinimal.h"
#include "DLAbilitySubObjectLib.h"
#include "DLGameplayAbilityTypes.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "DLAbilityHitDef.generated.h"



USTRUCT(BlueprintType)
struct FDLOutgoingGEDefine
{
	GENERATED_BODY()

public:

	// GE 的资产
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TSubclassOf<UGameplayEffect> GEClass;

	// 这个GE的等级，-1 表示使用技能的等级
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int32 OverrideLevel = -1;

public:

	bool operator==(const FDLOutgoingGEDefine& Other)const
	{
		return Other.OverrideLevel == OverrideLevel && Other.GEClass == GEClass;
	}
};

UCLASS(Abstract, NotBlueprintType, Blueprintable, CollapseCategories, EditInlineNew)
class DLABILITYSYSTEM_API UDLGameplayHitProcessCondition : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool Condition(const FGameplayAbilityTargetDataHandle& TargetDataHandle);


	virtual  bool Condition_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
	{
		return false;
	}
};




UCLASS(NotBlueprintable, Meta = (Displayname = And))
class DLABILITYSYSTEM_API UDLGameplayHitProcessCondition_And
	: public UDLGameplayHitProcessCondition
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Instanced)
		TArray<UDLGameplayHitProcessCondition*> OperateCondition;


protected:

	virtual bool Condition_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override
	{
		for (const auto Condition : OperateCondition)
		{
			if (!Condition)
			{
				continue;
			}

			if (!Condition->Condition(TargetDataHandle))
			{
				return false;
			}
		}

		return true;
	}
};


UCLASS(NotBlueprintable, Meta = (Displayname = Or))
class DLABILITYSYSTEM_API UDLGameplayHitProcessCondition_Or
	: public UDLGameplayHitProcessCondition
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Instanced)
		TArray<UDLGameplayHitProcessCondition*> OperateCondition;

protected:

	virtual bool Condition_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override
	{
		for (const auto Condition : OperateCondition)
		{
			if (!Condition)
			{
				continue;
			}

			if (Condition->Condition(TargetDataHandle))
			{
				return true;
			}
		}

		return false;
	}
};

/**
* @brief 技能 Hit 的定义，定义这个 Hit 的力量、GE 等
*/
USTRUCT(BlueprintType)
struct FDLAbilityHitDefine
	: public FDLAbilityDefineBase
{
	GENERATED_BODY()
public:
	/**
	* @brief 攻击的力量
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Ability.AttackPower"))
		FGameplayTag AttackPower;

	/**
	* @brief 攻击的类型
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Ability.AttackType"))
		FGameplayTag AttackType;

	/**
	* @brief 被攻击的对象 产生的 效果  击退 击倒 击飞 眩晕等
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Ability.UnderAttack"))
		FGameplayTag UnderAttackEffectTag;

	/**
	 * @brief 削韧值  公式-> 实际韧性值=【基础韧性值+装备强韧度】+【出手韧性系数*基础韧性值+出手韧性值】-【削韧*（1-装备削韧减免率）】
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float CutTenacityValue = 0.f;

	/**
	* @brief 应用的 GE
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FDLOutgoingGEDefine> OutgoingGEDefineArray;

	/**
	* @brief 这个 Hit 在计算攻击信息时的策略
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<UCalculateAbilityAttackInfo> CalculateAttackInfoClass;
public:

	FGameplayTagContainer CollectAttackTags()const
	{
		FGameplayTagContainer Ret;
		Ret.AddTag(AttackPower);
		Ret.AddTag(AttackType);
		Ret.AddTag(UnderAttackEffectTag);
		return Ret;
	}
};
