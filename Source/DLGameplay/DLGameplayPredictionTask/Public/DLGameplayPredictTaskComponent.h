#pragma once


#include "CoreMinimal.h"
#include "DLGameplayPredictTask.h"
#include "DLGameplayPredictTaskComponent.generated.h"


struct FDLGameplayPredictTaskContextBase;
class UDLGameplayPredictTask;


struct FDLPredictTaskKey
{
public:

	int32 Key = 0;
};


USTRUCT()
struct FDLNetPredictTaskContext
{
	GENERATED_BODY()
public:

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

public:

	UPROPERTY()
		AActor* Target = nullptr;

	FDLGameplayPredictTaskContextTypeId TaskContextTypeId;

	TSharedPtr<FDLGameplayPredictTaskContextBase> ContextData;

	FGameplayPredictTaskId TaskId;
};

template<>
struct TStructOpsTypeTraits<FDLNetPredictTaskContext> : public TStructOpsTypeTraitsBase2<FDLNetPredictTaskContext>
{
	enum
	{
		WithNetSerializer = true
	};
};


DECLARE_DELEGATE_RetVal(TSharedPtr<FPredictTaskActorInfo>, FMakePredictTaskActorInfo);


USTRUCT()
struct FDLGameplayPredictTaskInstancesInfo
{
	GENERATED_BODY()
public:

	UPROPERTY()
		TSubclassOf<UDLGameplayPredictTask> Class;

	UPROPERTY()
		TArray<UDLGameplayPredictTask*> Instances;

public:

	bool operator==(const TSubclassOf<UDLGameplayPredictTask>& InClass) const
	{
		return Class == InClass;
	}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnActivePredictTask, UDLGameplayPredictTask*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEndPredictTask, UDLGameplayPredictTask*, bool);


UCLASS()
class DLGAMEPLAYPREDICTIONTASK_API UDLGameplayPredictTaskComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UDLGameplayPredictTaskComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Setup(APlayerController* InPlayerController);

	bool ProxyActiveTask(TSharedPtr<FDLGameplayPredictTaskContextBase> Context, FGameplayPredictTaskId TaskId);

	void AuthorityActiveTask(TSharedPtr<FDLGameplayPredictTaskContextBase> Context, FGameplayPredictTaskId TaskId);

	void UpdateTags(FGameplayTagContainer Tags);

	UFUNCTION(NetMulticast, Reliable)
		void MultiCastActiveTask(FDLNetPredictTaskContext ContextData, FGameplayPredictTaskId TaskId);


	bool HasAuthority() const;

	FString NetRoleToString() const;

	void TryCancelOtherTask(UDLGameplayPredictTask* Task, bool& OutHasBlock);

	FMakePredictTaskActorInfo OnMakePredictTaskActorInfo;

	void OnEndTask(UDLGameplayPredictTask* Task, bool Cancel);
	void OnActiveTask(UDLGameplayPredictTask* Task);
	void OnInitTask(UDLGameplayPredictTask* Task);

	UDLGameplayPredictTask* AllocatePredictTask(TSubclassOf<UDLGameplayPredictTask> Class);

	FOnActivePredictTask OnActivePredictTask;
	FOnEndPredictTask OnEndPredictTask;

protected:

	// 正在执行的任务, 不是预测的，是真正拿到授权的任务
	UPROPERTY()
		TArray<UDLGameplayPredictTask*> ActiveTasks;

	// 模拟端正在预测的任务
	UPROPERTY()
		TArray<UDLGameplayPredictTask*> ProxyPredictTasks;


	UPROPERTY()
		TArray<UDLGameplayPredictTask*> TickTaskArray;

	UPROPERTY()
		TArray<FGameplayPredictTaskId> ProxyCompletedTask;

	UPROPERTY()
		TArray<FDLGameplayPredictTaskInstancesInfo> PredictTaskPool;

	UPROPERTY()
		APlayerController* PlayerController;
};