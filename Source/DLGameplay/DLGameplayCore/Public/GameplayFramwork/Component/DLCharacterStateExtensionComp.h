// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DLCharacterStateExtensionComp.generated.h"


UCLASS()
class DLGAMEPLAYCORE_API UDLCharacterStateExtensionComp
	: public UActorComponent
{
	GENERATED_BODY()

public:

	UDLCharacterStateExtensionComp();

	void OnDied_CallAndRegister(const FSimpleMulticastDelegate::FDelegate& Delegate);

	void OnDied_Unregister(const UObject* BindObj);

	void ServerSetCharacterDied();

	void ServerSetup();

	bool IsDied() const;

protected:

	virtual void BeginPlay() override;

private:

	void CharacterDiedBroadcast();

protected:

	UFUNCTION()
		void OnRep_IsDied();

private:

	UPROPERTY(ReplicatedUsing = OnRep_IsDied)
		bool bIsDied = false;

private:

	FSimpleMulticastDelegate OnDied;
};
