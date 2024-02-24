// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLPlayerSetupInfo.h"
#include "GameplayFramwork/DLPlayerControllerBase.h"

#include "DLAbilityTestPlayerController.generated.h"


UCLASS()
class DLABILITYTEST_API ADLAbilityTestPlayerController : public ADLPlayerControllerBase
{
	GENERATED_BODY()

public:

	ADLAbilityTestPlayerController();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

protected:

	virtual bool ReadPlayerSetupInfo(FDLPlayerSetupInfo& OutSetup) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		FDLPlayerSetupInfo PlayerSetupInfo;
};
