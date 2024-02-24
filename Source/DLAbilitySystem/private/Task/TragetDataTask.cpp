// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeConst

#include "TragetDataTask.h"

#include "AbilitySystemComponent.h"


UServerWaitTargetDataTask* UServerWaitTargetDataTask::ServerWaitTargetData(UGameplayAbility* OwningAbility,
																		   FName TaskInstanceName, bool TriggerOnce)
{
	UServerWaitTargetDataTask* MyObj = NewAbilityTask<UServerWaitTargetDataTask>(OwningAbility, TaskInstanceName);
	MyObj->bTriggerOnce = TriggerOnce;
	return MyObj;
}

void UServerWaitTargetDataTask::Activate()
{
	Super::Activate();

	// 客户端不做任何处理
	if(IsLocallyControlled())
	{
		return;
	}

	const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
	const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

	UE_LOG(LogTemp, Log, TEXT("UServerWaitTargetDataTask::Activate  ActivationPredictionKey: %s SpecHandle: %s"), *ActivationPredictionKey.ToString(), *SpecHandle.ToString());

	AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)
		.AddUObject(this, &UServerWaitTargetDataTask::OnTargetDataReplicatedCallback);
}

void UServerWaitTargetDataTask::OnDestroy(bool bAbilityEnded)
{
	if (AbilitySystemComponent)
	{
		const FGameplayAbilitySpecHandle	SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).RemoveAll(this);
	}

	Super::OnDestroy(bAbilityEnded);
}

void UServerWaitTargetDataTask::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data,
	FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(Data, ActivationTag);
	}

	if (bTriggerOnce)
	{
		EndTask();
	}
}
