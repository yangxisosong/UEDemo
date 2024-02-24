// Fill out your copyright notice in the Description page of Project Settings.


#include "DLLoopholesTask.h"

#include "DLTypeCast.h"

bool UDLLoopholesTask::CanCancelTaskByOther_Implementation(UDLGameplayPredictTask* Other) const
{
	return false;
}

bool UDLLoopholesTask::CanBlockOtherTask_Implementation(UDLGameplayPredictTask* Other)
{
	// 如果是自己，就直接 Block，目前这个任务 每个角色只允许一个
	if (Cast<UDLLoopholesTask>(Other))
	{
		return true;
	}

	return Super::CanBlockOtherTask_Implementation(Other);
}

FDLGameplayPredictTaskContextBase* UDLLoopholesTask::GetContext()
{
	return &Context;
}

bool UDLLoopholesTask::OnInitTask(TSharedPtr<FPredictTaskActorInfo> InActorInfo,
								const FDLGameplayPredictTaskContextBase* InContext)
{
	if (!Super::OnInitTask(InActorInfo, InContext))
	{
		return false;
	}

	ActorInfo = *InActorInfo;

	if (const auto MyContext = CastPredictTaskContext<FDLLoopholesTaskContext>(InContext))
	{
		Context = *MyContext;

		if (!ActorInfo.SkeletalMeshComp)
		{
			return false;
		}

		AnimInstance = ActorInfo.SkeletalMeshComp->GetAnimInstance();
		if (!AnimInstance)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

FString UDLLoopholesTask::ToString() const
{
	return Super::ToString();
}

bool UDLLoopholesTask::HasAuthority() const
{
	return ActorInfo.AvatarActor->GetLocalRole() == ENetRole::ROLE_Authority;
}
