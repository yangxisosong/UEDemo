#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "TragetDataTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnServerWaitTargetDataTask, const FGameplayAbilityTargetDataHandle&, DataHandle, const FGameplayTag&, CustomTag);


UCLASS()
class DLABILITYSYSTEM_API UServerWaitTargetDataTask : public UAbilityTask
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
		FOnServerWaitTargetDataTask	ValidData;

	/**
	 * @brief 服务器等待客户端的 TargetData 数据
	 * @param OwningAbility
	 * @param TaskInstanceName
	 * @param TriggerOnce 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Ability|Tasks")
		static UServerWaitTargetDataTask* ServerWaitTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName,  bool TriggerOnce = false);

protected:

	virtual void Activate() override;
			
	virtual void OnDestroy(bool AbilityEnded) override;

private:

	UFUNCTION()
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);

private:

	bool bTriggerOnce = false;
};

//UCLASS()
//class UClientSendTargetDataTask : public UAbilityTask
//{
//	GENERATED_BODY()
//public:
//
//
//
//};


//
//UCLASS()
//class UClientSendDataAndServerWaitTask : public UAbilityTask
//{
//	GENERATED_BODY()
//public:
//
//	UPROPERTY(BlueprintAssignable)
//		FOnServerWaitTargetDataTask	ValidData;
//
//
//	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Ability|Tasks")
//		static UClientSendDataAndServerWaitTask* ClientSendAndServerWaitTask(UGameplayAbility* OwningAbility, FName TaskInstanceName,  bool TriggerOnce = false);
//
//protected:
//
//	virtual void Activate() override;
//
//	virtual void OnDestroy(bool AbilityEnded) override;
//
//private:
//
//	UFUNCTION()
//		void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);
//
//private:
//
//	bool bTriggerOnce;
//};