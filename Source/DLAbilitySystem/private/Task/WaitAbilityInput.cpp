// Fill out your copyright notice in the Description page of 
#include "WaitAbilityInput.h"
#include "ASC/DLPlayerAbilitySysComponent.h"


UWaitAbilityInputTask* UWaitAbilityInputTask::WaitAbilityInput(UGameplayAbility* OwningAbility, const int32 InTargetInputId,
                                                               const EInputEvent InInputEvent, const float InTimeOut, const bool bTestAlreadyEvent)
{
	UWaitAbilityInputTask* Task = NewAbilityTask<UWaitAbilityInputTask>(OwningAbility);
	Task->bTestInitialState = bTestAlreadyEvent;
	Task->TimeOut = InTimeOut;
	Task->InputEvent = InInputEvent;
	Task->TargetInputId = InTargetInputId;
	return Task;
}

void UWaitAbilityInputTask::Activate()
{
	StartTime = GetWorld()->GetTimeSeconds();

	UDLPlayerAbilitySysComponent* MyAbilitySystemComponent = Cast<UDLPlayerAbilitySysComponent>(AbilitySystemComponent);

	if (!Ability || !MyAbilitySystemComponent)
	{
		return;
	}
	if (bTestInitialState && IsLocallyControlled())
	{
		if (MyAbilitySystemComponent->QueryAbilityInputState(TargetInputId) == InputEvent)
		{
			this->OnTriggerCallback();
			return;
		}
	}


	if (TimeOut > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimeOutHandle,
			FTimerDelegate::CreateUObject(this, &UWaitAbilityInputTask::OnTimeout),
			TimeOut,
			false
		);

	}

	if (IsPredictingClient())
	{
		InputChangeDelegateHandle
			= MyAbilitySystemComponent->OnAbilityInputChange().AddWeakLambda(this, [MyAbilitySystemComponent, this](EInputEvent InEvent, int32 Id)
			{
				if (InEvent == InputEvent && TargetInputId == Id)
				{
					this->OnTriggerCallback();

					MyAbilitySystemComponent->OnAbilityInputChange().Remove(InputChangeDelegateHandle);
				}
			});
	}

	if (IsForRemoteClient())
	{
		auto& Delegate = AbilitySystemComponent->AbilityReplicatedEventDelegate(
			EAbilityGenericReplicatedEvent::GameCustom1,
			GetAbilitySpecHandle(),
			GetActivationPredictionKey());

		DelegateHandle = Delegate.AddUObject(this, &UWaitAbilityInputTask::OnTriggerCallback);

		if (!AbilitySystemComponent->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()))
		{
			SetWaitingOnRemotePlayerData();
		}
	}

	return;
}

void UWaitAbilityInputTask::OnTriggerCallback()
{
	const float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;

	if (!Ability || !AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);

	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent, IsPredictingClient());

	if (IsPredictingClient())
	{
		AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
	}
	else
	{
		AbilitySystemComponent->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		this->OnTrigger.Broadcast(ElapsedTime);
	}

	GetWorld()->GetTimerManager().ClearTimer(TimeOutHandle);

	EndTask();
}

void UWaitAbilityInputTask::OnTimeout()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		const float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;

		this->OnTimeOut.Broadcast(ElapsedTime);
	}

	GetWorld()->GetTimerManager().ClearTimer(TimeOutHandle);

	EndTask();

}
