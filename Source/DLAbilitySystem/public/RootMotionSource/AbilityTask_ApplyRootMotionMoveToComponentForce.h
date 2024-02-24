// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToActorForce.h"
#include "AbilityTask_ApplyRootMotionMoveToComponentForce.generated.h"



UCLASS()
class DLABILITYSYSTEM_API UAbilityTask_ApplyRootMotionMoveToComponentForce : public UAbilityTask_ApplyRootMotionMoveToActorForce
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_ApplyRootMotionMoveToComponentForce* ApplyRootMotionMoveToComponentForce(UGameplayAbility* OwningAbilityv2, FName TaskInstanceNamev2, USceneComponent* TargetComponentv2, FVector TargetLocationOffsetv2, ERootMotionMoveToActorTargetOffsetType OffsetAlignmentv2, float Durationv2, UCurveFloat* TargetLerpSpeedHorizontalv2, UCurveFloat* TargetLerpSpeedVerticalv2, bool bSetNewMovementModev2, EMovementMode MovementModev2, bool bRestrictSpeedToExpectedv2, UCurveVector* PathOffsetCurvev2, UCurveFloat* TimeMappingCurvev2, ERootMotionFinishVelocityMode VelocityOnFinishModev2, FVector SetVelocityOnFinishv2, float ClampVelocityOnFinishv2, bool bDisableDestinationReachedInterruptv2);

	bool UpdateTargetLocationv2(float DeltaTime);

	FVector CalculateTargetOffsetv2() const;
	
	virtual void TickTask(float DeltaTime) override;
	virtual void SharedInitAndApply() override;
	
	UPROPERTY(Replicated)
	USceneComponent* TargetComponent;
};
