#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "Interface/IDLCharacterController.h"
#include "DLAIControllerBase.generated.h"


class UDLCharacterAsset;
class ADLCharacterBase;
class ADLPlayerStateBase;
class UDLGameplayAbilityBase;
USTRUCT(BlueprintType)
struct FDLGeneralHandle
{
	GENERATED_BODY()
public:

	uint64 MagicNumber = 0;

	bool operator==(const FDLGeneralHandle& Other) const
	{
		return Other.MagicNumber == MagicNumber;
	}

	FDLGeneralHandle()
	{
		static uint64 Number = 0;
		MagicNumber = Number++;
	}

	FORCEINLINE friend uint32 GetTypeHash(const FDLGeneralHandle& Value)
	{
		return GetTypeHash(Value.MagicNumber);
	}
};

USTRUCT(BlueprintType)
struct FDLAIActiveAbilityArg
{
	GENERATED_BODY()
public:

};


DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnUnitTagsChange, FGameplayTag, ChangeTag, int32, CurrentCount);


UCLASS()
class DLGAMEPLAYCORE_API ADLAIControllerBase
	: public AAIController
	, public IDLCharacterController
{
	GENERATED_BODY()
public:

	ADLAIControllerBase();

	/**
	 * @brief 监听一个单位的Tag变化
	 * @param TargetUnit 目标单位
	 * @param ListenTag 监听的Tag
	 * @param Listener 监听者
	 * @return 句柄，这个句柄可以用来取消监听
	 */
	UFUNCTION(BlueprintCallable)
		FDLGeneralHandle ListenUnitTagsChange(AActor* TargetUnit, FGameplayTag ListenTag, const FOnUnitTagsChange& Listener);


	/**
	 * @brief 取消监听单位的Tag变化
	 * @param TargetUnit 
	 * @param Handle 
	 */
	UFUNCTION(BlueprintCallable)
		void RemoveUnitTagsChangeListener(AActor* TargetUnit, const FDLGeneralHandle& Handle);

	/**
	 * @brief 查询技能是否可以施放
	 * @param TargetUnit 
	 * @param Handle 
	 */
	UFUNCTION(BlueprintCallable)
		bool AI_CanActivateAbility(AActor* TargetUnit, const TSubclassOf<UDLGameplayAbilityBase> AbilityClass);

	/**
	 * @brief 释放一个技能  
	 * @param AbilityClass 技能的 Class 
	 * @param Arg 技能参数
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
		bool TryActiveAbility(TSubclassOf<UDLGameplayAbilityBase> AbilityClass, FDLAIActiveAbilityArg Arg);

	
	/**
	 * @brief 通过名字调用计数器
	 * @param CounterName 计数器名字
	 * @param Count 计数器添加次数
	 */
	UFUNCTION(BlueprintCallable)
		void CallCounterByName(FName CounterName,int32 Count);

	/**
	 * @brief 锁定目标
	 * @param Unit 被多顶的目标对象
	 */
	UFUNCTION(BlueprintCallable)
		void LockUnit(AActor* Unit);

	/**
	 * @brief 解锁目标
	 */
	UFUNCTION(BlueprintCallable)
		void UnlockUnit();

	
	/**
	 * @brief 设置攻击目标，进入战斗
	 * @param TargetActor 
	 */
	UFUNCTION(BlueprintCallable)
		void ServerSetAttackTarget(AActor* TargetActor);

	/**
	 * @brief 取消攻击目标，退出战斗
	 * @param TargetActor 
	 */
	UFUNCTION(BlueprintCallable)
		void ServerCancelAttackTarget(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
		void ServerApplyGameplayEffect(AActor* TargetActor,TSubclassOf<UGameplayEffect> GameplayEffectClass, int32 Level = 1);
	
public:

	ADLCharacterBase* GetCharacterBase() const;

	void OnCharacterDied();

	void ServerSetupCharacter(const UDLCharacterAsset* Asset, ADLCharacterBase* InCharacter, ADLPlayerStateBase* InPlayerState);

	FName GetNpcSpawnPointId() const
	{
		return NPCSpawnPointId;
	}

	void SetNpcSpawnPointId(FName Id)
	{
		NPCSpawnPointId = Id;
	}
	
protected:

	virtual void TryTurnToRotation(FRotator Rotator, bool ImmediatelyTurn, float OverrideYawSpeed = 0.f) override;

public:

	virtual void OnPossess(APawn* InPawn) override;

	virtual void InitPlayerState() override;
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn) override;


protected:

	UPROPERTY(EditDefaultsOnly, Category = DLAI)
		TSubclassOf<ADLPlayerStateBase> PlayerStateClass;

	UPROPERTY(EditDefaultsOnly, Category = DLAI);
		FName NPCSpawnPointId;

	UPROPERTY(EditAnywhere)
	bool EnableFaceWithTime = false;

protected:

	TMap<FDLGeneralHandle, FDelegateHandle> UnitTagsChangeHandleMap;
};
