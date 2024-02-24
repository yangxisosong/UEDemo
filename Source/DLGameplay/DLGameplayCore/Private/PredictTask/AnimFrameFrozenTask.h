#pragma once

#include "CoreMinimal.h"
#include "DLGameplayPredictTask.h"
#include "DLGameplayPredictTaskTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayCoreLog.h"
#include "AnimFrameFrozenTask.generated.h"


USTRUCT(BlueprintType)
struct FDLAnimFrameFrozenTaskContext : public FDLGameplayPredictTaskContextBase
{
	GENERATED_BODY()

public:

	PREDICT_TASK_ID_DEF(FDLAnimFrameFrozenTaskContext)

public:

	// 定帧的时间
	UPROPERTY(BlueprintReadWrite)
		float FrozenDuration = 0.f;

	// 定帧的部位
	UPROPERTY(BlueprintReadWrite)
		FGameplayTagContainer Parts;

	//
	UPROPERTY(BlueprintReadWrite)
		FGameplayTag FrameFrozenType;


	UPROPERTY(BlueprintReadWrite)
		UCurveFloat* AnimSpeedCurve;


	// 被打时的 Tags 信息，只有被打触发的才有效
	UPROPERTY(BlueprintReadWrite)
		FGameplayTagContainer UnderAttackTags;

public:

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override
	{
		bool Out = false;

		FDLGameplayPredictTaskContextBase::NetSerialize(Ar, Map, Out);
		bOutSuccess &= Out;

		Ar << AnimSpeedCurve;
		Ar << FrozenDuration;

		FrameFrozenType.NetSerialize(Ar, Map, Out);
		bOutSuccess &= Out;

		Parts.NetSerialize(Ar, Map, Out);
		bOutSuccess &= Out;

		UnderAttackTags.NetSerialize(Ar, Map, Out);
		bOutSuccess &= Out;

		return true;
	}


	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("%s FrozenDuration:%2f, FrameFrozenType:%s, UnderAttackTags:%s, Parts:%s"),
				*FDLGameplayPredictTaskContextBase::ToString(),
				FrozenDuration,
				*FrameFrozenType.ToString(),
				*UnderAttackTags.ToString(),
				*Parts.ToString());
	}
};

/**
* @brief 动画定帧的任务
*/
UCLASS(Abstract)
class UAnimFrameFrozenTask : public UDLGameplayPredictTask
{
	GENERATED_BODY()

public:

	virtual bool OnInitTask(TSharedPtr<FPredictTaskActorInfo> InActorInfo, const FDLGameplayPredictTaskContextBase* InContext) override
	{
		ActorInfo = *InActorInfo;

		if (const auto* UnderAttackData = CastPredictTaskContext<FDLAnimFrameFrozenTaskContext>(InContext))
		{
			Context = *UnderAttackData;
			AnimInstance = ActorInfo.SkeletalMeshComp->GetAnimInstance();

			ensureAlwaysMsgf(AnimInstance, TEXT("动画实例不应该是空的"));

			if (!AnimInstance)
			{
				return false;
			}

			return true;
		}

		return false;
	}

	virtual bool HasAuthority() const override
	{
		if (IsValid(ActorInfo.AvatarActor))
		{
			return ActorInfo.AvatarActor->GetLocalRole() == ENetRole::ROLE_Authority;
		}

		return false;
	}

	virtual void Tick(const float Dt) override
	{
		TickTask(Dt);
	}

	virtual FDLGameplayPredictTaskContextBase* GetContext() override
	{
		return &Context;
	}


	UFUNCTION(BlueprintImplementableEvent)
		void TickTask(float Dt);

public:

	UPROPERTY(BlueprintReadWrite, Transient)
		FGameplayTag ActiveAttackPower;

	UPROPERTY(BlueprintReadOnly, Transient)
		UAnimInstance* AnimInstance;

	UPROPERTY(BlueprintReadOnly, Transient)
		FDLAnimFrameFrozenTaskContext Context;

	UPROPERTY(BlueprintReadOnly, Transient)
		FPredictTaskActorInfo ActorInfo;
};

