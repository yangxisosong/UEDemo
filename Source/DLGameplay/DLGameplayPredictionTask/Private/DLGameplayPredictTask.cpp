#include "DLGameplayPredictTask.h"
#include "DLGameplayPredictTaskLog.h"
#include "DLGameplayPredictTaskComponent.h"

bool UDLGameplayPredictTask::InitTask(UDLGameplayPredictTaskComponent* Comp,
	const TSharedPtr<FPredictTaskActorInfo> ActorInfo, const FGameplayPredictTaskId& InTaskId,
	const FDLGameplayPredictTaskContextBase* Context)
{
	UE_LOG(LogDLGameplayPredictTask, Log, TEXT("%s [%s] InitTask:%s"), *Comp->GetOwner()->GetName(), *Comp->NetRoleToString(), *this->GetName());
	TaskId = InTaskId;
	PredictTaskComponent = Comp;

	if (OnInitTask(ActorInfo, Context))
	{
		PredictTaskComponent->OnInitTask(this);
		return true;
	}

	return false;
}

void UDLGameplayPredictTask::ActiveTask()
{
	UE_LOG(LogDLGameplayPredictTask, Log, TEXT("%s [%s] ActiveTask Context-> %s")
		, *PredictTaskComponent->GetOwner()->GetName(), *PredictTaskComponent->NetRoleToString(), GetContext() ? *GetContext()->ToString() : TEXT("None"));

	bIsActive = true;
	PredictTaskComponent->OnActiveTask(this);
	this->OnActiveTask();
}

bool UDLGameplayPredictTask::OnInitTask(TSharedPtr<FPredictTaskActorInfo> ActorInfo,
										const FDLGameplayPredictTaskContextBase* Context)
{
	return true;
}

bool UDLGameplayPredictTask::HasAuthority() const
{
	return false;
}

void UDLGameplayPredictTask::EndTask(const bool Cancel)
{
	UE_LOG(LogDLGameplayPredictTask, Log, TEXT("%s [%s] EndTask:%s")
	, *PredictTaskComponent->GetOwner()->GetName(), *PredictTaskComponent->NetRoleToString(), *this->GetName());

	this->OnEndTask(Cancel);
	PredictTaskComponent->OnEndTask(this, Cancel);
	bIsActive = false;
}

FString UDLGameplayPredictTask::ToString() const
{
	return this->GetName();
}

bool UDLGameplayPredictTask::IsActive() const
{
	return bIsActive;
}

FDLGameplayPredictTaskContextBase* UDLGameplayPredictTask::GetContext()
{
	return nullptr;
}

UWorld* UDLGameplayPredictTask::GetWorld() const
{
	if (PredictTaskComponent)
	{
		return PredictTaskComponent->GetWorld();
	}
	return nullptr;
}

FGameplayPredictTaskId UDLGameplayPredictTask::GetInstanceId() const
{
	return TaskId;
}

bool UDLGameplayPredictTask::CanBlockOtherTask_Implementation(UDLGameplayPredictTask* Other)
{
	return false;
}

bool UDLGameplayPredictTask::CanCancelTaskByOther_Implementation(UDLGameplayPredictTask* Other) const
{
	return false;
}

void UDLGameplayPredictTask::OnBlockOtherTask(UDLGameplayPredictTask* Other)
{
}
