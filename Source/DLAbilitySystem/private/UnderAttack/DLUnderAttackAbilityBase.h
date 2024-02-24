#pragma once

#include "CoreMinimal.h"
#include "DLGameplayAbilityBase.h"
#include "DLUnderAttackAbilityBase.generated.h"


USTRUCT(BlueprintType)
struct FDLUnderAttackContext
{
	GENERATED_BODY()
public:

	// 标识被打的这个行为
	UPROPERTY(BlueprintReadWrite)
		FGameplayTagContainer  AttackTags;

	// 教唆者
	UPROPERTY(BlueprintReadWrite)
		AActor* Instigate = nullptr;

	// 方向信息
	UPROPERTY(BlueprintReadWrite)
		FGameplayTag AttackDirectionTag;

	// 攻击者是否在被攻击者的前方 
	UPROPERTY(BlueprintReadWrite)
		bool  InFront = false;

	// 击中信息
	UPROPERTY(BlueprintReadWrite)
		FHitResult HitResult;
};


USTRUCT(BlueprintType)
struct FDLUnderAttackEventData : public FGameplayEventData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
		FDLUnderAttackContext AttackContext;
};


// 没错 被打也是一种能力 !
UCLASS()
class UDLUnderAttackAbilityBase
	: public UDLGameplayAbilityBase
{
	GENERATED_BODY()
public:


	UFUNCTION(BlueprintImplementableEvent)
		void ActivateUnderAttack(const FDLUnderAttackContext& Context);

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
									const FGameplayAbilityActorInfo* ActorInfo,
									const FGameplayAbilityActivationInfo ActivationInfo,
									const FGameplayEventData* TriggerEventData) override
	{
		if (ensureAlwaysMsgf(TriggerEventData, TEXT("一定是通过事件这种方式触发的")))
		{
			const FDLUnderAttackEventData* Data = static_cast<const FDLUnderAttackEventData*>(TriggerEventData);

			FDLUnderAttackContext Context = Data->AttackContext;
			Context.AttackTags.AddTag(Data->EventTag);

			this->ActivateUnderAttack(Data->AttackContext);
		}
	}

};


