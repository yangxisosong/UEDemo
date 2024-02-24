// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLGameplayPredictTask.h"
#include "DLLoopholesTask.generated.h"


USTRUCT(BlueprintType)
struct FDLLoopholesTaskContext : public FDLGameplayPredictTaskContextBase
{
	GENERATED_BODY()

public:

	PREDICT_TASK_ID_DEF(FDLLoopholesTaskContext)

public:

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override
	{
		bool Out = false;

		FDLGameplayPredictTaskContextBase::NetSerialize(Ar, Map, Out);
		bOutSuccess &= Out;


		return true;
	}


	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("FDLLoopholesTaskContext"));
	}
};


/**
 * @brief 角色出 大破绽 的任务，每个角色 仅仅有一个 破绽的任务，实现上，如果当前有破绽任务在运行，那么就会直接阻止其他破绽的任务运行
 */
UCLASS()
class DLGAMEPLAYCORE_API UDLLoopholesTask
		: public UDLGameplayPredictTask
{
	GENERATED_BODY()
public:

	virtual bool CanCancelTaskByOther_Implementation(UDLGameplayPredictTask* Other) const override;

	virtual bool CanBlockOtherTask_Implementation(UDLGameplayPredictTask* Other) override;

	virtual FDLGameplayPredictTaskContextBase* GetContext() override;

	virtual bool OnInitTask(TSharedPtr<FPredictTaskActorInfo> ActorInfo, const FDLGameplayPredictTaskContextBase* Context) override;

	virtual FString ToString() const override;

	virtual bool HasAuthority() const override;


protected:

	UPROPERTY(BlueprintReadOnly, Transient)
		FDLLoopholesTaskContext Context;

	UPROPERTY(BlueprintReadOnly, Transient)
		FPredictTaskActorInfo ActorInfo;

	UPROPERTY(BlueprintReadOnly, Transient)
		UAnimInstance* AnimInstance;
};
