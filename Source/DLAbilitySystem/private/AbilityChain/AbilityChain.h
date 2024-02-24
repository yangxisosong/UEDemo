
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameAbilitySysDef.h"
#include "AbilityChain.generated.h"


class UAbilityChainAsset;
class UDLGameplayAbilityBase;

struct FAbilityChainInputKey
{
	FAbilityChainInputKey() {}

	explicit FAbilityChainInputKey(const FString& Str)
	{
		auto Index = StaticEnum<EPrimaryInputID>()->GetIndexByNameString(Str);
		if (Index != INDEX_NONE)
		{
			KeyIndex = static_cast<EPrimaryInputID>(Index);
		}
	}

	explicit FAbilityChainInputKey(const EPrimaryInputID ID)
	{
		KeyIndex = ID;
	}

	EPrimaryInputID KeyIndex = EPrimaryInputID::None;

	bool operator==(const FAbilityChainInputKey& Other)const
	{
		return KeyIndex == Other.KeyIndex;
	}

	bool operator!=(const FAbilityChainInputKey& Other)const
	{
		return KeyIndex != Other.KeyIndex;
	}

	static const FAbilityChainInputKey None;

	FString ToString()const
	{
		return StaticEnum<EPrimaryInputID>()->GetNameStringByValue(static_cast<int32>(KeyIndex));
	}
};

USTRUCT()
struct FAlternateAbilityDefine
{
	GENERATED_BODY()
public:

	// 备选技能的优先级  值越大 越先被评估
	UPROPERTY(EditAnywhere)
		int32 Priority = 0;

	// 备选技能的 Class
	UPROPERTY(EditAnywhere)
		TSoftClassPtr<UDLGameplayAbilityBase> AbilityClass;
};


struct FAbilityChainNodeDefine
{
public:

	FAbilityChainInputKey InputTags;

	// 备选技能列表
	TArray<FAlternateAbilityDefine> AlternateAbilityList;

	// 子节点
	TArray<TSharedPtr<FAbilityChainNodeDefine>> ChildNodes;
};


// 技能链的评估上下文
struct FAbilityChainEvaluateContext
{
	//记录已经CheckOut的输入
	TArray<FAbilityChainInputKey> InputIdList;
};


UCLASS(Blueprintable)
class DLABILITYSYSTEM_API UAbilityChain : public UObject
{
	GENERATED_BODY()

private:

	// 技能链根节点
	TSharedPtr<FAbilityChainNodeDefine> AbilityRoot;

	bool EnabledAcceptInput = false;

	FAbilityChainInputKey CurrentInputKey;

	FAbilityChainInputKey LastInputKey;

	// 评估上下文
	TUniquePtr<FAbilityChainEvaluateContext> EvaluateContext;

	// 已经被检出的技能列表
	UPROPERTY(Transient)
	TArray<TSubclassOf<UGameplayAbility>> CheckOutAbilityList;

	FString AbilityChainListDebugString;

protected:

	// 搜索一个符合输入列表的技能节点
	TSharedPtr<FAbilityChainNodeDefine> SearchMatchAbilityChainNode(const TArray<FAbilityChainInputKey>& InputIdList) const;

public:

	/**
	 * @brief 由外部系统注入输入
	 * @param InputKeys 
	 */
	void InjectInput(const FAbilityChainInputKey& InputKeys);

	/**
	 * @brief 设置是否接受输入, 可以起到屏蔽连招系统的作用
	 * @param AcceptInput 
	 */
	void SetEnabledAcceptInput(const bool AcceptInput)
	{
		EnabledAcceptInput = AcceptInput;
	}

	struct FCheckOutAbilityArg
	{
		TWeakObjectPtr<UAbilitySystemComponent> ASC;
		const FGameplayTagContainer* TargetTags = nullptr;
		const FGameplayTagContainer* SourceTags = nullptr;

		// 当前正在活跃的技能
		TWeakObjectPtr<UDLGameplayAbilityBase> CurrentActiveAbility;
	};

	/**
	 * @brief 检出一个备选的技能
	 * @param Arg 
	 * @return 
	 */
	TSubclassOf<UDLGameplayAbilityBase> CheckoutAlternateAbility(const FCheckOutAbilityArg& Arg);


	/**
	 * @brief 提交被检出技能的使用情况
	 * @param AbilityClass 
	 */
	void CommitCheckOutAbility(TSubclassOf<UDLGameplayAbilityBase> AbilityClass);


	/**
	 * @brief 确认被检出的技能执行完毕
	 * @param AbilityClass 
	 */
	void AckCheckOutAbility(TSubclassOf<UDLGameplayAbilityBase> AbilityClass);

	/**
	 * @brief 获取上一次输入的 Key
	 * @return 
	 */
	FAbilityChainInputKey GetLastInputId()const { return  LastInputKey; }

	/**
	 * @brief 获取调试字符串，字符串返回当前的连招状态以及连招结构
	 * @return 
	 */
	FString GetDebugString()const;

	/**
	 * @brief 初始化连招 使用连招资产
	 * @param Asset 
	 * @return 
	 */
	bool Init(UAbilityChainAsset* Asset);

private:

	/*
	 * 构建技能连招Tree

	 {
		"InputID": "",
		"AlternateAbilityList": [],
		"ChildNodes": [
			{
				"InputID": "KeyOne",
				"AlternateAbilityList":[
					{
						"Priority":0,
						"AbilityClass":"Blueprint'/Game/DL/Character/ZhuZhuQing/Ability/Atk_ClawLight1/GA_Atk_ClawLight1.GA_Atk_ClawLight1_C'"
					}
				],
				"ChildNodes":[
					{
						"InputID" : "KeyOne",
						"AlternateAbilityList":[
							{
								"Priority":0,
								"AbilityClass":"Blueprint'/Game/DL/Character/ZhuZhuQing/Ability/Atk_ClawLight1/GA_Atk_ClawLight1.GA_Atk_ClawLight1_C'"
							}
						],
						"ChildNodes":[]
					}
				]
			}
		]
	 }

	 */
	bool BuildAbilityChainTree(const FString& InAbilityJson);

};






