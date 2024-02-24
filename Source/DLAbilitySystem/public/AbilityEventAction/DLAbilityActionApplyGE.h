#pragma once
#include "DLAbilityAction.h"
#include "DLGameplayAbilityBase.h"
#include "DLGameplayEffectContext.h"
#include "DLAbilityActionApplyGE.generated.h"

UENUM(BlueprintType)
enum class EDLActionApplyGETargetType : uint8
{
	Self,
	TargetData,
};


UCLASS(DisplayName = ApplyGE)
class UDLAbilityActionApplyGE
	: public UDLAbilityAction
{
	GENERATED_BODY()

protected:

	virtual void OnExec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event) override
	{
		const auto Ability = Context.Ability;

		const int32 Level = (OverrideLevel == -1 ? Ability->GetAbilityLevel() : OverrideLevel);

		if (ApplyTargetType == EDLActionApplyGETargetType::Self)
		{
			for (const auto& EffectClass : ApplyEffectClassArray)
			{
				const auto Specs = Ability->MakeOutgoingGameplayEffectSpec(EffectClass, Level);
				Ability->K2_ApplyGameplayEffectSpecToOwner(Specs);
			}
		}
		else
		{
			const auto HitActorInterface = GetActionFeature<IDLAbilityAFHitActor>(Event);

			if (!HitActorInterface)
			{
				return;
			}

			const auto TargetData = HitActorInterface->GetTargetData();
			if (TargetData.Num() == 0)
			{
				return;
			}

			for (const auto& EffectClass : ApplyEffectClassArray)
			{
				const auto Specs = Ability->MakeOutgoingGameplayEffectSpec(EffectClass, Level);
				Ability->K2_ApplyGameplayEffectSpecToTarget(Specs, TargetData);
			}
		}
	}


public:

	// 应用的目标类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EDLActionApplyGETargetType ApplyTargetType = EDLActionApplyGETargetType::TargetData;

	// 使用Level覆盖当前的技能等级,  -1  为不进行覆盖
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 OverrideLevel = -1;

	// Apply GE 的数组
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<TSubclassOf<UGameplayEffect>> ApplyEffectClassArray;
};