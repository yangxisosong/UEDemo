#pragma once


#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "DLGameplayAbilityBase.h"
#include "GameplayTask.h"
#include "ASC/DLUnitAbilitySystemComponent.h"

#include "GameplayTask_ActiveAbility.generated.h"


class UDLGameplayAbilityBase;


USTRUCT(BlueprintType)
struct FDLGameplayTaskActiveAbilityPayload
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
		bool bIsCancel = false;

	UPROPERTY(BlueprintReadWrite)
		AActor* HitUnit = nullptr;

	UPROPERTY(BlueprintReadWrite)
		FActiveGameplayEffectHandle ActiveGEHandle;

	UPROPERTY(BlueprintReadWrite)
		FGameplayEffectSpec EffectSpec;

	UPROPERTY(BlueprintReadWrite)
		UAbilitySystemComponent* TargetASC = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
		EDLAbilityStage OldStage;
	
	UPROPERTY(BlueprintReadWrite)
		EDLAbilityStage NewStage;
	
	UPROPERTY(BlueprintReadWrite)
	UGameplayAbility* GA = nullptr;
};


UCLASS(Blueprintable)
class UGameplayTask_ServerActiveAbility
	: public UGameplayTask
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotify, FDLGameplayTaskActiveAbilityPayload, Payload);

