// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Animation/DLAnimMacros.h"
#include "DLAnimEnum.h"
#include "DLAnimStruct.generated.h"

class UAnimSequenceBase;
class UCurveFloat;
class UCurveVector;
class UAnimationAsset;

struct FAnimBlendCurvesConfig;
struct FMovementSettings;
struct FMovementStateSettings;
struct FRotateInPlaceAsset;
struct FDynamicMontageParams;
struct FLeanAmount;
struct FVelocityBlend;
struct FTurnInPlaceAsset;
struct FAnimGraphGrounded;
struct FAnimGraphInAir;
struct FAnimGraphAimingValues;
struct FAnimGraphLayerBlending;
struct FAnimGraphFootIK;
struct FAnimTurnInPlace;
struct FAnimCharacterInfo;
struct FDLAnimAsset;
struct FAnimTransitionSeqConfig;
struct FAnimTurnInPlaceConfig;
struct FAnimSkeletonConfig;
struct FCharacterSpeedConfig;
struct FStopAnimConfigs;

USTRUCT(BlueprintType)
struct FAnimBlendCurvesConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
		UCurveFloat* DiagonalScaleAmountCurve = nullptr;

	/*行走步幅混合曲线*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
		UCurveFloat* StrideBlend_N_Walk = nullptr;

	/*小跑步幅混合曲线*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
		UCurveFloat* StrideBlend_N_Run = nullptr;

	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	// 	UCurveFloat* StrideBlend_C_Walk = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
		UCurveFloat* LandPredictionCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
		UCurveFloat* LeanInAirCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
		UCurveVector* YawOffset_FB = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
		UCurveVector* YawOffset_LR = nullptr;

public:
	bool IsValidConfig(TArray<FText>& Errors) const;
};

USTRUCT(BlueprintType)
struct FStopAnimConfigs
{
	GENERATED_BODY()
public:
	/*停止曲线-前*/
	UPROPERTY(EditAnywhere)
		UCurveFloat* Forward = nullptr;

	/*停止曲线-后*/
	UPROPERTY(EditAnywhere)
		UCurveFloat* Backward = nullptr;

	/*停止曲线-左*/
	UPROPERTY(EditAnywhere)
		UCurveFloat* Left = nullptr;

	/*停止曲线-右*/
	UPROPERTY(EditAnywhere)
		UCurveFloat* Right = nullptr;
public:
	bool IsValidConfig(TArray<FText>& Errors) const;

};

USTRUCT(BlueprintType)
struct FCharacterSpeedConfig
{
	GENERATED_BODY()
public:

	/*左右*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float LeftAndRightSpeed = 350.0f;

	/*后*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float BackSpeed = 250.0f;

	/*前*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float ForwardSpeed = 450.0f;

	/*停止动画播放匹配曲线,Y为时间,X为速度*/
	UPROPERTY(EditAnywhere)
		FStopAnimConfigs StopSetting;

public:
	float GetSpeedByDirection(const EMovementDirection Direction) const;

	float CalculateSpeed(const FVector& Velocity, const FRotator& CharacterRotation) const;

	bool IsValidConfig(TArray<FText>& Errors) const;

};

USTRUCT(BlueprintType)
struct FMovementSettings
{
	GENERATED_BODY()
public:

	/*行走速度配置  #TODO No*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FCharacterSpeedConfig Walk;

	/*小跑速度配置	#TODO No */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FCharacterSpeedConfig Run;

	/*冲刺速度配置  #TODO No */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FCharacterSpeedConfig Sprint;

	/*小跑停止距离   #TODO No */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float JogStopDistance;

	/*冲刺停止距离   #TODO No */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float SprintStopDistance;

	/*移动曲线    #TODO No */
	UPROPERTY(EditAnywhere)
		UCurveVector* MovementCurve = nullptr;

	/*旋转速率曲线   #TODO  用来匹配 锁定状态时 跟着BOSS 旋转     */
	UPROPERTY(EditAnywhere)
		UCurveFloat* RotationRateCurve = nullptr;

	/*混合用曲线设置  纯表现 */
	UPROPERTY(EditAnywhere)
		FAnimBlendCurvesConfig BlendCurvesConfig;

public:

	FCharacterSpeedConfig GetSpeedForGait(const EGaitType Gait) const;

	bool IsValidConfig(TArray<FText>& Errors) const;
};

USTRUCT(BlueprintType)
struct FMovementStateSettings
{
	GENERATED_BODY()
public:
	/*普通*/
	UPROPERTY(EditAnywhere)
		FMovementSettings Normal;

	/*锁定*/
	UPROPERTY(EditAnywhere)
		FMovementSettings Locking;

public:
	bool IsValidConfig(TArray<FText>& Errors) const;
};

