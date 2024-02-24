// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLAnimComponentBase.h"
#include "DLAnimComponentHumanLocomotion.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRagdollStateChangedSignature, bool, bRagdollState);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UDLAnimComponentHumanLocomotion : public UDLAnimComponentBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDLAnimComponentHumanLocomotion();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame by character.
	virtual void OnTick(float DeltaTime) override;

public:
	virtual void OnLanded(const FHitResult& Hit) override;

	void OnReceivedFrameFrozenStart( UAnimSequence* TargetSeq, const float StartTime, const float EndTime,const FName SocketName);

	void OnReceivedFrameFrozenEnd();

protected:
	virtual void SetEssentialValues(float DeltaTime);

	void UpdateCharacterMovement();

	virtual void UpdateGroundedRotation(float DeltaTime);

	virtual void UpdateInAirRotation(float DeltaTime);

	virtual void UpdateRagdoll(float DeltaTime);

	virtual void SetActorLocationDuringRagdoll(float DeltaTime);

	virtual void RagdollStart();

	virtual void RagdollEnd();
protected:
	void SetSpeed(float NewSpeed);

	void SetIsMoving(bool bNewIsMoving);

	void SetMovementInputAmount(float NewMovementInputAmount);

	void SetHasMovementInput(bool bNewHasMovementInput);

	void SetAcceleration(const FVector& NewAcceleration);

	void SetGait(EGaitType NewGait);

	float CalculateGroundedRotationRate();

	void SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed, float DeltaTime);

	void LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed, float DeltaTime);

	EGaitType GetAllowedGait();

	EGaitType GetActualGait(EGaitType AllowedGait);

	void SetDesiredGait(EGaitType NewGait);

	void SetActorLocationAndTargetRotation(FVector NewLocation, FRotator NewRotation);

	void SetMovementState(const EMovementState NewState);

	void EventOnLanded();

	void OnLandRoll() const;

	void OnLandFrictionReset();
protected:
	virtual void OnMovementStateChanged(EMovementState PreviousState);

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

public:
	FVector GetMovementInput() const;

public:
	UPROPERTY(BlueprintAssignable, Category = "Movement")
		FRagdollStateChangedSignature RagdollStateChangedDelegate;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		FRotator TargetRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		FRotator InAirRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		FRotator LastVelocityRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		FRotator LastMovementInputRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		bool bRagdollOnGround = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		bool bRagdollFaceUp = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		FVector LastRagdollVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		FVector TargetRagdollLocation = FVector::ZeroVector;

	// 如果skeleton使用的是旋转后的pelvis骨骼，反转计算
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
		bool bReversedPelvis = false;

	FVector PreviousVelocity = FVector::ZeroVector;

	float PreviousAimYaw = 0.0f;
	FVector Acceleration = FVector::ZeroVector;
	float EasedMaxAcceleration = 0.0f;
	// FRotator AimingRotation = FRotator::ZeroRotator;
	EGaitType DesiredGait = EGaitType::Running;
	FVector CurrentAcceleration = FVector::ZeroVector;

	/* ragdoll pull*/
	float RagdollPull = 0.0f;

public:
	FRotator AimingRotation = FRotator::ZeroRotator;

	friend class AAnimationTestCharacter;

private:
	FTimerHandle OnLandedFrictionResetTimer;
};
