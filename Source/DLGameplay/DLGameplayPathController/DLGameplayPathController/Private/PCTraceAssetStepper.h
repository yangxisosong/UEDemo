// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PCTraceStepperBase.h"

#include "PCTraceAssetStepper.generated.h"

class UDLPathControllerComponent;

UCLASS(BlueprintType)
class UPCTraceAssetStepperUnlock :public UPCTraceStepperBase
{
	GENERATED_BODY()
protected:
	virtual void OnInitialize(const FPCLaunchParam& InLaunchParam, const FTransform& InSubObjectInitialTransform
		, UDLPathControllerComponent* InPCComp, const bool bEnableDebug) override;

	virtual void OnEvaluate(const float DeltaTime, FTransform& OutTransform) override;


	virtual void OnPostEvaluate(const float DeltaTime) override;


	virtual void Advance(const float DeltaMoveTime) override;

	virtual bool CheckIsOverDuration() override;
protected:
	float DefaultSpeed = 0.0f;

	FRotator BaseRotation = FRotator::ZeroRotator;
};

UCLASS(BlueprintType)
class UPCTraceAssetStepperToLocation :public UPCTraceAssetStepperUnlock
{
	GENERATED_BODY()
protected:
	virtual void OnInitialize(const FPCLaunchParam& InLaunchParam, const FTransform& InSubObjectInitialTransform
		, UDLPathControllerComponent* InPCComp, const bool bEnableDebug) override;

	virtual void OnPostEvaluate(const float DeltaTime) override;
};
