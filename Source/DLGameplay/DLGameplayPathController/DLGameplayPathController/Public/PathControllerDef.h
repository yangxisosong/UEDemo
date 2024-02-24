// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PathControllerDef.generated.h"

class UPathControllerTraceAsset;
DECLARE_LOG_CATEGORY_EXTERN(LogDL_PathController, Log, All);

static TAutoConsoleVariable<bool> CVarPathControllerDebug(
	TEXT("SubObject.PathController.Debug"),
	false,
	TEXT("子物体飞行路径调试"),
	ECVF_Cheat
);

#define IS_PATH_CONTROLLER_LOG_ENABLED() CVarPathControllerDebug.GetValueOnGameThread()

#define DL_PATH_CONTROLLER_LOG(Verbosity, Format, ...) \
{\
	if(IS_PATH_CONTROLLER_LOG_ENABLED()) \
	{\
		UE_LOG(LogDL_PathController, Verbosity, Format, __VA_ARGS__);\
	}\
}

#define DL_PATH_CONTROLLER_LOG_WITHOUT_VALUE(Verbosity, Format) \
{\
	if(IS_PATH_CONTROLLER_LOG_ENABLED()) \
	{\
		UE_LOG(LogDL_PathController, Verbosity, Format);\
	}\
}

USTRUCT(BlueprintType)
struct FPCLaunchParam
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		UPathControllerTraceAsset* Asset = nullptr;

	UPROPERTY(BlueprintReadWrite)
		TWeakObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
		FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
		bool bIsTrack = false;

	UPROPERTY(BlueprintReadWrite)
		FRotator RelativeRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite)
		bool bIsHoming = false;

	UPROPERTY(BlueprintReadWrite)
		float PersistTime = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		FVector InitialDirection = FVector::ZeroVector;
};


