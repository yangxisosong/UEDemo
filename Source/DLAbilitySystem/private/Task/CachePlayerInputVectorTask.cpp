// Fill out your copyright notice in the Description page of Project Settings.


#include "CachePlayerInputVectorTask.h"

#include "Interface/IDLPlayerController.h"

UCachePlayerInputVectorTask* UCachePlayerInputVectorTask::CachePlayerInputVector(UGameplayAbility* OwningAbility,
                                                                                 FName TaskInstanceName, bool InJustCacheNonzeroVec)
{
	UCachePlayerInputVectorTask* Task = NewAbilityTask<UCachePlayerInputVectorTask>(OwningAbility, TaskInstanceName);
	Task->JustCacheNonzeroVec = InJustCacheNonzeroVec;
	Task->bTickingTask = true;
	return Task;
}

FVector UCachePlayerInputVectorTask::GetCacheValue()
{
	return CacheValue;
}

void UCachePlayerInputVectorTask::Activate()
{
	Super::Activate();

	if ((!Ability) || !Ability->GetActorInfo().IsLocallyControlledPlayer())
	{
		this->EndTask();
	}
}

void UCachePlayerInputVectorTask::TickTask(float DeltaTime)
{
	if (IsValid(Ability))
	{
		if (const IDLPlayerControllerInterface* Interface = Cast<IDLPlayerControllerInterface>(Ability->GetActorInfo().PlayerController))
		{
			auto&& Vec = Interface->GetInputVector();

			if (JustCacheNonzeroVec)
			{
				if (!Vec.IsNearlyZero())
				{
					CacheValue = Vec;
					this->OnCacheInputVec.Broadcast(CacheValue);
				}
			}
			else
			{
				CacheValue = Vec;
				this->OnCacheInputVec.Broadcast(CacheValue);
			}
	
		}
	}
}
