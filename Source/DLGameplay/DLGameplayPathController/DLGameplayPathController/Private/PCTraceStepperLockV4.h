// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PCTraceStepperBase.h"
#include "PCTraceStepperLockV4.generated.h"

/**
 *
 */
UCLASS()
class DLGAMEPLAYPATHCONTROLLER_API UPCTraceStepperLockV4 : public UPCTraceStepperBase
{
	GENERATED_BODY()
private:
	FVector LastTargetLocation;
	float LastSpeed = 0.0f;

	bool bArrivedTargetRadius = false;
	float ArrivedTargetBaseSpeed = 0.0f;

	bool bArrivedTarget = false;

	bool bNeedTraceFix = false;
	bool bOverTraceFixOverDuration = false;
	int32 IterateCount = 0;
	float OverFixBaseSpeed = 0.0f;

	bool bIsHoming = false;
	float HomingBaseSpeed = 0.0f;
	int32 HomingCount = 0;

	FVector TraceFixTargetLocation = FVector::ZeroVector;
	FVector TraceFixTargetDirection = FVector::ZeroVector;

	int32 TraceFixCount = 0;

	float DefaultSpeed = 0.0f;

	float CurDeltaDistance = 0.0f;

	FVector SplineEndDirection = FVector::ZeroVector;
	bool bIsSplineEnd = false;

	float LastAngle = 0.0f;
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

	// float PredictTraceFixDeltaDistance(FTransform& InNewTrans, FTransform& InPrevTransform);

	bool CheckIsOverFixDuration();

	bool CheckIsSplineEnd();

	void AdvanceInternal(const float DeltaTime, float& DeltaDistance, FVector& OutTraceFixTargetDirection
	, int32& OutTraceFixCount, FVector& OutTraceFixTargetLocation, FTransform& OutTargetTransform);
};
