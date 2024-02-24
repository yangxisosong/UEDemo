// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_SimulateRootMotion.h"

#include "Curves/CurveVector.h"

void UANS_SimulateRootMotion::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
										  float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	TimePassed = 0.0f;
	InitialLocation = MeshComp->GetRelativeLocation();
}

void UANS_SimulateRootMotion::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	TimePassed += FrameDeltaTime;
	if (RootMotionCurve)
	{
		MeshComp->SetRelativeLocation(InitialLocation + RootMotionCurve->GetVectorValue(TimePassed));
	}
}

void UANS_SimulateRootMotion::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	TimePassed = 0.0f;
	MeshComp->SetRelativeLocation(InitialLocation);
}
