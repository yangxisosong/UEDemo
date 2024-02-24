#pragma once
#include "DLGameplayAbilityTypes.h"
#include "Interface/ICharacterAttack.h"
#include "DLCalculateAbilityAttackInfo.generated.h"

class UDLGameplayAbilityBase;
USTRUCT(BlueprintType)
struct FDLAttackActorArg
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
		FDLAbilityEventData EventData;

	UPROPERTY(BlueprintReadWrite)
		FGameplayTagContainer AttackTags;
};


USTRUCT(BlueprintType)
struct FDLCalculateAttackInfoTargetInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
		AActor* Target;

	UPROPERTY(BlueprintReadWrite)
		FGameplayTagContainer Tags;

	UPROPERTY(BlueprintReadWrite)
		FHitResult HitResult;
};

/**
* @brief 计算技能的攻击信息的策略类，使用其 DefaultObj
*/
UCLASS(MinimalAPI, Blueprintable, NotBlueprintType, Abstract)
class UCalculateAbilityAttackInfo : public UObject
{
	GENERATED_BODY()
public:

	/**
	* @brief 计算技能攻击的表现数据
	* @param Ability 技能
	* @param TargetInfoArray 目标的 的信息
	* @param Instigate 技能的教唆者
	* @param EffectCauser 攻击的造成者
	* @param EffectTrigger 攻击的触发者，可能是子物体 等等
	* @param OutInfo 输出表现数据
	*/
	UFUNCTION(BlueprintNativeEvent)
		void Exec(UDLGameplayAbilityBase* Ability,
			const TArray<FDLCalculateAttackInfoTargetInfo>& TargetInfoArray,
			AActor* Instigate,
			AActor* EffectCauser,
			AActor* EffectTrigger,
			TArray<FCharacterAttackArg>& OutInfo);
};