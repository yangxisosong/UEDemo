#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "ICharacterStateAccessor.generated.h"


class UDLLockPointComponent;
UINTERFACE(MinimalAPI, BlueprintType)
class UCharacterStateAccessor : public UInterface
{
	GENERATED_BODY()
};


class DLGAMEPLAYTYPES_API ICharacterStateAccessor
{
	GENERATED_BODY()

public:

	/**
	 * @brief 获取角色当前的状态 Tag
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName = GetCurrentCharacterState)
		FGameplayTagContainer k2_GetCurrentCharacterState() const;


	/**
	 * @brief 获取角色的一些属性值  比如 攻击 防御 之类的
	 * @param Attribute 属性值的定义
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		float GetCurrentAttributeValue(const FGameplayAttribute& Attribute)const;

	/**
	 * @brief 目标转向
	 * @param OutTargetRotation 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName = GetTurnToTargetRotation)
		void K2_GetTurnToTargetRotation(FRotator& OutTargetRotation)const;

	/**
	 * @brief 角色转向是否完成
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName = IsTurnToComplate)
		bool K2_IsTurnToComplate()const;

	/**
	 * @brief 是否锁定一个单位
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName = IsLockTargetUnit)
		bool K2_IsLockTargetUnit() const;

	/**
	 * @brief 获取锁定的单位，如果没有锁定 返回 nullptr
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName = GetLockTargetUnit)
		AActor* K2_GetLockTargetUnit()const;

	/**
	 * @brief 获取锁定的点，如果没有锁定 返回 Nullptr
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName = GetLockPoint)
		UDLLockPointComponent* K2_GetLockPoint() const;


	/**
	 * @brief 是否死亡
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName = IsDied)
		bool K2_IsDied() const;
};
