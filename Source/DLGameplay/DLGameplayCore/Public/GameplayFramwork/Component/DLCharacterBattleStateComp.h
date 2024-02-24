// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "NativeGameplayTags.h"
#include "DLCharacterBattleStateComp.generated.h"

namespace EDLGameplayTag
{
	DLGAMEPLAYCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Unit_State_Battle);
}

namespace EDLGameplayMsg
{
	// NPC 进入战斗状态
	DLGAMEPLAYCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_InBattleState);

	// NPC 离开战斗状态
	DLGAMEPLAYCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_OutBattleState);
}


USTRUCT(BlueprintType)
struct FDLGameplayBattleStateMsgBody
{
	GENERATED_BODY()
public:

	UPROPERTY()
		APlayerState* TargetPS = nullptr;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterBattleStateChange, bool, CurrentState);

UCLASS()
class DLGAMEPLAYCORE_API UDLCharacterBattleStateComp : public UActorComponent
{
	GENERATED_BODY()

public:

	UDLCharacterBattleStateComp();


	/**
	 * @brief 把 TargetActor 视为 攻击对象
	 * @param TargetActor
	 */
	void ServerSetAttackTarget(AActor* TargetActor);


	/**
	 * @brief 取消 TargetActor 为 攻击对象
	 * @param TargetActor
	 */
	void ServerCancelAttackTarget(AActor* TargetActor);


	/**
	 * @brief 给系统注入 释放技能的事件
	 */
	void ServerInjectAbilityActiveEvent(const FGameplayTagContainer& AbilityTags);


	/**
	 * @brief 给系统注入被打的事件
	 */
	void ServerInjectBeAttackEvent();


	// 角色的战斗转态改变
	UPROPERTY(BlueprintAssignable)
	FOnCharacterBattleStateChange OnCharacterBattleStateChange;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeAsEnemyUnit, AActor* Target);
	FOnChangeAsEnemyUnit OnChangeAsEnemyUnit;

	AActor* GetAsEnemyActor() const;

private:

	AActor* GetEnemyActor();

	UDLCharacterBattleStateComp* FindBscEnsure(AActor* Actor) const;

	void ServerBeAttackTarget(UDLCharacterBattleStateComp* Bsc);

	void ServerRemoveAttackSelf(UDLCharacterBattleStateComp* Bsc);

	void CheckBattleState();

	void SetBattleState(const bool InBattle);

	void BroadcastBattleState();

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	UFUNCTION()
		void OnRep_InBattleState();

	UFUNCTION()
		void OnRep_AsEnemyUnit();
private:

	// 把自己视为攻击目标的 BSC
	TArray<TWeakObjectPtr<UDLCharacterBattleStateComp>> AsEnemyBscArray;


	UPROPERTY(ReplicatedUsing = OnRep_InBattleState)
		bool bInBattleState = false;


	UPROPERTY(ReplicatedUsing = OnRep_AsEnemyUnit)
		AActor* AsEnemyUnit;

private:

	//-------------躺平状态-----------------------//

	// 上次被打的时间
	float LastBeAttackedTime = 0;

	// 上次攻击的时间
	float LastUsedAbilityTime = 0;

	// 配置 进入躺平状态的 最小时间
	float MinLeaveLieStateDuration = 5;

	// 是否在躺平状态
	bool bInLieState = true;

private:

	// 这个组件当前的活跃时间
	float TimeTotal = 0.f;
};
