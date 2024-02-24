// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayFramwork/DLPlayerStateBase.h"
#include "UObject/Object.h"
#include "DLPlayerStateExtensionComp.generated.h"


USTRUCT()
struct FPlayerInitData
{
	GENERATED_BODY()
public:

	UPROPERTY()
		class UDLCharacterAsset* CharacterAsset;

	UPROPERTY()
		FCharacterInfoBase InfoBase;

};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGeneralPSExtentEvent, ADLPlayerStateBase*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPlayerInitDataReady, ADLPlayerStateBase*, const FPlayerInitData&);


UCLASS()
class DLGAMEPLAYCORE_API UDLPlayerStateExtensionComp : public UActorComponent
{
	GENERATED_BODY()

public:

	void OnPlayerBaseInfoSet_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate Delegate);

	void OnPlayerBaseInfoSet_Unregister(const UObject* BindObject);

	void OnPlayerInitDataReady_CallAndRegister(FOnPlayerInitDataReady::FDelegate Delegate);

	void OnPlayerInitDataReady_Unregister(const UObject* BindObject);

	void OnPlayerPawnInitReady_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate Delegate);

	void OnPlayerPawnInitReady_Unregister(const UObject* BindObject);

	void OnPlayerAbilitySystemInit_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate Delegate);

	void OnPlayerAbilitySystemInit_Unregister(const UObject* BindObject);

	void OnPlayerPawnRemove_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate Delegate);

	void OnPlayerPawnRemove_Unregister(const UObject* BindObject);


private:

	bool CheckPlayerInitDataReady() const;

	void TryBroadcastPlayerInitDataReady() const;

public:

	void LoadPlayerAssetComplate(class UDLCharacterAsset* CharacterAsset);

	void PlayerBaseInfoReady(const FCharacterInfoBase& InfoBase);

	void PlayerPawnInitReady();

	void PlayerAbilitySystemInit();

	void PlayerPawnRemove();

protected:

	ADLPlayerStateBase* GetPlayerStateBase() const;

private:

	bool bLoadPlayerAssetComplate = false;
	bool bPlayerBaseInfoReady = false;
	bool bPlayerPawnInitReady = false;
	bool bPlayerAbilitySystemInit = false;
	bool bPlayerPawnRemove = true;

	FOnPlayerInitDataReady OnPlayerInitDataReady;

	FOnGeneralPSExtentEvent OnPlayerPawnInitReady;
	FOnGeneralPSExtentEvent OnPlayerBaseInfoSet;
	FOnGeneralPSExtentEvent OnPlayerPawnRemove;

	FOnGeneralPSExtentEvent OnAbilitySystemInit;

	UPROPERTY(Transient)
	FPlayerInitData InitData;
};