USTRUCT(BlueprintType)
struct FRotateInPlaceAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Rotation System")
		FName SlotName;

	UPROPERTY(EditAnywhere, Category = "Rotation System")
		float SlowTurnRate = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Rotation System")
		float FastTurnRate = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Rotation System")
		float SlowPlayRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Rotation System")
		float FastPlayRate = 1.0f;
};

USTRUCT(BlueprintType)
struct FDynamicMontageParams
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimSequenceBase* Animation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float BlendInTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float BlendOutTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float PlayRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float StartTime = 0.0f;
};

USTRUCT(BlueprintType)
struct FLeanAmount
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
		float LeftRight = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
		float ForwardBack = 0.0f;
};

USTRUCT(BlueprintType)
struct FVelocityBlend
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		float Forward = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		float Backward = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		float Left = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		float Right = 0.0f;
};

USTRUCT(BlueprintType)
struct FTurnInPlaceAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		UAnimSequenceBase* Animation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		float AnimatedAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		FName SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		bool ScaleTurnAngle = true;

public:
	bool IsValidConfig(TArray<FText>& Errors) const;
};

USTRUCT(BlueprintType)
struct FAnimGraphGrounded
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Grounded")
		FHipsDirection TrackedHipsDirection = EHipsDirection::Forward;

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
struct FAnimGraphInAir
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
struct FAnimGraphAimingValues
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Aiming")
		FRotator SmoothedAimingRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Aiming")
		FRotator SpineRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Aiming")
		FVector2D AimingAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Aiming")
		float AimSweepTime = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Aiming")
		float InputYawOffsetTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Aiming")
		float ForwardYawTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Aiming")
		float LeftYawTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Aiming")
		float RightYawTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Aiming")
		FVector2D SmoothedAimingAngle = FVector2D::ZeroVector;
};

USTRUCT(BlueprintType)
struct FAnimGraphLayerBlending
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		int32 OverlayOverrideState = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float EnableAimOffset = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float BasePose_N = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float BasePose_CLF = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Arm_L = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Arm_L_Add = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Arm_L_LS = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Arm_L_MS = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Arm_R = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Arm_R_Add = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Arm_R_LS = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Arm_R_MS = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Hand_L = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Hand_R = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Legs = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Legs_Add = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Pelvis = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Pelvis_Add = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Spine = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Spine_Add = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Head = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float Head_Add = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float EnableHandIK_L = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | LayerBlending")
		float EnableHandIK_R = 1.0f;
};

USTRUCT(BlueprintType)
struct FAnimGraphFootIK
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		float FootLock_L_Alpha = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		float FootLock_R_Alpha = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		bool UseFootLockCurve_L;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		bool UseFootLockCurve_R;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		FVector FootLock_L_Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		FVector TargetFootLock_R_Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		FVector FootLock_R_Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		FRotator TargetFootLock_L_Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		FRotator FootLock_L_Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		FRotator TargetFootLock_R_Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		FRotator FootLock_R_Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		FVector FootOffset_L_Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		FVector FootOffset_R_Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		FRotator FootOffset_L_Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		FRotator FootOffset_R_Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		FVector PelvisOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | FootIK")
		float PelvisAlpha = 0.0f;
};

USTRUCT(BlueprintType)
struct FAnimTurnInPlace
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInce")
		float ElapsedDelayTime = 0.0f;
};

USTRUCT(BlueprintType)
struct FAnimRotateInPlace
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | RotateInPlace")
		float RotateMinThreshold = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | RotateInPlace")
		float RotateMaxThreshold = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | RotateInPlace")
		float AimYawRateMinRange = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | RotateInPlace")
		float AimYawRateMaxRange = 270.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | RotateInPlace")
		float MinPlayRate = 1.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | RotateInPlace")
		float MaxPlayRate = 3.0f;
};

USTRUCT(BlueprintType)
struct FAnimCharacterInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FCharacterMainState CharacterMainState = ECharacterMainState::Normal;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FGameplayTagContainer AnimCharacterStateContainer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FVector Velocity = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FGaitType Gait;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		float AimYawRate = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FRotator AimingRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FRotator CharacterActorRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FMovementState MovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FMovementState PrevMovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FDLAnimOverlayState OverlayState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FMovementAction MovementAction = EMovementAction::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		float StopAnimBeginTimeStamp = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		FGaitType LastInputGait = EGaitType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		float RotateDelta = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation | Character")
		bool bHasHorizontalInput = false;
public:
	bool HasAnyTags(const FGameplayTagContainer& Tags) const;

	bool HasTag(const FGameplayTag& Tag) const;
};

