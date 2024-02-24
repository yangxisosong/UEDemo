// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "ICharacterWeapon.generated.h"



UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UCharacterWeapon : public UInterface
{
	GENERATED_BODY()
};

class DLGAMEPLAYTYPES_API ICharacterWeapon
	: public IInterface
{
	GENERATED_BODY()

public:

	// 获取当前的武器
	UFUNCTION(BlueprintCallable)
	virtual TArray<AActor*> GetCurrentWeapons() = 0;

	UFUNCTION(BlueprintCallable)
	virtual AActor* GetWeaponWithAttachSocketTag(FGameplayTag SocketTag) = 0;

};
