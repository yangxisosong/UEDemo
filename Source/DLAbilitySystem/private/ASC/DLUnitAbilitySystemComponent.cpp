#include "ASC/DLUnitAbilitySystemComponent.h"

#include "InputMappingContext.h"
#include "DisplayDebugHelpers.h"
#include "DLGameplayAbilityBase.h"
#include "EnhancedInputSubsystems.h"
#include "AbilityChain/AbilityChain.h"
#include "DLKit/Public/ConvertString.h"
#include "Engine/Canvas.h"
#include "DLAbilityLog.h"
#include "DLAbilitySystemGlobal.h"

void UDLUnitAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	UDLGameplayAbilityBase* Ability = Cast<UDLGameplayAbilityBase>(AbilitySpec.Ability);

	if (Ability)
	{
		auto& Tags = Ability->GetCancelAbilityByTags();

		for (auto& Tag : Tags.IgnoreTags)
		{
			NeedCancelAbilityTags.FindOrAdd(Tag).Add(AbilitySpec.Handle);
		}

		for (auto& Tag : Tags.RequireTags)
		{
			NeedCancelAbilityTags.FindOrAdd(Tag).Add(AbilitySpec.Handle);
		}
	}

	OnActivateAbilitiesChange.Broadcast();

	UE_LOG(LogDLAbility, Log, TEXT("Give Ability %s"), *AbilitySpec.GetDebugString());
}

void UDLUnitAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);

	UDLGameplayAbilityBase* Ability = Cast<UDLGameplayAbilityBase>(AbilitySpec.Ability);

	if (Ability)
	{
		auto& Tags = Ability->GetCancelAbilityByTags();

		for (auto& Tag : Tags.IgnoreTags)
		{
			NeedCancelAbilityTags.FindOrAdd(Tag).Remove(AbilitySpec.Handle);
		}

		for (auto& Tag : Tags.RequireTags)
		{
			NeedCancelAbilityTags.FindOrAdd(Tag).Remove(AbilitySpec.Handle);
		}
	}

	OnActivateAbilitiesChange.Broadcast();

	UE_LOG(LogDLAbility, Log, TEXT("Remove Ability %s"), *AbilitySpec.GetDebugString());
}

void UDLUnitAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool bTagExists)
{
	Super::OnTagUpdated(Tag, bTagExists);
	
	UE_LOG(LogDLAbilitySystem, Verbose, TEXT("[%s] (%s) OnTagUpdated ->  Tag: %s bTagExists: %d "), TO_STR(this->GetAvatarActor()), NET_ROLE_STR(this->GetAvatarActor()), *Tag.ToString(), bTagExists);

	if (GetOwner()->HasAuthority() && bTagExists)
	{
		if (NeedCancelAbilityTags.Contains(Tag))
		{
			static FGameplayTagContainer MyTags;
			MyTags.Reset();

			GetOwnedGameplayTags(MyTags);

			for (const auto& Data : NeedCancelAbilityTags[Tag])
			{
				const auto AbilitySpec = FindAbilitySpecFromHandle(Data);
				if (AbilitySpec)
				{
					UDLGameplayAbilityBase* Ability = Cast<UDLGameplayAbilityBase>(AbilitySpec->Ability);
					if (Ability)
					{
						if (Ability->GetCancelAbilityByTags().RequirementsMet(MyTags))
						{
							this->CancelAbility(Ability);
						}
					}
				}
			}

		}
	}


	OnGameplayTagChange.Broadcast(Tag, bTagExists);
}

void UDLUnitAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);

	// TODO 需要根据 Tag来做 技能区分，现在仅仅是临时处理
	if (Cast<UDLGameplayAbilityBase>(Ability))
	{
		ActiveAbilityCount++;
	}

	// 处理这个技能打断其他技能的问题
	static TMap<FGameplayAbilitySpecHandle, FGameplayTagContainer> CopyAbilityBeCancelByAbilityTag;
	CopyAbilityBeCancelByAbilityTag.Empty();

	CopyAbilityBeCancelByAbilityTag.Append(AbilityBeCancelByAbilityTag);

	for (const auto& BeCancelByAbilityTag : CopyAbilityBeCancelByAbilityTag)
	{
		if (Ability->AbilityTags.HasAny(BeCancelByAbilityTag.Value))
		{
			UE_LOG(LogDLAbilitySystem, Log, TEXT("Active New Ability %s , So CancelAbility %s"), TO_STR(Ability), *BeCancelByAbilityTag.Key.ToString());
			const auto Spec = this->FindAbilitySpecFromHandle(BeCancelByAbilityTag.Key);
			if (ensureAlwaysMsgf(Spec, TEXT("一定能够找到")))
			{
				this->CancelAbilitySpec(*Spec, Ability);
			}
		}
	}



	// ReSharper disable once CppLocalVariableMayBeConst
	UDLGameplayAbilityBase* AbilityBase = Cast<UDLGameplayAbilityBase>(Ability);
	if (!AbilityBase)
	{
		return;
	}


