#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Interface/IDLAbilityWeaponAttack.h"
#include "DLAbilityAnimNtf.generated.h"


class UDLGameplayAbilityBase;

UCLASS(hideFunctions = (Received_NotifyBegin, Received_NotifyEnd))
class UDLAbilityAnimNtfStateBase : public UAnimNotifyState
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
		UDLGameplayAbilityBase* GetActiveAbility(USkeletalMeshComponent* MeshComp) const;
public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override final;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override final;

	virtual void OnBeginNtf_Implementation(UDLGameplayAbilityBase* Ability, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)const { }

	virtual void OnEndNtf_Implementation(UDLGameplayAbilityBase* Ability, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)const { }

protected:

	UFUNCTION(BlueprintNativeEvent)
		void OnBeginNtf(UDLGameplayAbilityBase* Ability, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)const;

	UFUNCTION(BlueprintNativeEvent)
		void OnEndNtf(UDLGameplayAbilityBase* Ability, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)const;
};


UCLASS(hideFunctions = Received_Notify)
class UDLAbilityAnimNtfBase : public UAnimNotify
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
		UDLGameplayAbilityBase* GetActiveAbility(USkeletalMeshComponent* MeshComp) const;

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:

	UFUNCTION(BlueprintNativeEvent)
		void OnNotify(UDLGameplayAbilityBase* Ability, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const;
};

UCLASS()
class UDLAnimNS_WeaponAttack : public UDLAbilityAnimNtfStateBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
		FWeaponAttackNtfInfo WeaponAttackInfo;

protected:

	virtual FString GetNotifyName_Implementation() const override;

	virtual void OnBeginNtf_Implementation(UDLGameplayAbilityBase* Ability, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)const override;

	virtual void OnEndNtf_Implementation(UDLGameplayAbilityBase* Ability, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)const override;
};
