// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/IDLPrimaryPlayerAbilitySystem.h"
#include "DLLocalPlayerAbilitySysComp.generated.h"


class UAbilitySystemComponent;


UCLASS()
class DLGAMEPLAYCORE_API UDLLocalPlayerAbilitySysComp
	: public UActorComponent
	, public IDLPrimaryPlayerAbilitySystem
{

	GENERATED_BODY()

public:

	UDLLocalPlayerAbilitySysComp();

	void AbilitySystemReady(UAbilitySystemComponent* InASC);


protected:

	virtual void BeginPlay() override;


public:

	virtual bool FindActiveAbilityDesc(FName AbilityId, FDLAbilityDesc& OutDesc) override;

	virtual void GetAllActiveAbilityDesc(TArray<FDLAbilityDesc>& AbilityDescArray) override;

	virtual FSimpleMulticastDelegate& OnActivateAbilitiesChange() override;

private:


	UPROPERTY()
		UAbilitySystemComponent* ASC;

	FSimpleMulticastDelegate OnActivateAbilitiesChangeDel;
};
