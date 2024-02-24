#include "DLGameplayPredictTaskComponent.h"
#include "DLGameplayPredictTaskLog.h"


bool FDLNetPredictTaskContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Target;

	Ar << TaskContextTypeId;

	Ar << TaskId.Id;

	if (Ar.IsLoading())
	{
		ContextData = MakeShareable(CreatePredictTaskContext(TaskContextTypeId));
		ensureAlwaysMsgf(ContextData, TEXT("没创建出来，序列化失败，TaskContextTypeId %d "), TaskContextTypeId);
		if (!ContextData)
		{
			bOutSuccess = false;
			return true;
		}
	}

	if (ContextData)
	{
		return ContextData->NetSerialize(Ar, Map, bOutSuccess);
	}

	bOutSuccess = true;
	return true;
}

UDLGameplayPredictTaskComponent::UDLGameplayPredictTaskComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDLGameplayPredictTaskComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
													FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TickTaskArray.Num() > 0)
	{
		auto CopyTickArray = TickTaskArray;
		for (const auto& TickTask : CopyTickArray)
		{
			TickTask->Tick(DeltaTime);
		}
	}

}

void UDLGameplayPredictTaskComponent::Setup(APlayerController* InPlayerController)
{
	PlayerController = InPlayerController;
}

bool UDLGameplayPredictTaskComponent::ProxyActiveTask(const TSharedPtr<FDLGameplayPredictTaskContextBase> Context,
													  const FGameplayPredictTaskId TaskId)
{
	if (!Context)
	{
		return false;
	}

	if (!IsValidPredictTaskId(TaskId))
	{
		return false;
	}

	if (!Context->TaskClass)
	{
		return false;
	}

	UDLGameplayPredictTask* Task = AllocatePredictTask(Context->TaskClass);
	if (!Task)
	{
		return false;
	}

	if (!ensureAlwaysMsgf(OnMakePredictTaskActorInfo.IsBound(), TEXT("一定是绑定了的")))
	{
		return false;
	}

	if (!Task->InitTask(this, OnMakePredictTaskActorInfo.Execute(), TaskId, Context.Get()))
	{
		return false;
	}


	bool HasBlock = false;
	this->TryCancelOtherTask(Task, HasBlock);
	if (!HasBlock)
	{
		Task->ActiveTask();
		this->ProxyPredictTasks.Add(Task);
	}

	return true;
}