#if WITH_EDITOR

	FDLDebugAbilityInfo Info;
	Info.AbilityInstanceName = Ability->GetFName();
	Info.ActiveId = AbilityBase->GetDebugActiveID();

	this->DebugAbilityActive(Info);

#endif


}

void UDLUnitAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,
	bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	// TODO 需要根据 Tag来做 技能区分，现在仅仅是临时处理
	if (Cast<UDLGameplayAbilityBase>(Ability))
	{
		ActiveAbilityCount--;
	}



	// ReSharper disable once CppLocalVariableMayBeConst
	UDLGameplayAbilityBase* AbilityBase = Cast<UDLGameplayAbilityBase>(Ability);
	if (!AbilityBase)
	{
		return;
	}


#if WITH_EDITOR

	FDLDebugAbilityInfo Info;
	Info.AbilityInstanceName = Ability->GetFName();
	Info.ActiveId = AbilityBase->GetDebugActiveID();

	this->DebugAbilityEnd(Info);

#endif
}

void UDLUnitAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (InOwnerActor && InAvatarActor)
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		OnInitActorInfo.ExecuteIfBound();
	}
}

void UDLUnitAbilitySystemComponent::ClearActorInfo()
{
	Super::ClearActorInfo();

	// ReSharper disable once CppExpressionWithoutSideEffects
	OnClearActorInfo.ExecuteIfBound();
}

void UDLUnitAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	OnActivateAbilitiesChange.Broadcast();
}

void UDLUnitAbilitySystemComponent::AddGameplayTags(const FGameplayTagContainer& Tags)
{
	if (!GetOwner())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		this->UpdateTagMap(Tags, 1);
		this->AddMinimalReplicationGameplayTags(Tags);

		if (this->GetOwnerActor())
		{
			this->GetOwnerActor()->ForceNetUpdate();
		}
	}
	else
	{
		this->UpdateTagMap(Tags, 1);
	}
}

void UDLUnitAbilitySystemComponent::RemoveGameplayTags(const FGameplayTagContainer& Tags)
{
	if (!GetOwner())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		this->UpdateTagMap(Tags, -1);
		this->RemoveMinimalReplicationGameplayTags(Tags);

		if (this->GetOwnerActor())
		{
			this->GetOwnerActor()->ForceNetUpdate();
		}
	}
	else
	{
		this->UpdateTagMap(Tags, -1);
	}
}


bool UDLUnitAbilitySystemComponent::TestAbilityBlock(const UGameplayAbility* Ability)
{
	// 如果 AbilityBeCancelByAbilityTag  中没有任何值，说明 这个技能不能被打断，技能的生命周期中绝大多数都是这种情况

	for (const auto& BeCancelByAbilityTag : AbilityBeCancelByAbilityTag)
	{
		if (Ability->AbilityTags.HasAny(BeCancelByAbilityTag.Value))
		{
			return false;
		}
	}

	if (ActiveAbilityCount > 0)
	{
		return true;
	}

	return false;
}

namespace UnitASCPrivate
{
	FString ToString(const TMap<FGameplayAbilitySpecHandle, FGameplayTagContainer>& Value)
	{
		FString Ret = TEXT("AbilityBeCancelByAbilityTag Value =====Begin======\n");
		for (const auto& BeCancelByAbilityTag : Value)
		{
			Ret += FString::Printf(TEXT("AbilityHandle %s —> Tags %s\n"), *BeCancelByAbilityTag.Key.ToString(), *BeCancelByAbilityTag.Value.ToStringSimple());
		}
		Ret += TEXT("AbilityBeCancelByAbilityTag Value =====End======");
		return Ret;
	}
}


