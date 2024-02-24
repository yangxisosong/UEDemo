// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayFramwork/DLCharacterBase.h"
#include "DLPlayerLockUnitLogic.generated.h"

struct FDLUnitUnderAttackInfo;
class ADLPlayerCharacter;
class UDLLockPointComponent;
/**
 *
 */
UCLASS(Blueprintable)
class DLGAMEPLAYCHARACTER_API UDLPlayerLockUnitLogic
	: public UObject
{
	GENERATED_BODY()

public:

	void Init(class ADLPlayerController* PC);

	void Tick(float Dt);

	UDLLockPointComponent* FindBastLockPoint(const TArray<AActor*>& AlternativeActor);

protected:

	virtual UWorld* GetWorld() const override;

public:

	UFUNCTION(BlueprintCallable)
		bool TryLockUnit();

	UFUNCTION(BlueprintCallable)
		bool TryLockNextUnit();

	UFUNCTION(BlueprintCallable)
		void ForceLockUnit(UDLLockPointComponent* Comp);

	UFUNCTION(BlueprintCallable)
		void UnLockUnit();

	UFUNCTION(BlueprintImplementableEvent)
		bool FilterUnLockPoint(ADLPlayerController* MyPC, ADLPlayerCharacter* MyCharacter, UDLLockPointComponent* LockPoint);

	UFUNCTION(BlueprintImplementableEvent)
		bool CheckLockPoint(ADLPlayerController* MyPC, ADLPlayerCharacter* MyCharacter, UDLLockPointComponent* Point);

protected:

	UFUNCTION() 
		void OnUnderAttack(const FDLUnitUnderAttackInfo& Info);
protected:

	UPROPERTY(EditDefaultsOnly)
		FVector2D LockRectInSceneMin;

	UPROPERTY(EditDefaultsOnly)
		FVector2D LockRectInSceneMax;

	UPROPERTY()
		class ADLPlayerController* PlayerController;

	UPROPERTY()
		bool bInit = false;

	UPROPERTY()
		UDLLockPointComponent* LockedPoint;
};
