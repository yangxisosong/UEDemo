// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PCTraceStepperBase.h"
#include "PCTraceStepperFreeTrack.generated.h"

/**
 *
 */
UCLASS()
class DLGAMEPLAYPATHCONTROLLER_API UPCTraceStepperFreeTrack : public UPCTraceStepperBase
{
	GENERATED_BODY()
private:
	FVector LastTargetLocation;
	float LastSpeed = 0.0f;

	bool bArrivedTargetRadius = false;
	float ArrivedTargetBaseSpeed = 0.0f;

	bool bArrivedTarget = false;

	bool bNeedTraceFix = true;
	int32 IterateCount = 0;
	float OverFixBaseSpeed = 0.0f;

	bool bIsHoming = false;
	float HomingBaseSpeed = 0.0f;
	int32 HomingCount = 0;

	FVector TraceFixTargetLocation = FVector::ZeroVector;
	FVector TraceFixTargetDirection = FVector::ZeroVector;

	int32 TraceFixCount = 0;

protected:
	virtual void OnInitialize(const FPCLaunchParam& InLaunchParam, const FTransform& InSubObjectInitialTransform
		, UDLPathControllerComponent* InPCComp, const bool bEnableDebug) override;

	virtual void OnEvaluate(const float DeltaTime, FTransform& OutTransform) override;

	virtual void OnPostEvaluate(const float DeltaTime) override;

	virtual void Advance(const float DeltaMoveTime) override;

	virtual bool CheckIsOverDuration() override;
private:
	bool CheckIsArriveTargetRadius();

	bool CheckShouldHoming();

	bool CheckIsNeedFix();
};
