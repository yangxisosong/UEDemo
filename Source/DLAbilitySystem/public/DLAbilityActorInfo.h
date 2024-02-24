// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "UObject/Object.h"
#include "DLAbilityActorInfo.generated.h"

class IDLCharacterController;
/**
 * 
 */
USTRUCT()
struct DLABILITYSYSTEM_API FDLAbilityActorInfo
		: public FGameplayAbilityActorInfo
{
	GENERATED_BODY()
public:

	virtual void InitFromActor(AActor *InOwnerActor, AActor *InAvatarActor, UAbilitySystemComponent* InAbilitySystemComponent) override;

public:

	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<UObject> CharacterController;

public:

	IDLCharacterController* GetCharacterController() const;
};
