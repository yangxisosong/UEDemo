// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/AITask_MoveTo.h"
#include "DLAITask_MoveTo.generated.h"

/**
 * 
 */
UCLASS()
class UDLAITask_MoveTo : public UAITask_MoveTo
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "AI|Tasks", meta = (AdvancedDisplay = "AcceptanceRadius,StopOnOverlap,AcceptPartialPath,bUsePathfinding,bUseContinuosGoalTracking,ProjectGoalOnNavigation", DefaultToSelf = "Controller", BlueprintInternalUseOnly = "TRUE", DisplayName = "DL AI Move To"))
static UDLAITask_MoveTo* DLAIMoveTo(AAIController* Controller, FVector GoalLocation, AActor* GoalActor = nullptr,
	float AcceptanceRadius = -1.f, EAIOptionFlag::Type StopOnOverlap = EAIOptionFlag::Default, EAIOptionFlag::Type AcceptPartialPath = EAIOptionFlag::Default,
	bool bUsePathfinding = true, bool bLockAILogic = true, bool bUseContinuosGoalTracking = false, EAIOptionFlag::Type ProjectGoalOnNavigation = EAIOptionFlag::Default);

	UFUNCTION(BlueprintCallable, Category = "AI|Tasks")
	void AbortTask();

};
