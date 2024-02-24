#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "CharacterTypesDef.h"
#include "GameFramework/PlayerState.h"
#include "Interface/ICharacterStateListener.h"
#include "Types/CharacterWeaponDef.h"
#include "DLPlayerStateBase.generated.h"


class UDLCharacterStateExtensionComp;
class UDLCharacterBattleStateComp;
class UDLLocalPlayerAbilitySysComp;
class UDLPlayerStateExtensionComp;
class ADLCharacterBase;
struct FDTCharacterInfoBaseRow;
class UDLUnitAbilitySystemComponent;

UCLASS()
class DLGAMEPLAYCORE_API ADLPlayerStateBase
	: public APlayerState
	, public IAbilitySystemInterface
	, public ICharacterStateListener
{
	GENERATED_BODY()

public:

	ADLPlayerStateBase();

	struct FAbilityInfo
	{
		// 技能的Class
		TWeakObjectPtr<UClass> Class;

		// 技能的等级
		int32 Level = 0;

		// 当授予技能时是否激活
		bool IsActiveWhenGive = false;
	};

	struct FPlayerBaseInfo
	{
		TWeakObjectPtr<APawn> Pawn;

		TArray<FAbilityInfo> AbilityInfos;

		TArray<FAbilityInfo> GameplayEffectInfos;

		TArray<FDLCharacterWeaponInfo> CharacterWeaponInfo;
	};


	// 服务端初始化玩家的基础信息
	virtual bool ServerInitPlayerBaseInfo(const FPlayerBaseInfo& Info);

	// 初始化玩家的数据部分
	virtual void OnInitPlayerData(const FPlayerBaseInfo& Info);

public:


#pragma region APlayerState

	virtual float TakeDamage(float DamageAmount,
					FDamageEvent const& DamageEvent,
					AController* EventInstigator,
					AActor* DamageCauser) override;

#pragma endregion

#pragma region IAbilitySystemInterface

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

#pragma endregion 

#pragma region ICharacterStateListener

	virtual FOnGameplayAttributeValueChange& OnGameplayAttributeValueChange(const FGameplayAttribute& Attribute) override;

#pragma endregion


	UDLUnitAbilitySystemComponent* GetASC() const;


	UFUNCTION(BlueprintCallable)
		void SetCharacterInfoBase(const FCharacterInfoBase& InCharacterInfo);

	UFUNCTION(Server, Reliable)
		void ClientLoadComplate();


	const TArray<FDLCharacterWeaponInfo>& GetCurrentWeaponInfo();

	bool IsLoadCharacterBaseInfo() const;

	void InitUnitBaseAttribute(const FDTCharacterInfoBaseRow* CharacterInfo) const;
	void OnCharacterDied();

	ADLCharacterBase* GetCharacterBase() const;

	bool IsInitPS() const { return  bInit; }

public:

	UFUNCTION(BlueprintPure)
		const FName& GetCharacterId()const;

	UFUNCTION(BlueprintCallable)
		const FCharacterInfoBase& GetCharacterInfoBase()const;

	UFUNCTION()
		virtual void OnRep_CharacterBaseInfo();


	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnAbilityActive(UGameplayAbility* Ability);

protected:

	void SetAbilitySystemComp(UDLUnitAbilitySystemComponent* ASC);

private:

	virtual void OnAbilitySystemActorInfoInit();

	virtual void OnAbilitySystemActorInfoClear();

protected:

	UPROPERTY()
		UDLUnitAbilitySystemComponent* AbilitySystem;

	UPROPERTY()
		UDLPlayerStateExtensionComp* Extension;

	UPROPERTY()
		UDLCharacterStateExtensionComp* CharacterStateExtension;

	UPROPERTY()
		UAttributeSet* AttributeSet;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CharacterBaseInfo)
		FCharacterInfoBase CharacterBaseInfo;

	UPROPERTY()
		TArray<FDLCharacterWeaponInfo> CharacterWeaponInfo;

	UPROPERTY()
		bool bIsLoadCharacterBaseInfo = false;

	UPROPERTY()
		bool bInit = false;
		

	UPROPERTY()
		UDLLocalPlayerAbilitySysComp* LocalPlayerAbilitySys;

	UPROPERTY()
		UDLCharacterBattleStateComp* CharacterBattleStateComp;
};
