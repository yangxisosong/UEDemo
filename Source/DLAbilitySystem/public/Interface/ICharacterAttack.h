#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "ICharacterAttack.generated.h"

UINTERFACE()
class UCharacterAttack : public UInterface
{
	GENERATED_BODY()
};


USTRUCT(BlueprintType)
struct FCharacterAttackArg
{
	GENERATED_BODY()
public:


	// 目标信息
	UPROPERTY(BlueprintReadWrite)
		AActor* Target = nullptr;

	// 教唆者
	UPROPERTY(BlueprintReadWrite)
		AActor* EffectCauser  = nullptr;

	// Hit 的朝向
	UPROPERTY(BlueprintReadWrite)
		FVector HitForceDirection;

	// 标识被打的这个行为
	UPROPERTY(BlueprintReadWrite)
		FGameplayTagContainer  AttackTags;

	// Hit 数据
	UPROPERTY(BlueprintReadWrite)
		FHitResult HitResult;
};


class DLABILITYSYSTEM_API ICharacterAttack
{
	GENERATED_BODY()

public:
	virtual void AttackActors(const TArray<FCharacterAttackArg>& AttackArgArr) = 0;
};
