// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLAnimEnum.generated.h"

UENUM(BlueprintType)
enum class EDLAnimCurveName :uint8
{
	Mask_AimOffset,
	BasePoseStanding,
	Layering_Spine_Add,
	Layering_Head_Add,
	Layering_Arm_L_Add,
	Layering_Arm_R_Add,
	Layering_Hand_R,
	Layering_Hand_L,
	Enable_HandIK_L,
	Layering_Arm_L,
	Enable_HandIK_R,
	Layering_Arm_R,
	Layering_Arm_L_LS,
	Layering_Arm_R_LS,
	Layering_UpperBody,
	FootLockLeft,
	FootLockRight,
	WeightInAir,

	EnableFootIkLeft,
	EnableFootIkRight,
	EnableTransition,
	RotationAmount,

	W_Gait,

	Mask_LandPrediction,
	YawOffset,
	Feet_Position
};

/**
* 角色行走状态
*/
UENUM(BlueprintType)
enum class EGaitType : uint8
{
	Walking		UMETA(ToolTip = "走"),
	Running		UMETA(ToolTip = "跑"),
	Sprinting	UMETA(ToolTip = "冲刺"),
	None,
};

USTRUCT(BlueprintType)
struct FGaitType
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		EGaitType Gait = EGaitType::Walking;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Walking_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Running_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Sprinting_ = false;
public:
	FGaitType()
	{
	}

	FGaitType(const EGaitType InitialGait) { *this = InitialGait; }

	const bool& Walking() const { return Walking_; }
	const bool& Running() const { return Running_; }
	const bool& Sprinting() const { return Sprinting_; }

	operator EGaitType() const { return Gait; }

	void operator=(const EGaitType NewGait)
	{
		Gait = NewGait;
		Walking_ = Gait == EGaitType::Walking;
		Running_ = Gait == EGaitType::Running;
		Sprinting_ = Gait == EGaitType::Sprinting;
	}
};



/**
* 脚步类型
*/
UENUM(BlueprintType)
enum class EFootstep : uint8
{
	Step	UMETA(ToolTip = "踏步"),
	WalkRun	UMETA(ToolTip = "走跑"),
	Jump	UMETA(ToolTip = "跳跃"),
	Land	UMETA(ToolTip = "降落")
};

/**
* 角色旋转模式
*/
UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	VelocityDirection	UMETA(ToolTip = "速度矢量方向"),
	LockingDirection		UMETA(ToolTip = "锁定方向"),
	Aiming				UMETA(ToolTip = "瞄准方向")
};

USTRUCT(BlueprintType)
struct FRotationMode
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		ERotationMode RotationMode = ERotationMode::VelocityDirection;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool VelocityDirection_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Aiming_ = false;
public:
	FRotationMode() {}
	FRotationMode(const ERotationMode InitialValue) { *this = InitialValue; }
	const bool& VelocityDirection() const { return VelocityDirection_; }
	const bool& Aiming() const { return Aiming_; }
	operator ERotationMode() const { return RotationMode; }

	void operator=(const ERotationMode NewValue)
	{
		RotationMode = NewValue;
		VelocityDirection_ = RotationMode == ERotationMode::VelocityDirection;
		Aiming_ = RotationMode == ERotationMode::Aiming;
	}
};

/**
* 角色主状态
*/
UENUM(BlueprintType)
enum class ECharacterMainState : uint8
{
	Normal				UMETA(ToolTip = "普通"),
	Locking				UMETA(ToolTip = "锁定"),
	Aiming				UMETA(ToolTip = "瞄准")
};

USTRUCT(BlueprintType)
struct FCharacterMainState
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		ECharacterMainState CharacterMainState = ECharacterMainState::Normal;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Normal_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Locking_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Aiming_ = false;

