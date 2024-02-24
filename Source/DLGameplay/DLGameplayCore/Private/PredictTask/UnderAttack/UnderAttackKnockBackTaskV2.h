// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PredictTask/UnderAttack/UnderAttackPredictTask.h"
#include "UnderAttackKnockBackTaskV2.generated.h"

/**
 *
 */
UCLASS()
class DLGAMEPLAYCORE_API UUnderAttackKnockBackTaskV2 : public UDLUnderAttackTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		void BeginMove(const FVector TargetLocation, const float Duration, const int32 Priority);

	UFUNCTION(BlueprintImplementableEvent)
		void EndMove();

	virtual void EndTask(const bool Cancel) override;

	virtual void Tick(float Dt) override;

protected:
	TSharedPtr<FRootMotionSource_MoveToForce> RootMotion;
	uint16 RootMotionId = static_cast<uint16>(ERootMotionSourceID::Invalid);
};
