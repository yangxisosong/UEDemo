// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/IDLLocalPlayerGameplayInterface.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Object.h"
#include "DLLocalPlayerGameplaySubsystem.generated.h"

class ADLPlayerStateBase;

UCLASS()
class DLGAMEPLAYCORE_API UDLLocalPlayerGameplaySubsystem
	: public ULocalPlayerSubsystem
	, public IDLLocalPlayerGameplayInterface
{
	GENERATED_BODY()

public:

	virtual TScriptInterface <IDLPrimaryPlayerAbilitySystem> GetPrimaryPlayerAbilitySystem() override;

	virtual TScriptInterface <ICharacterStateAccessor> GetPrimaryPlayerStateAccessor() override;

	virtual TScriptInterface <ICharacterStateListener> GetPrimaryPlayerStateListener() override;

	virtual TScriptInterface<ICharacterStateAccessor> GetEnemyUnitStateAccessor() override;

	virtual TScriptInterface<ICharacterStateListener> GetEnemyUnitStateListener() override;

	virtual void OnLocalPlayerGameplayReady_CallOrRegister(const FSimpleMulticastDelegate::FDelegate& Delegate) override;

	virtual void OnLocalPlayerGameplayReady_Unregister(UObject* BindObject) override;

	virtual void OnPlayerAbilitySysReady_CallOrRegister(const FSimpleMulticastDelegate::FDelegate& Delegate) override;

	virtual void OnPlayerAbilitySysReady_Unregister(UObject* BindObject) override;

	virtual bool IsInBattleState() override;

	virtual bool IsGameplayReady() override;

	virtual bool HasEnemyUnitFocus() override;

	virtual void PlayerReborn() override;

protected:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	void OnPawnDied();
	void OnChangeAsEnemyUnit(AActor* Actor);
	void SetPlayerState(ADLPlayerStateBase* PS);


private:

	void OnPlayerPawnInitReady(ADLPlayerStateBase* PS);

	UFUNCTION()
	void OnBattleStateChange(bool InBattle);

public:

	static UDLLocalPlayerGameplaySubsystem* GetInstanceOnlyLocalPlayer(const UObject* WorldObject)
	{
		const auto LocalPlayer = UGameplayStatics::GetGameInstance(WorldObject)->GetFirstGamePlayer();
		if (LocalPlayer)
		{
			return LocalPlayer->GetSubsystem<UDLLocalPlayerGameplaySubsystem>();
		}
		return nullptr;
	}

private:

	UPROPERTY()
		ADLPlayerStateBase* LocalPlayerState;

	UPROPERTY()
		AActor* EnemyUnitActor = nullptr;

	UPROPERTY()
		bool bIsReady = false;

private:

	FSimpleMulticastDelegate OnGameplayReady;

	FSimpleMulticastDelegate OnPlayerAbilitySysReady;

	bool bInBattleState = false;
};

