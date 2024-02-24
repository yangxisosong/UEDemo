// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "UObject/Object.h"
#include "WaitAbilityInput.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDLAbilityWaitInputDelegate, float, DurationTime);

/**
 *
 */
UCLASS()
class DLABILITYSYSTEM_API UWaitAbilityInputTask
	: public UAbilityTask
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FDLAbilityWaitInputDelegate	OnTrigger;

	UPROPERTY(BlueprintAssignable)
	FDLAbilityWaitInputDelegate	OnTimeOut;

	/**
	 * @brief 等待输入
	 * @param OwningAbility
	 * @param InTargetInputId 
	 * @param InInputEvent 
	 * @param InTimeOut 等待超时时间  秒  0 表示永远不超时
	 * @param bTestAlreadyEvent 在激活时是否先进行测试，判断当前状态是否满足
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UWaitAbilityInputTask* WaitAbilityInput(UGameplayAbility* OwningAbility, int32 InTargetInputId, EInputEvent InInputEvent = EInputEvent::IE_Pressed, float InTimeOut = 0.f, bool bTestAlreadyEvent = false);


protected:

	virtual void Activate() override;

private:

	void OnTriggerCallback();
	void OnTimeout();
private:

	float StartTime = 0.f;
	bool bTestInitialState = false;
	float TimeOut = 0.f;
	FDelegateHandle DelegateHandle;
	FDelegateHandle InputChangeDelegateHandle;
	EInputEvent InputEvent = EInputEvent::IE_Pressed;
	int32 TargetInputId = 0;
	FTimerHandle TimeOutHandle;
};