public:
	// 技能执行失败
	UPROPERTY(BlueprintAssignable)
		FOnNotify Failed;

	// 技能执行完成, 可能是被取消
	UPROPERTY(BlueprintAssignable)
		FOnNotify Complate;

	//开始移动
	UPROPERTY(BlueprintAssignable)
		FGenericGameplayTaskDelegate OnStartRotate;

	//结束旋转
	UPROPERTY(BlueprintAssignable)
		FGenericGameplayTaskDelegate OnStopRotate;

	//后摇
	UPROPERTY(BlueprintAssignable)
		FOnNotify OnPostStart;
	
	UPROPERTY(BlueprintAssignable)
		FOnNotify ApplyGEToTarget;


	UPROPERTY(BlueprintAssignable)
		FOnNotify ApplyGEToSelf;


	// 击中一个单位   这个事件 一定早与 ApplyGEToTarget ApplyGEToSelf
	UPROPERTY(BlueprintAssignable)
		FOnNotify HitUnit;

	UFUNCTION(BlueprintImplementableEvent)
	FGameplayTag GetCanRotateTag();

	UFUNCTION(BlueprintCallable, Category = "GameplayTask", meta = (AdvancedDisplay = "TaskOwner", BlueprintInternalUseOnly = "TRUE"))
		static UGameplayTask_ServerActiveAbility* TaskServerActiveAbility(
			const TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
			const FName TaskName,
			const TSubclassOf<UGameplayTask_ServerActiveAbility> ServerActiveAbilityClass,
			const TSubclassOf<UDLGameplayAbilityBase> Ability
			)
	{
		if (TaskOwner.GetInterface())
		{
			if (ServerActiveAbilityClass)
			{
				UGameplayTask_ServerActiveAbility* MyTask = NewObject<UGameplayTask_ServerActiveAbility>(
					(UObject*)GetTransientPackage(), ServerActiveAbilityClass);
				if (MyTask)
				{
					MyTask->InitTask(*TaskOwner,TaskOwner->GetGameplayTaskDefaultPriority());
					MyTask->Ability = Ability;
					return MyTask;
				}
			}
		}

		return nullptr;
	}


	virtual void Activate() override
	{
		if (GetOwnerActor()->GetLocalRole() != ENetRole::ROLE_Authority)
		{
			this->NtfFailed();
			return;
		}

		IAbilitySystemInterface* AbilitySystem = Cast<IAbilitySystemInterface>(GetAvatarActor());
		if (!ensureAlwaysMsgf(AbilitySystem, TEXT("正在监听一个不支持的 单位")))
		{
			this->NtfFailed();
			return;
		}

		ASC = Cast<UDLUnitAbilitySystemComponent>(AbilitySystem->GetAbilitySystemComponent());
		if (!ensureAlwaysMsgf(ASC, TEXT("这个单位技能组件 不应该是 Nullptr")))
		{
			this->NtfFailed();
			return;
		}


		AbilityEndHandle = ASC->OnAbilityEnded.AddWeakLambda(this, [this](const FAbilityEndedData& Data)
			{
				if (Data.AbilityThatEnded->GetClass()->GetDefaultObject() != Ability.GetDefaultObject())
				{
					return;
				}

				FDLGameplayTaskActiveAbilityPayload Payload;
				Payload.bIsCancel = Data.bWasCancelled;

				// ReSharper disable once CppExpressionWithoutSideEffects
				this->Complate.Broadcast(Payload);
				this->EndTask();

			});

		AbilityStageChangeHandle = ASC->OnStageChange.AddWeakLambda(this, [this](UGameplayAbility* GA, const EDLAbilityStage& OldStage,const EDLAbilityStage& NewStage)
			{
				if (NewStage == EDLAbilityStage::Post)
				{
					FDLGameplayTaskActiveAbilityPayload Payload;
					Payload.OldStage = OldStage;
					Payload.NewStage = NewStage;
					Payload.GA = GA;
					this->OnPostStart.Broadcast(Payload);
				}
			});

		AbilityNtfStateChangeHandle = ASC->OnNtfStateChange.AddWeakLambda(this, [this](UGameplayAbility* GA, const bool IsBeginState,const FGameplayTag& Tag)
		{
			if (Tag == GetCanRotateTag())
			{
				if (IsBeginState)
				{
					this->OnStartRotate.Broadcast();
				}
				else
				{
					this->OnStopRotate.Broadcast();
				}
			}
		});
		AbilityApplyGEToTargetHandle = ASC->OnGameplayEffectAppliedDelegateToTarget.AddWeakLambda(this,
			[this](UAbilitySystemComponent* Target, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
			{
				if (Spec.GetEffectContext().GetAbility() != Ability.GetDefaultObject())
				{
					return;
				}

				FDLGameplayTaskActiveAbilityPayload Payload;
				Payload.ActiveGEHandle = Handle;
				Payload.EffectSpec = Spec;
				Payload.TargetASC = Target;
				Payload.HitUnit = Target->GetAvatarActor();

				// ReSharper disable once CppExpressionWithoutSideEffects
				this->HitUnit.Broadcast(Payload);

				// ReSharper disable once CppExpressionWithoutSideEffects
				this->ApplyGEToTarget.Broadcast(Payload);
			});


		AbilityApplyGEToSelfHandle = ASC->OnGameplayEffectAppliedDelegateToSelf.AddWeakLambda(this,
			[this](UAbilitySystemComponent* Target, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
			{
				if (Spec.GetEffectContext().GetAbility() != Ability.GetDefaultObject())
				{
					return;
				}

				FDLGameplayTaskActiveAbilityPayload Payload;
				Payload.ActiveGEHandle = Handle;
				Payload.EffectSpec = Spec;
				Payload.TargetASC = Target;
				Payload.HitUnit = Target->GetAvatarActor();

				// ReSharper disable once CppExpressionWithoutSideEffects
				this->HitUnit.Broadcast(Payload);

				// ReSharper disable once CppExpressionWithoutSideEffects
				this->ApplyGEToSelf.Broadcast(Payload);
			});

		const bool IsSucceed = ASC->TryActivateAbilityByClass(Ability.Get());
		if (!IsSucceed)
		{
			this->NtfFailed();
		}
	}



private:

	virtual void OnDestroy(bool bInOwnerFinished) override
	{
		if (ASC)
		{
			ASC->OnAbilityEnded.Remove(AbilityEndHandle);
			ASC->OnGameplayEffectAppliedDelegateToTarget.Remove(AbilityApplyGEToTargetHandle);
			ASC->OnGameplayEffectAppliedDelegateToSelf.Remove(AbilityApplyGEToSelfHandle);
			ASC->OnStageChange.Remove(AbilityStageChangeHandle);
		}
	}

	void NtfFailed()
	{
		Failed.Broadcast(FDLGameplayTaskActiveAbilityPayload{});
		this->EndTask();
	}

private:

	UPROPERTY()
		UDLUnitAbilitySystemComponent* ASC;

	UPROPERTY()
		TSubclassOf<UDLGameplayAbilityBase> Ability;

private:
	FDelegateHandle AbilityStageChangeHandle;
	FDelegateHandle AbilityNtfStateChangeHandle;
	FDelegateHandle AbilityEndHandle;
	FDelegateHandle AbilityApplyGEToTargetHandle;
	FDelegateHandle AbilityApplyGEToSelfHandle;
};

