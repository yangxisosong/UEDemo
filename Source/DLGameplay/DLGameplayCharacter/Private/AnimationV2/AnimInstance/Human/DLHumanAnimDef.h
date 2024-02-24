// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/DLAnimBaseDef.h"
#include "AnimationV2/DLAnimCommonDef.h"
#include "DLHumanAnimDef.generated.h"

class UAnimSequenceBase;
class UCurveFloat;
class UCurveVector;
class UAnimationAsset;

/**
* 触地状态
*/
UENUM(BlueprintType)
enum class EAnimGroundedEntryState : uint8
{
	None	UMETA(ToolTip = "无"),
	Roll	UMETA(ToolTip = "滚动")
};

USTRUCT(BlueprintType)
struct FAnimGroundedEntryState
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		EAnimGroundedEntryState GroundedEntryState = EAnimGroundedEntryState::None;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool None_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Roll_ = false;
public:
	FAnimGroundedEntryState() {}
	FAnimGroundedEntryState(const EAnimGroundedEntryState InitialValue) { *this = InitialValue; }
	const bool& None() const { return None_; }
	const bool& Roll() const { return Roll_; }
	operator EAnimGroundedEntryState() const { return GroundedEntryState; }

	void operator=(const EAnimGroundedEntryState NewValue)
	{
		GroundedEntryState = NewValue;
		None_ = GroundedEntryState == EAnimGroundedEntryState::None;
		Roll_ = GroundedEntryState == EAnimGroundedEntryState::Roll;
	}
};

USTRUCT(BlueprintType, Blueprintable)
struct FHumanAnimCharacterInfo :public FAnimCharacterInfoBase
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | CharacterInfo")
		FAnimCharacterMainState CharacterMainState = EAnimCharacterMainState::Normal;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | CharacterInfo")
		FAnimSpecialState SpecialState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | CharacterInfo")
		FAnimGaitType Gait;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | CharacterInfo")
		FAnimGaitType LastInputGait;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | CharacterInfo")
		FVector Velocity = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FVector RelativeVelocityDirection = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FVector Acceleration = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FVector MovementInput = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		bool bIsMoving = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		bool bHasMovementInput = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		float Speed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		float MovementInputAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | CharacterInfo")
		FRotator CharacterActorRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FAnimMovementState MovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FAnimMovementDirection MovementDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FRotator TargetRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FRotator Turn_TargetRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FDLMovementAction MovementAction;
};

USTRUCT(BlueprintType)
struct FAnimLandConfig
{
	GENERATED_BODY()
public:
	/*落地滚动动画*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UAnimMontage* LandRollMontage = nullptr;

	/*触发发落地滚动的下落速度*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float BreakfallOnLandVelocity = 600.0f;
public:
	bool IsValidConfig(TArray<FText>& Errors) const;
};

USTRUCT(BlueprintType)
struct FDLTurnInPlaceAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		UAnimSequenceBase* Animation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		float AnimatedAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		FName SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		bool ScaleTurnAngle = true;
public:
	bool IsValidConfig(TArray<FText>& Errors) const;
};

USTRUCT(BlueprintType)
struct FDLAnimTurnInPlaceConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		float TurnCheckMinAngle = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		float Turn180Threshold = 130.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Animation | TurnInPlace")
		float AimYawRateLimit = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		float MinAngleDelay = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		float MaxAngleDelay = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		float BlendInTime = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		float BlendOutTime = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		FDLTurnInPlaceAsset N_TurnIP_L90;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		FDLTurnInPlaceAsset N_TurnIP_R90;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		FDLTurnInPlaceAsset N_TurnIP_L180;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInPlace")
		FDLTurnInPlaceAsset N_TurnIP_R180;
public:
	bool IsValidConfig(TArray<FText>& Errors) const;
};

USTRUCT(BlueprintType)
struct FHumanIKConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FootIK")
		float IK_TraceDistanceBelowFoot = 55.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FootIK")
		float IK_TraceDistanceAboveFoot = 60.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FootIK")
		float FootHeight = 15.0f;
};

USTRUCT(BlueprintType)
struct FHumanAnimConfig :public FAnimConfigBase
{
	GENERATED_BODY()
public:
	/*移动状态设置*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FDLAnimMovementSettings MovementStateSettings;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float VelocityBlendInterpSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float GroundedLeanInterpSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float GroundedLeanScale = 1.0f;

	/*行走动画设计时速*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
		float AnimatedWalkSpeed = 150.0f;

	/*小跑动画设计时速*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
		float AnimatedJogSpeed = 350.0f;

	/*冲刺动画设计时速*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
		float AnimatedSprintSpeed = 600.0f;

	/*最小触发移动的速度*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
		float MinMoveThreshold = 50.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FAnimLandConfig LandConfig;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "TurnInPlace")
		FDLAnimTurnInPlaceConfig TurnInPlaceConfig;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		FHumanIKConfig IKConfig;
public:

	virtual bool IsValidConfig(TArray<FText>& Errors) const override;
};

USTRUCT(BlueprintType)
struct FAnimGraphGroundedValues
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		EDirectionOfHipAnim TrackedHipsDirection = EDirectionOfHipAnim::None;

	//初始值一定是false
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		bool bShouldMove = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		bool bRotateL = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		bool bRotateR = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Grounded")
		bool bPivot = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		float RotateRate = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		float RotationScale = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		float DiagonalScaleAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		float WalkRunBlend = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		float StandingPlayRate = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		float StrideBlend = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		float ForwardYaw = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		float BackwardYaw = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		float LeftYaw = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		float RightYaw = 0.0f;

};

USTRUCT(BlueprintType)
struct FAnimGraphInAirValues
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		bool bJumped = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		float JumpPlayRate = 1.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		float FallSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		float LandPrediction = 1.0f;
};

USTRUCT(BlueprintType)
struct FDLAnimGraphAimingValues
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS")
		FVector2D AimingAngle;
};
