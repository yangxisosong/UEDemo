// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_SimulateRootMotion.generated.h"

class UCurveVector;
/**
 *
 */
UCLASS()
class DLGAMEPLAYCHARACTER_API UANS_SimulateRootMotion : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere)
		UCurveVector* RootMotionCurve = nullptr;

private:
	float TimePassed = 0.0f;
	FVector InitialLocation = FVector::ZeroVector;
};
