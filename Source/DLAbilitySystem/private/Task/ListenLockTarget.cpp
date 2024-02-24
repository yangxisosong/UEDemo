// Fill out your copyright notice in the Description page of Project Settings.


#include "ListenLockTarget.h"

#include "Interface/ICharacterStateAccessor.h"


UListenLockTarget* UListenLockTarget::ListenLockTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UListenLockTarget* Task = NewAbilityTask<UListenLockTarget>(OwningAbility, TaskInstanceName);
	Task->bTickingTask = true;
	return Task;
}


void UListenLockTarget::TickTask(float DeltaTime)
{
	if (ICharacterStateAccessor::Execute_K2_IsLockTargetUnit(GetAvatarActor()))
	{
		OnLockTarget.Broadcast(ICharacterStateAccessor::Execute_K2_GetLockPoint(GetAvatarActor()));
		this->EndTask();
	}
}
