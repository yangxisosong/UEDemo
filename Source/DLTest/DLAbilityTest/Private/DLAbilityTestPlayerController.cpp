// Fill out your copyright notice in the Description page of Project Settings.


#include "DLAbilityTestPlayerController.h"


// Sets default values
ADLAbilityTestPlayerController::ADLAbilityTestPlayerController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADLAbilityTestPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADLAbilityTestPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ADLAbilityTestPlayerController::ReadPlayerSetupInfo(FDLPlayerSetupInfo& OutSetup)
{
	OutSetup = PlayerSetupInfo;
	return true;
}

