#pragma once

#include "CoreMinimal.h"

#include "Abilities/Tasks/AbilityTask.h"
#include "DLGameplayAbilityLib.h"
#include "PlayMontageAndWaitForEvent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayMontageAndWaitForEventDelegate, const FDLAbilityEventData&, EventData);


UCLASS()
class DLABILITYSYSTEM_API UPlayMontageAndWaitForEventTask
	: public UAbilityTask
{
	GENERATED_BODY()

public:



	// 播放完成
	UPROPERTY(BlueprintAssignable)
		FPlayMontageAndWaitForEventDelegate OnCompleted;


	UPROPERTY(BlueprintAssignable)
		FPlayMontageAndWaitForEventDelegate OnBlendOut;

	// 被打断
	UPROPERTY(BlueprintAssignable)
		FPlayMontageAndWaitForEventDelegate OnInterrupted;

	// 被取消
	UPROPERTY(BlueprintAssignable)
		FPlayMontageAndWaitForEventDelegate OnCancelled;

	// 接收到事件
	UPROPERTY(BlueprintAssignable)
		FPlayMontageAndWaitForEventDelegate EventReceived;

	/**
	 *	播放一个蒙太奇，等待动画中的事件响应
	 *
	 * @param OwningAbility
	 * @param TaskInstanceName 
	 * @param MontageToPlay 
	 * @param EventTags	需要等待的事件Tags
	 * @param PlayRate 播放的速率
	 * @param PlayMontageStartSection 
	 * @param bStopWhenAbilityEnds 
	 * @param AnimRootMotionTranslationScale 
	 * @return
	 * @see UDLGameplayAbilityLib::SendAbilityEventToActor
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UPlayMontageAndWaitForEventTask* PlayMontageAndWaitForEvent(
			UGameplayAbility* OwningAbility,
			FName TaskInstanceName,
			UAnimMontage* MontageToPlay,
			FGameplayTagContainer EventTags,
			float PlayRate = 1.f,
			FName PlayMontageStartSection = NAME_None,
			bool bStopWhenAbilityEnds = true,
			float AnimRootMotionTranslationScale = 1.f);

private:

	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;

private:

	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted) const;
	void OnAbilityCancelled() const;
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) const;
	bool StopPlayingMontage() const;

private:

	UPROPERTY()
		UAnimMontage* MontageToPlay;

	UPROPERTY()
		FGameplayTagContainer EventTags;

private:

	// 动画播放的速率
	float Rate;

	FName StartSection;

	float AnimRootMotionTranslationScale;

	bool bStopWhenAbilityEnds;

	FOnMontageBlendingOutStarted BlendingOutDelegate;

	FOnMontageEnded MontageEndedDelegate;

	FDelegateHandle CancelledHandle;

	FDelegateHandle EventHandle;
};
