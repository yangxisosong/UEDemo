#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DTHeroInfo.generated.h"

class UDLGameplayAbilityBase;
class UGameplayEffect;
class UEntityInitSpec;
class ADLCharacterBase;

USTRUCT(BlueprintType)
struct FGASpecInitInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<UDLGameplayAbilityBase> AbilityClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 InitLevel = 0;
};


USTRUCT(BlueprintType)
struct FGESpecInitInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 InitLevel = 0;
};


UCLASS(Blueprintable)
class UEntityInitSpec
	: public UObject
{
	GENERATED_BODY()
public:
	// 初始拥有的Buff
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FGESpecInitInfo> StartupGameplayEffects;

	// 初始拥有的技能
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FGASpecInitInfo> StartupGameplayAbilityArr;

	// 初始的 Attributes
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FGESpecInitInfo InitAttributes;
};


USTRUCT(BlueprintType)
struct FDTHeroInfo
	: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 id = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSoftClassPtr<UEntityInitSpec> entityInitSpec;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSoftClassPtr<ADLCharacterBase> heroCharacterBaseClass;
};



