#pragma once

#include "CoreMinimal.h"
#include "DLAbilityTargetSelector.h"
#include "GameplayTask.h"
#include "UObject/Object.h"
#include "GameplayTask_FindTargetUnit.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFindTargetUnit, const FHitResult&, HitResult);

UCLASS()
class UGameplayTask_FindTargetUnit
	: public UGameplayTask
{
	GENERATED_BODY()
public:

	/**
	 * @brief 在一段时间内 查找多个目标单位
	 * @param TaskOwner
	 * @param TaskName
	 * @param TargetSelector 查找目标的 Selector
	 * @param FindTimeDuration 查找的持续时间，如果时间为 0 则只查询一次
	 * @param FindTimeInterval 每次查找的时间间隔 （性能警告，间隔不要太小）
	 * @return
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTask", meta = (AdvancedDisplay = "TaskName", BlueprintInternalUseOnly = "TRUE"))
		static UGameplayTask_FindTargetUnit* FindTargetUnits(
					const TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
					const FName TaskName,
					UDLGameplayTargetSelectorShapeTraceBase* TargetSelector,
					const float FindTimeDuration = 0.f,
					const float FindTimeInterval = 0.5f)
	{
		if (TaskOwner.GetInterface())
		{
			UGameplayTask_FindTargetUnit* MyTask = NewTask<UGameplayTask_FindTargetUnit>(TaskOwner, TaskName);
			if (MyTask)
			{
				MyTask->TargetSelector = TargetSelector;
				MyTask->FindTimeDuration = FindTimeDuration;
				MyTask->FindTimeInterval = FindTimeInterval;

				return MyTask;
			}
		}

		return nullptr;
	}

	/**
	* @brief 在一段时间内 查找一个目标单位
	* @param TaskOwner
	* @param TaskName
	* @param TargetSelector 查找目标的 Selector
	* @param FindTimeDuration 查找的持续时间，如果时间为 0 则只查询一次
	* @param FindTimeInterval 每次查找的时间间隔 （性能警告，间隔不要太小）
	* @return
	*/
	UFUNCTION(BlueprintCallable, Category = "GameplayTask", meta = (AdvancedDisplay = "TaskName", BlueprintInternalUseOnly = "TRUE"))
		static UGameplayTask_FindTargetUnit* FindOneTargetUnit(
			const TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
			const FName TaskName,
			UDLGameplayTargetSelectorShapeTraceBase* TargetSelector,
			const float FindTimeDuration = 0.f,
			const float FindTimeInterval = 0.5f)
	{
		if (TaskOwner.GetInterface())
		{
			UGameplayTask_FindTargetUnit* MyTask = NewTask<UGameplayTask_FindTargetUnit>(TaskOwner, TaskName);
			if (MyTask)
			{
				MyTask->TargetSelector = TargetSelector;
				MyTask->FindTimeDuration = FindTimeDuration;
				MyTask->FindTimeInterval = FindTimeInterval;
				MyTask->bIsFindOne = true;
				return MyTask;
			}
		}

		return nullptr;
	}
	


	/**
	 * @brief 找到目标
	 */
	UPROPERTY(BlueprintAssignable)
		FOnFindTargetUnit OnFindTargetUnit;


protected:

	virtual void Activate() override
	{
		if (!ensureAlwaysMsgf(TargetSelector, TEXT("TaskFindTargetWithDuration  忘记设置 目标选择器了")))
		{
			this->EndTask();
			return;
		}

		if (!ensureAlwaysMsgf(GetTaskOwner(), TEXT("TaskFindTargetWithDuration GetTaskOwner() 是空的？")))
		{
			this->EndTask();
			return;
		}

		if (!FMath::IsNearlyZero(FindTimeDuration))
		{
			GetWorld()->GetTimerManager().SetTimer(
				TimerDestroyHandle,
				FTimerDelegate::CreateUObject(this, &UGameplayTask_FindTargetUnit::OnTimerOutDestroy),
				FindTimeDuration,
				false);

			GetWorld()->GetTimerManager().SetTimer(
			TimerIntervalHandle,
			FTimerDelegate::CreateUObject(this, &UGameplayTask_FindTargetUnit::FindTarget),
			FindTimeInterval,
			false);
		}
		else
		{
			this->FindTarget();

			if (TaskState != EGameplayTaskState::Finished)
			{
				this->EndTask();	
			}
		}
	}

	virtual void OnDestroy(bool bInOwnerFinished) override
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		}
	}

private:

	void OnTimerOutDestroy()
	{
		this->EndTask();
	}

	void FindTarget()
	{
		bool Succeed = false;
		TArray<FHitResult> HitResults;
		TargetSelector->CalculateTargetUnits(GetAvatarActor(), TEXT("TaskFindTargetWithDuration"), Succeed, HitResults, {});

		if (Succeed)
		{
			for (const auto& Data : HitResults)
			{
				if (Data.GetActor())
				{
					this->OnFindTargetUnit.Broadcast(Data);

					if (bIsFindOne)
					{
						this->EndTask();
						break;
					}
				}
			}
		}
	}

private:

	UPROPERTY()
		FTimerHandle TimerIntervalHandle;

	UPROPERTY()
		FTimerHandle TimerDestroyHandle;

	UPROPERTY()
		float FindTimeDuration = 0.f;

	UPROPERTY()
		float FindTimeInterval = 0.f;

	UPROPERTY()
		bool bIsFindOne = false;

	UPROPERTY()
		UDLGameplayTargetSelectorShapeTraceBase* TargetSelector;
};
