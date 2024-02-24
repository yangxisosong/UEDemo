// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PathControllerDef.h"

#include "PCTraceStepperBase.generated.h"

class UDLPathControllerComponent;

#define TARGET_ARRIVE_BIAS 8.0f

/**
 *
 */
UCLASS(Abstract, Blueprintable)
class DLGAMEPLAYPATHCONTROLLER_API UPCTraceStepperBase : public UObject
{
	GENERATED_BODY()
public:
	void Initialize(const FPCLaunchParam& InLaunchParam, const FTransform& InSubObjectInitialTransform
	, UDLPathControllerComponent* InPCComp, const bool bEnableDebug = false);

	void Evaluate(const float DeltaTime, FTransform& OutTransform);

	void PostEvaluate(const float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetTimePassed() const { return TimePassed; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetDistanceFlew() const { return Distance; }

protected:
	virtual void OnInitialize(const FPCLaunchParam& InLaunchParam, const FTransform& InSubObjectInitialTransform
	, UDLPathControllerComponent* InPCComp, const bool bEnableDebug = false) {}

	UFUNCTION(BlueprintImplementableEvent)
		void OnInitialize_BP(const FPCLaunchParam& InLaunchParam, const FTransform& InSubObjectInitialTransform
		   , UDLPathControllerComponent* InPCComp, const bool bEnableDebug = false);

	virtual void OnEvaluate(const float DeltaTime, FTransform& OutTransform) {}

	UFUNCTION(BlueprintImplementableEvent)
		void OnEvaluate_BP(const float DeltaTime, FTransform& OutTransform);

	virtual void OnPostEvaluate(const float DeltaTime) {}

	UFUNCTION(BlueprintImplementableEvent)
		void OnPostEvaluate_BP(const float DeltaTime);

	UFUNCTION(BlueprintCallable)
		bool IterateFrequencyOptimize(const float DeltaSeconds, float& DeltaMoveTime);

	virtual void Advance(const float DeltaMoveTime);

	void ApplyGravity(const float DeltaSeconds, FTransform& Transform);

	virtual bool CheckIsOverDuration() { PURE_VIRTUAL(UPCTraceStepperBase::CheckIsOverDuration)  return false; }

	UFUNCTION(BlueprintCallable)
		void FinishTrace();

	UFUNCTION(BlueprintPure)
		float CheckDeltaSeconds(float DeltaSeconds) const;

protected:
	UPROPERTY(BlueprintReadWrite)
		float IterateTime = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		float CurDeltaMove = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		FPCLaunchParam LaunchParam;

	UPROPERTY(BlueprintReadWrite)
		UDLPathControllerComponent* PCComp = nullptr;

	UPROPERTY(BlueprintReadWrite)
		FTransform SubObjectInitialTransform;

	/**
	 * @brief 世界坐标系下的子物体的目标Transform
	 */
	UPROPERTY(BlueprintReadWrite)
		FTransform TargetTransform;

	/**
	 * @brief 世界坐标系下的上次Advance时子物体的Transform
	 */
	UPROPERTY(BlueprintReadWrite)
		FTransform LastTransform;

	UPROPERTY(BlueprintReadWrite)
		FTransform LastEvaluateTransform;

	UPROPERTY(BlueprintReadWrite)
		float TimePassed = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		float Distance = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		TArray<float> PointMarkers;

	UPROPERTY(BlueprintReadWrite)
		int32 CurMarkIndex = 0;

	UPROPERTY(BlueprintReadWrite)
		float PassedGravityDistance;

	UPROPERTY(BlueprintReadWrite)
		FVector DirectionWhenOverDuration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
		FVector DirectionWhenOverFixDuration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
		bool bIsOverDuration = false;
protected:
	int32 EvaluateCount = 0;
	int32 FrameCount = 0;
};
