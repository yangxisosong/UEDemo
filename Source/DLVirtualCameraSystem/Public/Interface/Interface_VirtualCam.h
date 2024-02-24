// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VCDataAssetDef.h"
#include "UObject/Interface.h"
#include "Interface_VirtualCam.generated.h"


UINTERFACE(MinimalAPI)
class UMyInterface_CameraData : public UInterface
{
	GENERATED_BODY()
};

class IMyInterface_CameraData
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	void TryFixVCamData(const FTransform& ActorLocalToWorld, const FTransform& ArmLocalToWorld,
	                    const FTransform& LockTargetTransform,float DeltaTime,FVCCamBlendData& OutCamData);

	UFUNCTION(BlueprintNativeEvent, meta = (BlueprintInternalUseOnly))
	void RegisterArmComponent(UActorComponent* Comp);

	UFUNCTION(BlueprintNativeEvent, meta = (BlueprintInternalUseOnly))
	int32 GetOrder();
};
