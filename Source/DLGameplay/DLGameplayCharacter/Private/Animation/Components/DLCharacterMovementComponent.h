// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimDef/DLAnimStruct.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DLCharacterMovementComponent.generated.h"


class AAnimationTestCharacter;
UCLASS()
class UDLCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()
public:

	virtual void BeginPlay() override;

	virtual void PhysWalking(float deltaTime, int32 Iterations) override;
	
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	virtual float GetMaxAcceleration() const override;

	virtual float GetMaxBrakingDeceleration() const override;


	virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
public:

	float GetMappedSpeed() const;

	void SetAllowedGait(const EGaitType NewGait);

	void SetCurrentMovementSettings(const FMovementSettings& NewMovementSettings);

private:
	void PredictStop(const float DeltaTime);

	void PredictStopAnimPos(const float DeltaTime, FVector& OutStopPosition, float& OutStopDis) const;

	void UpdateCharacterLocation(const float DeltaTime);

	UCurveFloat* GetStopCurve() const;

	EMovementDirection GetMovementDirection() const;

protected:
	UFUNCTION(BlueprintPure)
		AAnimationTestCharacter* GetDLCharacterOwner() const;

public:
	UPROPERTY(BlueprintReadOnly)
		EGaitType AllowedGait = EGaitType::Walking;

	UPROPERTY(BlueprintReadOnly, Category = "Character | Movement")
		FMovementSettings CurrentMovementSettings;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float StepLength = 1.0f;

private:
	bool bStopping = false;
	bool bStopped = true;
	FVector TargetStopPosition = FVector::ZeroVector;
};