public:
	FCharacterMainState()
	{
	}

	FCharacterMainState(const ECharacterMainState InitialState) { *this = InitialState; }

	const bool& Normal() const { return Normal_; }
	const bool& Locking() const { return Locking_; }
	const bool& Aiming() const { return Aiming_; }

	operator ECharacterMainState() const { return CharacterMainState; }

	void operator=(const ECharacterMainState NewGait)
	{
		CharacterMainState = NewGait;
		Normal_ = CharacterMainState == ECharacterMainState::Normal;
		Locking_ = CharacterMainState == ECharacterMainState::Locking;
		Aiming_ = CharacterMainState == ECharacterMainState::Aiming;
	}
};

/**
* 扭动方向
*/
UENUM(BlueprintType)
enum class EHipsDirection : uint8
{
	Forward,
	Back,
	RightForward,
	RightBackward,
	LeftForward,
	LeftBackward
};

USTRUCT(BlueprintType)
struct FHipsDirection
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		EHipsDirection HipsDirection = EHipsDirection::Forward;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Forward_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Back_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool RightForward_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool RightBackward_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool LeftForward_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool LeftBackward_ = false;

public:
	FHipsDirection()
	{
	}

	FHipsDirection(const EHipsDirection InitialState) { *this = InitialState; }

	const bool& Forward() const { return Forward_; }
	const bool& Back() const { return Back_; }
	const bool& RightForward() const { return RightForward_; }
	const bool& RightBackward() const { return RightBackward_; }
	const bool& LeftForward() const { return LeftForward_; }
	const bool& LeftBackward() const { return LeftBackward_; }

	operator EHipsDirection() const { return HipsDirection; }

	void operator=(const EHipsDirection NewState)
	{
		HipsDirection = NewState;
		Forward_ = HipsDirection == EHipsDirection::Forward;
		Back_ = HipsDirection == EHipsDirection::Back;
		RightForward_ = HipsDirection == EHipsDirection::RightForward;
		RightBackward_ = HipsDirection == EHipsDirection::RightBackward;
		LeftForward_ = HipsDirection == EHipsDirection::LeftForward;
		LeftBackward_ = HipsDirection == EHipsDirection::LeftBackward;
	}
};

/**
* 角色移动状态
*/
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	None		UMETA(ToolTip = "无"),
	Grounded	UMETA(ToolTip = "处于地面"),
	InAir		UMETA(ToolTip = "空中"),
	Ragdoll		UMETA(ToolTip = "布娃娃")
};

USTRUCT(BlueprintType)
struct FMovementState
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = ")Animation|Movement System")
		EMovementState MovementState = EMovementState::None;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = ")Animation|Movement System")
		bool None_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = ")Animation|Movement System")
		bool Grounded_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = ")Animation|Movement System")
		bool InAir_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = ")Animation|Movement System")
		bool Ragdoll_ = false;
public:
	FMovementState() {}
	FMovementState(const EMovementState InitialValue) { *this = InitialValue; }
	const bool& None() const { return None_; }
	const bool& Grounded() const { return Grounded_; }
	const bool& InAir() const { return InAir_; }
	const bool& Ragdoll() const { return Ragdoll_; }
	operator EMovementState() const { return MovementState; }

	void operator=(const EMovementState NewValue)
	{
		MovementState = NewValue;
		None_ = MovementState == EMovementState::None;
		Grounded_ = MovementState == EMovementState::Grounded;
		InAir_ = MovementState == EMovementState::InAir;
		Ragdoll_ = MovementState == EMovementState::Ragdoll;
	}
};

/**
* 移动方向
*/
UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Forward,
	Right,
	Left,
	Backward
};