void UDLUnitAbilitySystemComponent::ClearAbilityBeCancelAbilityTag(const UGameplayAbility* Ability)
{
	const auto AbilityHandle = FindAbilitySpecHandleForClass(Ability->GetClass(), nullptr);
	if (!ensureAlwaysMsgf(AbilityHandle.IsValid(), TEXT("这里不应该取到无效的 Handle")))
	{
		return;
	}

	AbilityBeCancelByAbilityTag.Remove(AbilityHandle);

	UE_LOG(LogDLAbilitySystem, Log, TEXT("(%s)ClearAbilityBeCancelAbilityTag  Target : %s"), NET_ROLE_STR(GetAvatarActor()), TO_STR(Ability));
	UE_LOG(LogDLAbilitySystem, Log, TEXT("%s"), *UnitASCPrivate::ToString(AbilityBeCancelByAbilityTag));
}



void UDLUnitAbilitySystemComponent::AppendAbilityBeCancelAbilityTag(UGameplayAbility* TargetAbility,
	const FGameplayTagContainer& AbilityTag)
{
	if (!TargetAbility)
	{
		return;
	}

	const auto AbilityHandle = FindAbilitySpecHandleForClass(TargetAbility->GetClass(), nullptr);
	if (!ensureAlwaysMsgf(AbilityHandle.IsValid(), TEXT("这里不应该取到无效的 Handle")))
	{
		return;
	}

	AbilityBeCancelByAbilityTag.FindOrAdd(AbilityHandle).AppendTags(AbilityTag);

	UE_LOG(LogDLAbilitySystem, Log, TEXT("(%s)AppendAbilityBeCancelAbilityTag  Target : %s, AbilityTag %s"),
		NET_ROLE_STR(GetAvatarActor()),
		TO_STR(TargetAbility),
		*AbilityTag.ToStringSimple()
	);

	UE_LOG(LogDLAbilitySystem, Log, TEXT("%s"), *UnitASCPrivate::ToString(AbilityBeCancelByAbilityTag));
}

void UDLUnitAbilitySystemComponent::RemoveAbilityBeCancelAbilityTag(UGameplayAbility* TargetAbility,
	const FGameplayTagContainer& AbilityTag)
{
	if (!TargetAbility)
	{
		return;
	}

	const auto AbilityHandle = FindAbilitySpecHandleForClass(TargetAbility->GetClass(), nullptr);
	if (!ensureAlwaysMsgf(AbilityHandle.IsValid(), TEXT("这里不应该取到无效的 Handle")))
	{
		return;
	}

	AbilityBeCancelByAbilityTag.FindOrAdd(AbilityHandle).RemoveTags(AbilityTag);

	UE_LOG(LogDLAbilitySystem, Log, TEXT("(%s)RemoveAbilityBeCancelAbilityTag  Target : %s, AbilityTag %s"), NET_ROLE_STR(GetAvatarActor()), TO_STR(TargetAbility), *AbilityTag.ToStringSimple());

	UE_LOG(LogDLAbilitySystem, Log, TEXT("%s"), *UnitASCPrivate::ToString(AbilityBeCancelByAbilityTag));
}

bool UDLUnitAbilitySystemComponent::GrantAbility(const TArray<FGameplayAbilitySpec>& InInitPrimaryAbility)
{
	for (const auto& AbilitySpec : InInitPrimaryAbility)
	{
		const auto Ability = Cast<UDLGameplayAbilityBase>(AbilitySpec.Ability);
		if (Ability)
		{
			static TArray<TSubclassOf<UDLGameplayAbilityBase>> AbilityArr;
			AbilityArr.Empty();

			Ability->GetSubAbilityClassArr(AbilityArr);

			for (TSubclassOf<UDLGameplayAbilityBase> Class : AbilityArr)
			{
				if (!Class)
				{
					UE_LOG(LogDLAbility, Warning, TEXT("%s 存在无效的 SubAbility"), TO_STR(Ability));
					continue;
				}

				const auto DefaultObject = Class.GetDefaultObject();

				FGameplayAbilitySpec AnticipationSpec(
					DefaultObject,
					AbilitySpec.Level,
					static_cast<int32>(DefaultObject->GetInputId()));

				if (AnticipationSpec.Ability)
				{
					this->GiveAbility(AnticipationSpec);
				}
			}
		}

		if (AbilitySpec.bActivateOnce)
		{
			FGameplayAbilitySpec Copy(AbilitySpec);
			this->GiveAbilityAndActivateOnce(Copy);
		}
		else
		{
			this->GiveAbility(AbilitySpec);
		}
	}

	bIsInit = true;

	return true;
}


