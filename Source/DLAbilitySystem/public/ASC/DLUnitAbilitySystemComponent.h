#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GameAbilitySysDef.h"
#include "GameFramework/Character.h"
#include "Interface/IDLAbilitySysComponent.h"
#include "DLUnitAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FDLAbilityInputChange, EInputEvent, int32 /*InputId*/);


USTRUCT(BlueprintType)
struct FDLDebugAbilityInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName AbilityInstanceName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ActiveId;
};


DECLARE_MULTICAST_DELEGATE_OneParam(FDLDebugOnAbilityEvent, const FDLDebugAbilityInfo& Info);


UCLASS()
class DLABILITYSYSTEM_API UDLUnitAbilitySystemComponent
	: public UAbilitySystemComponent
	, public IDLAbilitySysComponent
{
	GENERATED_BODY()

public:

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnStageChange, UGameplayAbility*, EDLAbilityStage/*OldStage*/, EDLAbilityStage/*NewStage*/);
	FOnStageChange OnStageChange;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnNtfChange, UGameplayAbility*, FGameplayTag/*NewStage*/);
	FOnNtfChange OnNtfChange;
	
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnNtfStateChange, UGameplayAbility*, bool/*OldStage*/, FGameplayTag/*NewStage*/);
	FOnNtfStateChange OnNtfStateChange;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameplayTagChange, const FGameplayTag& /*Tag*/, bool /*bTagExists*/);
	FOnGameplayTagChange OnGameplayTagChange;

public:

	FSimpleDelegate OnInitActorInfo;
	FSimpleDelegate OnClearActorInfo;
	FSimpleMulticastDelegate OnActivateAbilitiesChange;


#pragma region UAbilitySystemComponent

	// 接管 获取能力、移除能力
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	// Owner Tag 更新
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool bTagExists) override;

	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void ClearActorInfo() override;

	virtual void OnRep_ActivateAbilities() override;
#pragma endregion

	/**
	 * @brief 手动增加 Tags
	 * @param Tags
	 * @note 如果是服务器调用，那么会给客户端同步这个 Tag
	 *		 如果是客户端调用，那么需要服务器也走同样的逻辑调用，否则就会导致这个Tag只是本地添加，并且会被新的同步把客户端的Tag意外丢失
	 *		 目前如果 服务器先调用，客户端再调用，会导致 Tag 数量比实际的多，但是当前并不考虑 Stack Count 所以不会有什么问题
	 */
	void AddGameplayTags(const FGameplayTagContainer& Tags);

	/**
	 * @brief
	 * @param Tags 手动移除 Tags
	 */
	void RemoveGameplayTags(const FGameplayTagContainer& Tags);


	ACharacter* GetLockCharacterActor()
	{
		//TODO 没有实现锁敌功能
		return nullptr;
	}

	ACharacter* GetSelfCharacterActor() const
	{
		return Cast<ACharacter>(GetAvatarActor());
	}

	bool TestAbilityBlock(const UGameplayAbility* Ability);

	void ClearAbilityBeCancelAbilityTag(const UGameplayAbility* Ability);
#pragma region IDLAbilitySysComponent

	virtual UGameplayAbility* K2_GetAnimatingAbility() override
	{
		return GetAnimatingAbility();
	};


	virtual void AppendAbilityBeCancelAbilityTag(UGameplayAbility* TargetAbility, const FGameplayTagContainer& AbilityTag) override;

	virtual void RemoveAbilityBeCancelAbilityTag(UGameplayAbility* TargetAbility, const FGameplayTagContainer& AbilityTag) override;



#pragma endregion

	virtual void DisplayDebugInfo(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) const {}

public:

	bool GrantAbility(const TArray<FGameplayAbilitySpec>& InInitPrimaryAbility);


	FActiveGameplayEffectHandle FindActiveGEHandle(TSubclassOf<UGameplayEffect> GE);

	void StopAbility();

public:

	UFUNCTION(BlueprintCallable)
		FGameplayAbilitySpecHandle FindAbilitySpecHandleForClass(TSubclassOf<UGameplayAbility> AbilityClass, UObject* OptionalSourceObject);

	UFUNCTION(BlueprintCallable)
		FGameplayAbilitySpecHandle FindAbilitySpecByTag(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
		void ServerAddLooseTags(const FGameplayTag& Tag, int32 Count);

	UFUNCTION(Server, Reliable)
		void ServerRemoveLooseTags(const FGameplayTag& Tag, int32 Count);

	UFUNCTION(Client, Reliable)
		void ClientAddLooseTags(const FGameplayTag& Tag, int32 Count);

	UFUNCTION(Client, Reliable)
		void ClientRemoveLooseTags(const FGameplayTag& Tag, int32 Count);


	UFUNCTION(BlueprintCallable)
		void AddOwnerTag(const FGameplayTag& Tag, int32 Count = 1);

	UFUNCTION(BlueprintCallable)
		void RemoveOwnerTag(const FGameplayTag& Tag, int32 Count = 1);




#if WITH_EDITOR

	UFUNCTION(NetMulticast, Reliable)
		void MulticastDebugAbilityActive(const FDLDebugAbilityInfo& Info);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastDebugAbilityEnd(const FDLDebugAbilityInfo& Info);

	
		void DebugAbilityActive(const FDLDebugAbilityInfo& Info);

	
		void DebugAbilityEnd(const FDLDebugAbilityInfo& Info);

#endif

public:

	FDLDebugOnAbilityEvent OnDebugAbilityActive;

	FDLDebugOnAbilityEvent OnDebugAbilityEnd;


protected:

	bool bIsInit = false;

protected:

	// 一个技能能被 能被 拥有什么标签的 的技能打断
	UPROPERTY()
		TMap<FGameplayAbilitySpecHandle, FGameplayTagContainer> AbilityBeCancelByAbilityTag;

	UPROPERTY()
		int32 ActiveAbilityCount = 0;

	TMap<FGameplayTag, TArray<FGameplayAbilitySpecHandle>> NeedCancelAbilityTags;
};
