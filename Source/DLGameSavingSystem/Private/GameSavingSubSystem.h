// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDLGameSavingSystem.h"
#include "IDLPlayerSavingAccessor.h"

#include "GameSavingSubSystem.generated.h"


class IDLPlayerSavingAccessor;

UCLASS(Config = Game)
class UGameSavingSubSystem
	: public UWorldSubsystem
	, public IDLGameSavingSystem
{
	GENERATED_BODY()

public:

	UGameSavingSubSystem();


	virtual IDLPlayerSavingAccessor* GetPlayerSavingAccessor() override;

	virtual IDLNPCSavingAccessor* GetNPCSavingAccessor() override;

private:

	UPROPERTY(EditAnywhere, Config, Meta = (MustImplement = DLPlayerSavingAccessor))
		TSoftClassPtr<UObject> PlayerSavingClass;

	UPROPERTY()
		UObject* PlayerSaving = nullptr;

	UPROPERTY()
		UObject* NPCSaving = nullptr;
};