FActiveGameplayEffectHandle UDLUnitAbilitySystemComponent::FindActiveGEHandle(const TSubclassOf<UGameplayEffect> GE)
{
	for (const FActiveGameplayEffect& Effect : &ActiveGameplayEffects)
	{
		if (Effect.Spec.Def && Effect.Spec.Def->GetClass() == GE)
		{
			return Effect.Handle;
		}
	}
	return {};
}

void UDLUnitAbilitySystemComponent::StopAbility()
{
	this->CancelAbilities();


}


FGameplayAbilitySpecHandle UDLUnitAbilitySystemComponent::FindAbilitySpecHandleForClass(
	TSubclassOf<UGameplayAbility> AbilityClass, UObject* OptionalSourceObject)
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		TSubclassOf<UGameplayAbility> SpecAbilityClass = Spec.Ability->GetClass();
		if (SpecAbilityClass == AbilityClass)
		{
			if (!OptionalSourceObject || (OptionalSourceObject && Spec.SourceObject == OptionalSourceObject))
			{
				return Spec.Handle;
			}
		}
	}

	return FGameplayAbilitySpecHandle();
}

FGameplayAbilitySpecHandle UDLUnitAbilitySystemComponent::FindAbilitySpecByTag(const FGameplayTag& AbilityTag)
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability->AbilityTags.HasTag(AbilityTag))
		{
			return Spec.Handle;
		}
	}

	return FGameplayAbilitySpecHandle();
}

void UDLUnitAbilitySystemComponent::ClientAddLooseTags_Implementation(const FGameplayTag& Tag, int32 Count)
{
	this->AddLooseGameplayTag(Tag, Count);
}

void UDLUnitAbilitySystemComponent::ClientRemoveLooseTags_Implementation(const FGameplayTag& Tag, int32 Count)
{
	this->RemoveLooseGameplayTag(Tag, Count);
}

void UDLUnitAbilitySystemComponent::AddOwnerTag(const FGameplayTag& Tag, int32 Count)
{
	this->AddLooseGameplayTag(Tag, Count);
}

void UDLUnitAbilitySystemComponent::RemoveOwnerTag(const FGameplayTag& Tag, int32 Count)
{
	this->RemoveLooseGameplayTag(Tag, Count);
}



#if WITH_EDITOR

void UDLUnitAbilitySystemComponent::DebugAbilityActive(const FDLDebugAbilityInfo& Info)
{
	if (GetAvatarActor() && GetAvatarActor()->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		OnDebugAbilityActive.Broadcast(Info);
	}
	else
	{
		MulticastDebugAbilityActive(Info);
	}
}

void UDLUnitAbilitySystemComponent::DebugAbilityEnd(const FDLDebugAbilityInfo& Info)
{
	if (!Cast<APawn>(GetAvatarActor()))
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(GetAvatarActor());
	if (Pawn->IsPlayerControlled() && Pawn->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		OnDebugAbilityEnd.Broadcast(Info);
	}
	else
	{
		MulticastDebugAbilityEnd(Info);
	}
}

void UDLUnitAbilitySystemComponent::MulticastDebugAbilityEnd_Implementation(const FDLDebugAbilityInfo& Info)
{
	if (GetAvatarActor()->GetLocalRole() != ENetRole::ROLE_Authority)
	{
		OnDebugAbilityEnd.Broadcast(Info);
	}
}

void UDLUnitAbilitySystemComponent::MulticastDebugAbilityActive_Implementation(const FDLDebugAbilityInfo& Info)
{
	if (GetAvatarActor()->GetLocalRole() != ENetRole::ROLE_Authority)
	{
		OnDebugAbilityActive.Broadcast(Info);
	}
}

#endif


void UDLUnitAbilitySystemComponent::ServerRemoveLooseTags_Implementation(const FGameplayTag& Tag, int32 Count)
{
	this->RemoveLooseGameplayTag(Tag, Count);
}

void UDLUnitAbilitySystemComponent::ServerAddLooseTags_Implementation(const FGameplayTag& Tag, int32 Count)
{
	this->AddLooseGameplayTag(Tag, Count);
}

