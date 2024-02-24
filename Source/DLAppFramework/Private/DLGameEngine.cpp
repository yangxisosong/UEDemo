// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameEngine.h"

void UDLGameEngine::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	const FString& ErrorString)
{
	Super::HandleNetworkFailure(World, NetDriver, FailureType, ErrorString);
}

void UDLGameEngine::HandleTravelFailure(UWorld* InWorld, ETravelFailure::Type FailureType, const FString& ErrorString)
{
	Super::HandleTravelFailure(InWorld, FailureType, ErrorString);
}
