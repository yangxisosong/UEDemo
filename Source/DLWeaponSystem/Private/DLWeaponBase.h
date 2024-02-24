// ReSharper disable CppClassCanBeFinal

#pragma once
#include "CoreMinimal.h"
#include "ConvertString.h"
#include "DLUnitSelectorInstance.h"
#include "GameplayTagContainer.h"
#include "WeaponTypes.h"
#include "DLWeaponSystem/Public/IDLWeapon.h"
#include "Engine/EngineTypes.h"
#include "Net/UnrealNetwork.h"
#include "DLWeaponLogDef.h"
#include "GameFramework/Character.h"
#include "DLWeaponBase.generated.h"


UCLASS()
class ADLWeaponBase
	: public AActor
	, public IDLWeapon
{
	GENERATED_BODY()

private:

	/**
	 * @brief 武器的ID
	 */
	UPROPERTY(EditAnywhere, Category = BaseInfo, meta = (AllowPrivateAccess))
		FName WeaponId;

	/**
	 * @brief 武器的标签集合
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseInfo, meta = (AllowPrivateAccess))
		FGameplayTagContainer WeaponTags;

	/**
	 * @brief 这个武器可挂载的点，比如 左手 右手  等
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseInfo, meta = (AllowPrivateAccess))
		TArray<FWeaponAttachInfo> AttachOptionsInfo;

	/**
	 * @brief 这个武器的材质
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseInfo, meta = (AllowPrivateAccess))
		UPhysicalMaterial* PhysicalMaterial;


	/**
	 * @brief 武器的默认的单位选择器
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BaseInfo, meta = (AllowPrivateAccess))
		class UDLGameplayTargetSelectorShapeTraceBase* DefaultUnitSelector;


protected:

	UFUNCTION(BlueprintNativeEvent, meta = (AutoCreateRefTerm = "Parameter"))
		void OnBeginAttack(const FWeaponAttackArg& Parameter);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = OnHitUnits)
		void K2_OnHitUnits(const TArray<FHitResult>& HitRets);

	UFUNCTION(BlueprintNativeEvent)
		void OnEndAttack();


	UFUNCTION(BlueprintPure)
		const FWeaponAttackContext& GetAttackContext() const;

	UFUNCTION(BlueprintPure)
		bool IsLocallyController() const;

private:

	UFUNCTION()
		void OnHitUnit(const TArray<FHitResult>& HitRets);

public:

	void AppendBaseWeaponTags(const FGameplayTagContainer& InTags);

protected:

	virtual void OnBeginAttack_Implementation(const FWeaponAttackArg& Parameter);

	virtual void OnEndAttack_Implementation();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:

	UFUNCTION(BlueprintPure)
		virtual bool IsAttacking() const override;

	virtual FName GetWeaponId() const override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	virtual void SetAttackContext(const FWeaponAttackContext& InAttackContext) override;

	virtual void ClearAttackContext() override;

	virtual void BeginAttack(const FWeaponAttackArg& Parameter) override;

	virtual void EndAttack() override;

	virtual TOptional<FWeaponAttachInfo> GetWeaponAttachInfo(const FGameplayTag& SocketTag) override;

	virtual AActor* CastToActor() override;

	virtual UPhysicalMaterial* GetPhysicalMaterial() const override;

	virtual FString ToString() const override;

	virtual void SetCurrentBoneSocket(const FGameplayTag& BoneSocket) override;

	virtual FGameplayTag GetCurrentBoneSocket() const override;

	virtual bool AttachToCharacter(ACharacter* InOwnerCharacter, const FGameplayTag& SocketTag, const FName& SocketName) override;

	virtual void DetachToCharacter() override;

protected:

	UPROPERTY(BlueprintReadWrite)
		UDLUnitSelectorInstance* SelectorInstance;

	UPROPERTY()
		bool bIsAttacking = false;

	UPROPERTY()
		FWeaponAttackContext AttackContext;

	UPROPERTY()
		bool bAttackContextIsValid = false;

	UPROPERTY(Replicated)
		FGameplayTag CurrentBoneSocket;
};