USTRUCT(BlueprintType)
struct FDLAnimAsset
{
	GENERATED_BODY()
public:
	/*动画资产*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UAnimationAsset* Animation = nullptr;
};

USTRUCT(BlueprintType)
struct FAnimTransitionSeqConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration | TransitionSeq")
		UAnimSequenceBase* TransitionAnim_R = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration | TransitionSeq")
		UAnimSequenceBase* TransitionAnim_L = nullptr;
};

USTRUCT(BlueprintType)
struct FAnimTurnInPlaceConfig
{
	GENERATED_BODY()
public:
	/*最小转身角度    */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInce")
		float TurnCheckMinAngle = 45.0f;

	/*180度转身最小触发值*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInce")
		float Turn180Threshold = 130.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInce")
		float AimYawRateLimit = 50.0f;

	/*最小转身响应延迟（越小越跟随）*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInce")
		float MinAngleDelay = 0.f;

	/*最大转身响应延迟（越小越跟随）*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInce")
		float MaxAngleDelay = 0.75f;

	/*向左90度转身*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInce")
		FTurnInPlaceAsset N_TurnIP_L90;

	/*向右90度转身*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInce")
		FTurnInPlaceAsset N_TurnIP_R90;

	/*向左180度转身*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInce")
		FTurnInPlaceAsset N_TurnIP_L180;

	/*向右180度转身*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | TurnInce")
		FTurnInPlaceAsset N_TurnIP_R180;

public:
	bool IsValidConfig(TArray<FText>& Errors) const;
};


USTRUCT(BlueprintType)
struct FAnimSkeletonConfig
{
	GENERATED_BODY()
public:
	FAnimSkeletonConfig();

	/*骨骼映射*/
	UPROPERTY(EditAnywhere, EditFixedSize)
		TMap<FGameplayTag, FName> Bones;

public:
	const FName GetBoneByTag(const FGameplayTag Name) const;
};

USTRUCT(BlueprintType)
struct FRollConfig
{
	GENERATED_BODY()
public:

	/*前翻*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UAnimMontage* Forward = nullptr;

	/*后翻*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UAnimMontage* Backward = nullptr;

	/*左翻*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UAnimMontage* Left = nullptr;

	/*右翻*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UAnimMontage* Right = nullptr;

	/*后撤*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UAnimMontage* StepBack = nullptr;

public:
	bool IsValidConfig(TArray<FText>& Errors) const;
};

USTRUCT(BlueprintType)
struct FLandConfig
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
struct DLGAMEPLAYCHARACTER_API FAnimConfig : public  FTableRowBase
{
	GENERATED_BODY()
public:
	FAnimConfig();

public:
	/*状态机中所使用的动画*/
	// UPROPERTY(BlueprintReadOnly, EditAnywhere)
	// 	TMap<FGameplayTag, FDLAnimAsset> Animations;

	/*移动状态设置*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FMovementStateSettings MovementStateSettings;

	/*骨骼映射配置*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FAnimSkeletonConfig SkeletonConfig;

	/*过渡动画配置*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FAnimTransitionSeqConfig TransitionSeqConfig;

	/*转身配置*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FAnimTurnInPlaceConfig TurnInPlaceConfig;

	/*翻滚配置  #TODO No */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FRollConfig RollConfig;

	/*行走动画设计时速*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
		float AnimatedWalkSpeed = 150.0f;

	/*小跑动画设计时速*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
		float AnimatedJogSpeed = 350.0f;

	/*冲刺动画设计时速*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
		float AnimatedSprintSpeed = 600.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "InterpSpeed")
		float VelocityBlendInterpSpeed = 12.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "InterpSpeed")
		float GroundedLeanInterpSpeed = 4.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "InterpSpeed")
		float InAirLeanInterpSpeed = 4.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "InterpSpeed")
		float SmoothedAimingRotationInterpSpeed = 10.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "InterpSpeed")
		float InputYawOffsetInterpSpeed = 8.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float TriggerPivotSpeedLimit = 200.0f;

	/*脚踝到地面的高度*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float FootHeight = 13.5f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float DynamicTransitionThreshold = 8.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float IK_TraceDistanceAboveFoot = 50.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float IK_TraceDistanceBelowFoot = 13.5f;

	/*旋转时会导致减速，这条曲线会在旋转时调整加速度解决该问题*/
	// UPROPERTY(BlueprintReadOnly, EditAnywhere)
	// 	UCurveFloat* RotationAcceleration = nullptr;

	/*落地配置  #TODO No 思考一下 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FLandConfig LandConfig;

public:
	const FAnimBlendCurvesConfig& GetBlendCurveConfigByState(const ECharacterMainState State) const;

	bool IsValidConfig(TArray<FText>& Errors) const;
};
