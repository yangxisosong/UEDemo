// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameEngine.h"
#include "DLGameEngine.generated.h"


UCLASS()
class DLAPPFRAMEWORK_API UDLGameEngine
			: public UGameEngine
{
	GENERATED_BODY()
public:



protected:

	// 处理网络错误
	virtual void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
										const FString& ErrorString) override;

	// 处理地图加载错误
	virtual void HandleTravelFailure(UWorld* InWorld, ETravelFailure::Type FailureType, const FString& ErrorString) override;
};
