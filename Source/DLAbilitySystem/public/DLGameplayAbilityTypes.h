#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "DLGameplayAbilityTypes.generated.h"


USTRUCT(BlueprintType)
struct DLABILITYSYSTEM_API FDLAbilityEventData
	: public FGameplayEventData
{
	GENERATED_USTRUCT_BODY()

public:

	// 技能动作释放的方向  比如向左边攻击、向右边挥砍
	// 这个字段 一般是 从动画的 配置中 转发过来的
	UPROPERTY(BlueprintReadWrite)
		FGameplayTag AbilityAnimationDirectionTag;

	UPROPERTY(BlueprintReadWrite)
		TArray<FHitResult> HitResults;
};

USTRUCT(BlueprintType)
struct FDLAbilityDefineBase
{
	GENERATED_BODY()

public:


	/**
	* @brief 定义的标识
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FGameplayTag DefineId;


public:

	bool operator==(const FGameplayTag& ID)const
	{
		return DefineId == ID;
	}
};