USTRUCT(BlueprintType)
struct FMovementDirection
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		EMovementDirection MovementDirection = EMovementDirection::Forward;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Forward_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Right_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Left_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Backward_ = false;
public:
	FMovementDirection() {}
	FMovementDirection(const EMovementDirection InitialValue) { *this = InitialValue; }
	const bool& Forward() const { return Forward_; }
	const bool& Right() const { return Right_; }
	const bool& Left() const { return Left_; }
	const bool& Backward() const { return Backward_; }
	operator EMovementDirection() const { return MovementDirection; }

	void operator=(const EMovementDirection NewValue)
	{
		MovementDirection = NewValue;
		Forward_ = MovementDirection == EMovementDirection::Forward;
		Right_ = MovementDirection == EMovementDirection::Right;
		Left_ = MovementDirection == EMovementDirection::Left;
		Backward_ = MovementDirection == EMovementDirection::Backward;
	}
};

/**
* 插槽
*/
UENUM(BlueprintType)
enum class EDLAnimSlot : uint8
{
	DefaultSlot,
	UpperBody,
	FullBody,
	GroundedSlot
};

/**
* 叠加状态
*/
UENUM(BlueprintType)
enum class EDLAnimOverlayState : uint8
{
	Default,
	Shield,
	Injured,
};

USTRUCT(BlueprintType)
struct FDLAnimOverlayState
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		EDLAnimOverlayState DLAnimOverlayState = EDLAnimOverlayState::Default;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Default_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Shield_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Injured_ = false;
public:
	FDLAnimOverlayState() {}
	FDLAnimOverlayState(const EDLAnimOverlayState InitialValue) { *this = InitialValue; }
	const bool& Default() const { return Default_; }
	const bool& Shield() const { return Shield_; }
	const bool& Injured() const { return Injured_; }
	operator EDLAnimOverlayState() const { return DLAnimOverlayState; }

	void operator=(const EDLAnimOverlayState NewValue)
	{
		DLAnimOverlayState = NewValue;
		Default_ = DLAnimOverlayState == EDLAnimOverlayState::Default;
		Shield_ = DLAnimOverlayState == EDLAnimOverlayState::Shield;
		Injured_ = DLAnimOverlayState == EDLAnimOverlayState::Injured;
	}
};

/**
* 角色移动动作状态
*/
UENUM(BlueprintType)
enum class EMovementAction : uint8
{
	None		UMETA(ToolTip = "无"),
	Rolling		UMETA(ToolTip = "滚动"),
	GettingUp	UMETA(ToolTip = "起身")
};

USTRUCT(BlueprintType)
struct FMovementAction
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		EMovementAction MovementAction = EMovementAction::None;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool None_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Rolling_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool GettingUp_ = false;
public:
	FMovementAction() {}
	FMovementAction(const EMovementAction InitialValue) { *this = InitialValue; }
	const bool& None() const { return None_; }
	const bool& Rolling() const { return Rolling_; }
	const bool& GettingUp() const { return GettingUp_; }
	operator EMovementAction() const { return MovementAction; }

	void operator=(const EMovementAction NewValue)
	{
		MovementAction = NewValue;
		None_ = MovementAction == EMovementAction::None;
		Rolling_ = MovementAction == EMovementAction::Rolling;
		GettingUp_ = MovementAction == EMovementAction::GettingUp;
	}
};

/**
* 触地状态
*/
UENUM(BlueprintType)
enum class EGroundedEntryState : uint8
{
	None	UMETA(ToolTip = "无"),
	Roll	UMETA(ToolTip = "滚动")
};

USTRUCT(BlueprintType)
struct FGroundedEntryState
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		EGroundedEntryState GroundedEntryState = EGroundedEntryState::None;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool None_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Roll_ = false;
public:
	FGroundedEntryState() {}
	FGroundedEntryState(const EGroundedEntryState InitialValue) { *this = InitialValue; }
	const bool& None() const { return None_; }
	const bool& Roll() const { return Roll_; }
	operator EGroundedEntryState() const { return GroundedEntryState; }

	void operator=(const EGroundedEntryState NewValue)
	{
		GroundedEntryState = NewValue;
		None_ = GroundedEntryState == EGroundedEntryState::None;
		Roll_ = GroundedEntryState == EGroundedEntryState::Roll;
	}
};
