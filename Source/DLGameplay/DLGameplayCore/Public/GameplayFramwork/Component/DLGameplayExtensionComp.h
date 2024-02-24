// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DLGameplayExtensionComp.generated.h"


UCLASS()
class DLGAMEPLAYCORE_API UDLGameplayExtensionComp : public UActorComponent
{
	GENERATED_BODY()

public:
	UDLGameplayExtensionComp();




protected:

	virtual void BeginPlay() override;
};
