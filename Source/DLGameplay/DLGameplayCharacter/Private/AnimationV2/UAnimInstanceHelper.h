// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLAnimCommonDef.h"

class UDLAnimInstanceBaseV2;
class UDLCharacterMovementComponentBase;
struct FAnimGraphInAirValues;
struct FAnimGraphGroundedValues;

class DLAnimInstanceHelper
{
public:
	static void CalculateVelocityBlend(const UDLAnimInstanceBaseV2* AnimIns, FAnimVelocityBlend& VelocityBlend, const FRotator& CharacterActorRotation, const FVector& Velocity);

	static FVector CalculateRelativeAccelerationAmount(const UDLAnimInstanceBaseV2* AnimIns, const FRotator& CharacterActorRotation, const FVector& Velocity
									, const FVector& Acceleration);

	static float CalculateSpeedBlend(const UDLAnimInstanceBaseV2* AnimIns, FVector2D& TargetWalkRushBlend, const FAnimGaitType& Gait
									, const FAnimVelocityBlend& InVelocityBlend, const FAnimCharacterMainState& CharacterMainState);

	static float CalculateStrideBlend(const UDLAnimInstanceBaseV2* AnimIns, const float Speed, const FAnimBlendCurves& BlendCurveConfig);

	static float CalculateStandingPlayRate(const float Speed, const float AnimatedWalkSpeed, const float AnimatedJogSpeed, const float AnimatedSprintSpeed
		, const UDLAnimInstanceBaseV2* AnimIns, const FAnimGaitType& Gait, const FAnimGraphGroundedValues& GroundedValues);
};
