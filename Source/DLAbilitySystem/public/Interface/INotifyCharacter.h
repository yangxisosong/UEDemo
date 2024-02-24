#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "INotifyCharacter.generated.h"



USTRUCT(BlueprintType)
struct FWeaponAttackParameter
{
	GENERATED_BODY()

public:

	// 技能中监听击中事件的 Tag, 这个Tag需要与技能中配置的监听动画事件的 Tag 一致
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Ability.ATKnumber"))
		FGameplayTag EventTag;

	// 挂载武器的插槽
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Gameplay.Character.BoneSocket"))
		FGameplayTagContainer SlotMountWeapon;


};


UINTERFACE(MinimalAPI)
class UNotifyCharacterWeapon
	: public UInterface
{
	GENERATED_BODY()
};


class DLABILITYSYSTEM_API INotifyCharacterWeapon
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (AutoCreateRefTerm = "Parameter"))
		void BeginAttack(const FWeaponAttackParameter& Parameter);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void EndAttack(const FWeaponAttackParameter& Parameter);

};


USTRUCT(BlueprintType)
struct FCharacterFrameFrozenArg
{
	GENERATED_BODY()
public:

	/**
	* @brief 定帧的持续时间
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Duration = 0.f;

	/**
	* @brief 定帧时的 时间 动画播放速度的曲线  归一化
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UCurveFloat* AnimSpeedCurve;

	/**
	* @brief 定帧的类型
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Gameplay.Character.FrameFrozenType"))
		FGameplayTag FrameFrozenType;

	/**
	* @brief 需要定帧的部位
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FGameplayTagContainer Parts;

	/**
	 * @brief 被打时的 Tags 信息，只有被打触发的才有效
	 */
	UPROPERTY(BlueprintReadWrite)
		FGameplayTagContainer UnderAttackTags;
};


UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UNotifyCharacterFrameFrozen
	: public UInterface
{
	GENERATED_BODY()
};

class DLABILITYSYSTEM_API INotifyCharacterFrameFrozen : public IInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
		virtual void FrameFrozen(FCharacterFrameFrozenArg Arg) {};

	UFUNCTION(BlueprintCallable)
		virtual void ForwardTargetCharacterFrameFrozen(AActor* Target, FCharacterFrameFrozenArg Arg) {};
};

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UNotifyCharacterHideMesh
	: public UInterface
{
	GENERATED_BODY()
};

class DLABILITYSYSTEM_API INotifyCharacterHideMesh : public IInterface
{
	GENERATED_BODY()
public:

	/**
	 * @brief 控制角色Mesh的显隐
	 * @param Reason 操纵Mesh的缘由
	 * @param bPropagateToChildren 是否对子项进行此操作
	 * @param bVisible 是否可见
	 */
	UFUNCTION(BlueprintCallable)
		virtual void SetMeshVisibility(const FString& Reason, bool bPropagateToChildren, bool bVisible) {};
};


UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UNotifyCharacterChangeCollision
	: public UInterface
{
	GENERATED_BODY()
};

class DLABILITYSYSTEM_API INotifyCharacterChangeCollision : public IInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, DisplayName = ChangeCollisionPreset)
		virtual void K2_ChangeCollisionPreset(const FName& CollisionPreset) {};

	UFUNCTION(BlueprintCallable, DisplayName = ChangeCollisionResponseToChannel)
		virtual void K2_ChangeCollisionResponseToChannel(ECollisionChannel Channel, ECollisionResponse NewResponse) {};

	UFUNCTION(BlueprintCallable, DisplayName = ChangeCollisionResponseToAllChannels)
		virtual void K2_ChangeCollisionResponseToAllChannels(ECollisionResponse NewResponse) {};

};

