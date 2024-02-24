#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "INotifyCharacter.h"
#include "IDLAbilityWeaponAttack.generated.h"

USTRUCT(BlueprintType)
struct FWeaponAttackNtfInfo
{
	GENERATED_BODY()

public:

	// 技能中监听击中事件的 Tag, 这个Tag需要与技能中配置的监听动画事件的 Tag 一致
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Ability.ATKnumber", ValidationTagVaild))
		FGameplayTag EventTag;

	// 挂载武器的插槽
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Gameplay.Character.BoneSocket", ValidationTagVaild))
		FGameplayTagContainer SlotMountWeapon;

	// 攻击方向的 Tag
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Gameplay.AttackDirection", ValidationTagVaild))
		FGameplayTag AttackDirectionTag;

	// 单位选择的定义
	UPROPERTY(EditAnywhere, meta=(ValidationTagVaild))
		FGameplayTag UnitSelectDefineId;
};

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class UDLAbilityWeaponAttack : public UInterface
{
	GENERATED_BODY()
};

class IDLAbilityWeaponAttack : public IInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	virtual void BeginAttack(const FWeaponAttackNtfInfo& Parameter) {};

	UFUNCTION(BlueprintCallable)
	virtual void EndAttack(const FWeaponAttackNtfInfo& Parameter) {};

};