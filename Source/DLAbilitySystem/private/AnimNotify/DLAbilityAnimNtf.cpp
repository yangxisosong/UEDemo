#include "DLAbilityAnimNtf.h"

#include "ConvertString.h"
#include "DLAbilityLog.h"
#include "DLGameplayAbilityBase.h"


UDLGameplayAbilityBase* UDLAbilityAnimNtfStateBase::GetActiveAbility(USkeletalMeshComponent* MeshComp) const
{
	const IAbilitySystemInterface* AbilitySystem = Cast<IAbilitySystemInterface>(MeshComp->GetOwner());
	if (AbilitySystem)
	{
		const auto ASC = AbilitySystem->GetAbilitySystemComponent();
		if (ASC)
		{
			UDLGameplayAbilityBase* Ability = Cast<UDLGameplayAbilityBase>(ASC->GetAnimatingAbility());
			return Ability;
		}
	}
	return nullptr;
}

void UDLAbilityAnimNtfStateBase::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{
	UDLGameplayAbilityBase* Ability = GetActiveAbility(MeshComp);
	if (Ability)
	{
		if (Ability->CanTriggerEventByAnimNtf(Animation, nullptr))
		{
			Ability->AddWittingEndNtfState(this);

			UE_LOG(LogDLAbility, Log, TEXT("<%s>(%s) %s NotifyBegin Enter"),
						TO_STR(Ability),
						ENUM_TO_STR(ENetRole, Ability->GetOwnerNetRole()),
						TO_STR(this)
			);

			this->OnBeginNtf(Ability, MeshComp, Animation, TotalDuration);

			UE_LOG(LogDLAbility, Log, TEXT("<%s>(%s) %s NtfBegin Leave"),
				TO_STR(Ability),
				ENUM_TO_STR(ENetRole, Ability->GetOwnerNetRole()),
				TO_STR(this)
			);
		}
	}
}

void UDLAbilityAnimNtfStateBase::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UDLGameplayAbilityBase* Ability = GetActiveAbility(MeshComp);
	if (Ability)
	{
		if (Ability->CanTriggerEventByAnimNtf(Animation, this))
		{
			Ability->RemoveWittingEndNtfState(this);

			UE_LOG(LogDLAbility, Log, TEXT("<%s>(%s) %s NotifyEnd Enter"),
				TO_STR(Ability),
				ENUM_TO_STR(ENetRole, Ability->GetOwnerNetRole()),
				TO_STR(this)
			);

			this->OnEndNtf(Ability, MeshComp, Animation);

			UE_LOG(LogDLAbility, Log, TEXT("<%s>(%s) %s NotifyEnd Leave"),
				TO_STR(Ability),
				ENUM_TO_STR(ENetRole, Ability->GetOwnerNetRole()),
				TO_STR(this)
			);
		}
	}
}

UDLGameplayAbilityBase* UDLAbilityAnimNtfBase::GetActiveAbility(USkeletalMeshComponent* MeshComp) const
{
	const IAbilitySystemInterface* AbilitySystem = Cast<IAbilitySystemInterface>(MeshComp->GetOwner());
	if (AbilitySystem)
	{
		const auto ASC = AbilitySystem->GetAbilitySystemComponent();
		if (ASC)
		{
			UDLGameplayAbilityBase* Ability = Cast<UDLGameplayAbilityBase>(ASC->GetAnimatingAbility());
			return Ability;
		}
	}
	return nullptr;
}

void UDLAbilityAnimNtfBase::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UDLGameplayAbilityBase* Ability = GetActiveAbility(MeshComp);
	if (Ability)
	{
		if (Ability->GetCurrentMontage() == Animation)
		{
			UE_LOG(LogDLAbility, Log, TEXT("<%s>(%s) %s Notify Enter"),
				TO_STR(Ability),
				ENUM_TO_STR(ENetRole, Ability->GetOwnerNetRole()),
				TO_STR(this)
			);

			this->OnNotify(Ability, MeshComp, Animation);

			UE_LOG(LogDLAbility, Log, TEXT("<%s>(%s) %s Notify Leave"),
				TO_STR(Ability),
				ENUM_TO_STR(ENetRole, Ability->GetOwnerNetRole()),
				TO_STR(this)
			);
		}
	}
}

void UDLAbilityAnimNtfBase::OnNotify_Implementation(
	UDLGameplayAbilityBase* Ability, USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation)const
{

}

FString UDLAnimNS_WeaponAttack::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("%s WeaponAttack"), *WeaponAttackInfo.EventTag.ToString());
}

void UDLAnimNS_WeaponAttack::OnBeginNtf_Implementation(UDLGameplayAbilityBase* Ability,
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)const
{
	if (Ability && Ability->IsLocallyControlled())
	{
		const auto WeaponAttack = Cast<IDLAbilityWeaponAttack>(Ability);
		if (WeaponAttack)
		{
			WeaponAttack->BeginAttack(WeaponAttackInfo);
		}
	}
}

void UDLAnimNS_WeaponAttack::OnEndNtf_Implementation(UDLGameplayAbilityBase* Ability, USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation)const
{
	if (Ability && Ability->IsLocallyControlled())
	{
		const auto WeaponAttack = Cast<IDLAbilityWeaponAttack>(Ability);
		if (WeaponAttack)
		{
			WeaponAttack->EndAttack(WeaponAttackInfo);
		}
	}
}
