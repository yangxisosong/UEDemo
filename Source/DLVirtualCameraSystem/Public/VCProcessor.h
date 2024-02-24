// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VCDataAssetDef.h"
#include "VCProcessor.generated.h"



USTRUCT(BlueprintType)
struct FProcessorEnv
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UObject* EnvDataObject = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurveCurrentTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DeltaTime = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsForward = true;
};

UCLASS(Abstract, BlueprintType, Blueprintable)
class DLVIRTUALCAMERASYSTEM_API UVCProcessor : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintImplementableEvent)
		void BeginProcess(const UObject* ProcessorEnv);
	
	UFUNCTION(BlueprintImplementableEvent)
		void TryFixVCamData(const FTransform& ActorLocalToWorld, const FTransform& ArmLocalToWorld,
							const FProcessorEnv ProcessorEnv,float& CurrentTime,float& MaxTime, FVCCamBlendData& OutCamData);

	UFUNCTION(BlueprintImplementableEvent)
		void EndProcess(const UObject* ProcessorEnv);
};
