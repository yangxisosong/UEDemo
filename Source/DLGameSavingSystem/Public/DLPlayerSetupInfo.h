#pragma once
#include "CoreMinimal.h"
#include "DLCharacterSetupDef.h"
#include "Abilities/GameplayAbility.h"
#include "DLPlayerSetupInfo.generated.h"


USTRUCT(BlueprintType)
struct FDLSetupAbilityInfo
{
	GENERATED_USTRUCT_BODY()
public:

	// 当授予技能时是否激活
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		bool IsActiveWhenGive = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSoftClassPtr<UGameplayAbility> AbilityClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 InitLevel = 0;
};


USTRUCT(BlueprintType)
struct FDLSetupGameplayEffectInfo
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSoftClassPtr<UGameplayEffect> EffectClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 InitLevel = 0;
};


USTRUCT(BlueprintType)
struct FDLPlayerSetupInfo
{
	GENERATED_BODY()

public:

	// 初始拥有的Buff
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FDLSetupGameplayEffectInfo> StartupGameplayEffects;

	// 初始拥有的技能
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FDLSetupAbilityInfo> StartupGameplayAbilityArr;

	// 当前拥有的武器   TODO 简单的一个方案，理论上说应该是一个数组，但是 后边可能会重新搭建 装备体系  这里就一切从简了  6 月份之后的版本会重新搭建这一块
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FDLSetupWeaponInfo> WeaponInfoArray;
};

