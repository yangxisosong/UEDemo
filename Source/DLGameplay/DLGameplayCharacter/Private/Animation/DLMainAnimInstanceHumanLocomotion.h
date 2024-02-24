// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLMainAnimInstance.h"
#include "DLGameplayCharacter/Public/Animation/AnimDef/DLAnimEnum.h"
#include "DLGameplayCharacter/Public/Animation/AnimDef/DLAnimStruct.h"
#include "DLMainAnimInstanceHumanLocomotion.generated.h"

enum class EDLAnimCurveName : uint8;
struct FDynamicMontageParams;
/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class UDLMainAnimInstanceHumanLocomotion : public UDLMainAnimInstance
{
	GENERATED_BODY()

private:
		friend class UDLHumanAnimInsHelper;
public:
#pragma region override animins method

	virtual void OnUpdateAnimation(float DeltaSeconds) override;
#pragma endregion override animins method

protected:
#pragma region logic handle
	virtual void UpdateAimingValues(float DeltaSeconds);

	virtual void UpdateLayerValues();

	virtual void UpdateMovementValues(float DeltaSeconds);

	virtual void UpdateRotationValues();

	virtual bool CanRotateInPlace();

	virtual void RotateInPlaceCheck();

	virtual bool CanTurnInPlace();

	virtual void TurnInPlaceCheck(float DeltaSeconds);

	void TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool OverrideCurrent);

	virtual bool CanDynamicTransition();

	virtual void DynamicTransitionCheck();

	virtual void UpdateInAirValues(float DeltaSeconds);

	virtual void UpdateRagRollValues();

	virtual bool ShouldMoveCheck();

	FVelocityBlend CalculateVelocityBlend();

	FVector CalculateRelativeAccelerationAmount();

	float CalculateStrideBlend();

	float CalculateWalkRunBlend();

	float CalculateStandingPlayRate();

	float CalculateDiagonalScaleAmount();

	float CalculateLandPrediction();

	FLeanAmount CalculateAirLeanAmount();

	EMovementDirection CalculateMovementDirection();

	float GetAnimCurveClamped(const EDLAnimCurveName Name, float Bias, float ClampMin, float ClampMax) const;

	bool GetIsWalking() const;

	bool GetIsJogging() const;

	bool GetIsSprinting() const;

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation | Logic")
		void PlayTransition(const FDynamicMontageParams& Params);
	virtual void PlayTransition_Implementation(const FDynamicMontageParams& Params);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation | Logic")
		void PlayTransitionChecked(const FDynamicMontageParams& Params);
	virtual void PlayTransitionChecked_Implementation(const FDynamicMontageParams& Params);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation | Logic")
		void PlayDynamicTransition(float ReTriggerDelay, FDynamicMontageParams Params);
	virtual void PlayDynamicTransition_Implementation(float ReTriggerDelay, FDynamicMontageParams Params);

	UFUNCTION(BlueprintCallable, Category = "Animation | Logic")
		void SetOverlayOverrideState(int32 OverlayOverrideState)
	{
		LayerBlendingValues.OverlayOverrideState = OverlayOverrideState;
	}

	UFUNCTION(BlueprintCallable, Category = "Animation | Logic")
		void SetTrackedHipsDirection(EHipsDirection HipsDirection)
	{
		GroundedValues.TrackedHipsDirection = HipsDirection;
	}
#pragma endregion logic handle

	void PlayDynamicTransitionDelay();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Logic Values")
		FAnimGraphGrounded GroundedValues;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Logic Values")
		FAnimTurnInPlace TurnInPlaceValues;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Logic Values")
		FAnimGraphLayerBlending LayerBlendingValues;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Logic Values")
		FAnimGraphFootIK FootIKValues;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Logic Values")
		FVelocityBlend VelocityBlend;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Logic Values")
		FVector RelativeAccelerationAmount = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Logic Values")
		FLeanAmount LeanAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Logic Values")
		FMovementDirection MovementDirection = EMovementDirection::Forward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Logic Values")
		FAnimGraphAimingValues AimingValues;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Logic Values")
		FAnimRotateInPlace RotateInPlaceValues;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Logic Values")
		FAnimGraphInAir InAirValues;

	/** Ragdoll */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Logic Values")
		float FlailRate = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Logic Values")
		FGroundedEntryState GroundedEntryState = EGroundedEntryState::None;

protected:

	FTimerHandle OnPivotTimer;

	FTimerHandle PlayDynamicTransitionTimer;

	bool bCanPlayDynamicTransition = true;

	friend class DLAnimHumanIKHelper;
};
