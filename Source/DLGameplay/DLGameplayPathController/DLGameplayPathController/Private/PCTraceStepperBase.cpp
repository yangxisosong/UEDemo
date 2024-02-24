// Fill out your copyright notice in the Description page of Project Settings.


#include "PCTraceStepperBase.h"

#include "DLPathControllerComponent.h"
#include "PathControllerTraceAsset.h"

DECLARE_STATS_GROUP(TEXT("Path Controller"), STATGROUP_PathController, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("PCStepper_Initialize"), STAT_PCStepper_Initialize, STATGROUP_PathController);
DECLARE_CYCLE_STAT(TEXT("PCStepper_Evaluate"), STAT_PCStepper_Evaluate, STATGROUP_PathController);
DECLARE_CYCLE_STAT(TEXT("PCStepper_PostEvaluate"), STAT_PCStepper_PostEvaluate, STATGROUP_PathController);
DECLARE_CYCLE_STAT(TEXT("PCStepper_Advance"), STAT_PCStepper_Advance, STATGROUP_PathController);


void UPCTraceStepperBase::Initialize(const FPCLaunchParam& InLaunchParam, const FTransform& InSubObjectInitialTransform,
	UDLPathControllerComponent* InPCComp, const bool bEnableDebug)
{
	SCOPE_CYCLE_COUNTER(STAT_PCStepper_Initialize);
	LaunchParam = InLaunchParam;
	PCComp = InPCComp;
	SubObjectInitialTransform = InSubObjectInitialTransform;
	if (!ensureMsgf(LaunchParam.Asset->GetNumberOfSplinePoints() > 1, TEXT("为什么你的轨迹上只有一个点！！！")))
	{
		return;
	}
	OnInitialize(InLaunchParam, InSubObjectInitialTransform, InPCComp, bEnableDebug);
	OnInitialize_BP(InLaunchParam, InSubObjectInitialTransform, InPCComp, bEnableDebug);
}

void UPCTraceStepperBase::Evaluate(const float DeltaTime, FTransform& OutTransform)
{
	SCOPE_CYCLE_COUNTER(STAT_PCStepper_Evaluate);
	DL_PATH_CONTROLLER_LOG(Log, TEXT("<UPCTraceStepperBase::Evaluate> DeltaTime:%f"), DeltaTime);
	OnEvaluate(DeltaTime, OutTransform);
	OnEvaluate_BP(DeltaTime, OutTransform);
}

void UPCTraceStepperBase::PostEvaluate(const float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_PCStepper_PostEvaluate);
	EvaluateCount++;
	OnPostEvaluate(DeltaTime);
	OnPostEvaluate_BP(DeltaTime);
}

bool UPCTraceStepperBase::IterateFrequencyOptimize(const float DeltaSeconds, float& DeltaMoveTime)
{

	FrameCount++;

	if (FrameCount <= LaunchParam.Asset->IterateFrequency && IterateTime <= CurDeltaMove)
	{
		IterateTime += DeltaSeconds;
		return true;
	}

	IterateTime = DeltaSeconds;
	DeltaMoveTime = IterateTime * LaunchParam.Asset->IterateFrequency;
	FrameCount = 1;
	return false;
}

void UPCTraceStepperBase::Advance(const float DeltaMoveTime)
{
	SCOPE_CYCLE_COUNTER(STAT_PCStepper_Advance);

	CurDeltaMove = DeltaMoveTime;
}

void UPCTraceStepperBase::ApplyGravity(const float DeltaSeconds, FTransform& Transform)
{
	if (FMath::IsNearlyZero(LaunchParam.Asset->GravityAcceleration))
	{
		return;
	}
	PassedGravityDistance += LaunchParam.Asset->GravityAcceleration * DeltaSeconds;
	const FVector Gravity{ 0.0f, 0.0f, PassedGravityDistance };
	Transform.SetLocation(Transform.GetLocation() - Gravity);
}

void UPCTraceStepperBase::FinishTrace()
{
	if (!ensureAlwaysMsgf(PCComp && LaunchParam.Asset, TEXT("PCComp or LaunchParam.Asset is invalid!!!")))
	{
		return;
	}
	PCComp->TrackComplete(LaunchParam.Asset);
}

float UPCTraceStepperBase::CheckDeltaSeconds(float DeltaSeconds) const
{
	if (FMath::IsNearlyZero(DeltaSeconds))
	{
#if WITH_EDITOR
		DeltaSeconds = PCComp->GetPreviewDeltaTime();
#else
		PCComp->GetWorld()->GetDeltaSeconds();
#endif
	}
	return DeltaSeconds;
}