void UDLGameplayPredictTaskComponent::AuthorityActiveTask(const TSharedPtr<FDLGameplayPredictTaskContextBase> Context, FGameplayPredictTaskId TaskId)
{
	FDLNetPredictTaskContext NetContext;
	NetContext.ContextData = Context;
	NetContext.TaskContextTypeId = Context->GetTypeId();

	this->MultiCastActiveTask(NetContext, TaskId);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UDLGameplayPredictTaskComponent::UpdateTags(const FGameplayTagContainer Tags)
{
	TArray<UDLGameplayPredictTask*> Tasks;
	Tasks.Append(ProxyPredictTasks);
	Tasks.Append(ActiveTasks);

	for (const auto& ActiveTask : Tasks)
	{
		if (Tags.HasAny(ActiveTask->CancelByTags))
		{
			ActiveTask->EndTask(true);
		}
	}
}

bool UDLGameplayPredictTaskComponent::HasAuthority() const
{
	return GetOwner()->HasAuthority();
}

FString UDLGameplayPredictTaskComponent::NetRoleToString() const
{
	return StaticEnum<ENetRole>()->GetNameStringByValue((int32)GetOwner()->GetLocalRole());
}

void UDLGameplayPredictTaskComponent::TryCancelOtherTask(UDLGameplayPredictTask* Task, bool& OutHasBlock)
{
	TArray<UDLGameplayPredictTask*> CancelTasks;

	for (const auto& ActiveTask : ActiveTasks)
	{
		// 检查一个任务能不能被 Task 打断
		const bool bIsCancel = ActiveTask->CanCancelTaskByOther(Task);

		UE_LOG(LogDLGameplayPredictTask, Log, TEXT("<UDLGameplayPredictTaskComponent::TryCancelOtherTask> %s[%s] ActiveTasks 任务%s被%s打断：%d")
			, *GetOwner()->GetName(), *NetRoleToString(), *ActiveTask->GetName(), *Task->GetName(), bIsCancel);

		if (bIsCancel)
		{
			CancelTasks.Add(ActiveTask);
		}
		else
		{
			// 如果无法打断，那么继续考虑 这个任务 会不会阻止 其他任务
			const bool bIsBlock = ActiveTask->CanBlockOtherTask(Task);
			if(bIsBlock)
			{
				ActiveTask->OnBlockOtherTask(Task);
				OutHasBlock = true;
			}
		}
	}

	for (const auto& CancelTask : CancelTasks)
	{
		CancelTask->EndTask(true);
	}

	CancelTasks.Empty();

	for (const auto& ActiveTask : ProxyPredictTasks)
	{
		const bool bIsCancel = ActiveTask->CanCancelTaskByOther(Task);

		UE_LOG(LogDLGameplayPredictTask, Log, TEXT("<UDLGameplayPredictTaskComponent::TryCancelOtherTask> %s[%s] ProxyPredictTasks 任务%s被%s打断：%d")
			, *GetOwner()->GetName(), *NetRoleToString(), *ActiveTask->GetName(), *Task->GetName(), bIsCancel);

		if (bIsCancel)
		{
			CancelTasks.Add(ActiveTask);
		}
		else
		{
			// 如果无法打断，那么继续考虑 这个任务 会不会阻止 其他任务
			const bool bIsBlock = ActiveTask->CanBlockOtherTask(Task);
			if(bIsBlock)
			{
				ActiveTask->OnBlockOtherTask(Task);
				OutHasBlock = true;
			}
		}
	}

	for (const auto& CancelTask : CancelTasks)
	{
		CancelTask->EndTask(true);
	}
}

void UDLGameplayPredictTaskComponent::OnEndTask(UDLGameplayPredictTask* Task, const bool Cancel)
{
	OnEndPredictTask.Broadcast(Task, Cancel);
	ActiveTasks.Remove(Task);
	ProxyPredictTasks.Remove(Task);
	TickTaskArray.Remove(Task);
	if (Task)
	{
		ProxyCompletedTask.AddUnique(Task->GetInstanceId());
	}
}

void UDLGameplayPredictTaskComponent::OnActiveTask(UDLGameplayPredictTask* Task)
{
	OnActivePredictTask.Broadcast(Task);
}

void UDLGameplayPredictTaskComponent::OnInitTask(UDLGameplayPredictTask* Task)
{
	if (Task->bEnabledTick)
	{
		TickTaskArray.Add(Task);
	}
}

UDLGameplayPredictTask* UDLGameplayPredictTaskComponent::AllocatePredictTask(TSubclassOf<UDLGameplayPredictTask> Class)
{
	auto InsInfo = PredictTaskPool.FindByKey(Class);
	if (!InsInfo)
	{
		InsInfo = &PredictTaskPool.AddDefaulted_GetRef();
		InsInfo->Class = Class;
	}

	UDLGameplayPredictTask* Task = nullptr;

	for (const auto TaskIns : InsInfo->Instances)
	{
		if (!TaskIns->IsActive())
		{
			Task = TaskIns;
			continue;
		}
	}

	if (!Task)
	{
		Task = NewObject<UDLGameplayPredictTask>(this, Class);
		InsInfo->Instances.Add(Task);

		UE_LOG(LogDLGameplayPredictTask, Log, TEXT("%s[%s] AllocatePredictTask  Class: %s, Count:%d"), *GetOwner()->GetName(), *NetRoleToString(), *Class->GetName(), InsInfo->Instances.Num());
	}

	return Task;
}

void UDLGameplayPredictTaskComponent::MultiCastActiveTask_Implementation(FDLNetPredictTaskContext ContextData, const FGameplayPredictTaskId TaskId)
{
	if (!ensureAlwaysMsgf(ContextData.ContextData, TEXT("不可能发送空的上下文数据")))
	{
		return;
	}

	if (!ensureAlwaysMsgf(IsValidPredictTaskId(TaskId), TEXT("不能发送无效的 TaskId")))
	{
		return;
	}

	if (!ensureAlwaysMsgf(OnMakePredictTaskActorInfo.IsBound(), TEXT("一定是绑定了的")))
	{
		return;
	}


	UE_LOG(LogDLGameplayPredictTask, Log, TEXT("%s [%s] MultiCastActiveTask %s TaskId %s"), *GetOwner()->GetName(), *NetRoleToString(), *ContextData.ContextData->ToString(), *TaskId.ToString());

	if (HasAuthority())
	{
		const auto& Context = ContextData.ContextData;
		UDLGameplayPredictTask* Task = AllocatePredictTask(Context->TaskClass);
		if (!Task)
		{
			return;
		}

		if (!Task->InitTask(this, OnMakePredictTaskActorInfo.Execute(), TaskId, Context.Get()))
		{
			return;
		}

		bool HasBlock = false;
		this->TryCancelOtherTask(Task, HasBlock);
		if (!HasBlock)
		{
			Task->ActiveTask();
			ActiveTasks.Add(Task);
		}
	}
	else
	{

		//检查代理端是否已经把任务完成了，防止重复创建任务
		const bool IsCompleted = ProxyCompletedTask.Contains(TaskId);
		ProxyCompletedTask.Remove(TaskId);

		if (IsCompleted)
		{
			return;
		}

		bool ExistPredictTask = false;

		if (ProxyPredictTasks.Num() > 0)
		{

			for (const auto& Task : ProxyPredictTasks)
			{
				if (Task->GetInstanceId() == TaskId)
				{
					ActiveTasks.Add(Task);

					// 通知任务收到来之服务端的校验
					Task->OnRecvVerify();

					ExistPredictTask = true;
					break;
				}
			}
		}

		if (!ExistPredictTask)
		{
			const FDLGameplayPredictTaskContextBase* Context = ContextData.ContextData.Get();
			UDLGameplayPredictTask* Task = AllocatePredictTask(Context->TaskClass);
			if (!Task)
			{
				return;
			}

			if (!Task->InitTask(this, OnMakePredictTaskActorInfo.Execute(), TaskId, Context))
			{
				return;
			}


			bool HasBlock = false;
			this->TryCancelOtherTask(Task, HasBlock);
			if (!HasBlock)
			{
				Task->ActiveTask();
				ActiveTasks.Add(Task);
			}
		}
	}
}
