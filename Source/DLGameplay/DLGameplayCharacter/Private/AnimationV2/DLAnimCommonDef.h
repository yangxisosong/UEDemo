// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLGameplayCore/Public/Animation/DLAnimBaseDef.h"
#include "DLAnimCommonDef.generated.h"

class UAnimSequenceBase;
class UCurveFloat;
class UCurveVector;
class UAnimationAsset;

/**
* 角色行走状态
*/
UENUM(BlueprintType)
enum class EAnimGaitType : uint8
{
	None,
	Walking		UMETA(ToolTip = "走"),
	Running		UMETA(ToolTip = "跑"),
	Sprinting	UMETA(ToolTip = "冲刺"),
};

USTRUCT(BlueprintType)
struct FAnimGaitType
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "GaitType")
		EAnimGaitType AnimGaitType = EAnimGaitType::None;
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "GaitType")
		bool None_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "GaitType")
		bool Walking_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "GaitType")
		bool Running_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "GaitType")
		bool Sprinting_ = false;
public:
	FAnimGaitType() {}
	FAnimGaitType(const EAnimGaitType InitialValue) { *this = InitialValue; }
	const bool& None() const { return None_; }
	const bool& Walking() const { return Walking_; }
	const bool& Running() const { return Running_; }
	const bool& Sprinting() const { return Sprinting_; }
	operator EAnimGaitType() const { return AnimGaitType; }

	void operator=(const EAnimGaitType NewValue)
	{
		AnimGaitType = NewValue;
		None_ = AnimGaitType == EAnimGaitType::None;
		Walking_ = AnimGaitType == EAnimGaitType::Walking;
		Running_ = AnimGaitType == EAnimGaitType::Running;
		Sprinting_ = AnimGaitType == EAnimGaitType::Sprinting;
	}
};

/**
* 角色主状态
*/
UENUM(BlueprintType)
enum class EAnimCharacterMainState : uint8
{
	None				UMETA(ToolTip = "无"),
	Normal				UMETA(ToolTip = "普通"),
	Locking				UMETA(ToolTip = "锁定"),
};

USTRUCT(BlueprintType)
struct FAnimCharacterMainState
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		EAnimCharacterMainState CharacterMainState = EAnimCharacterMainState::None;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Normal_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool Locking_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation|Movement System")
		bool None_ = false;

public:
	FAnimCharacterMainState()
	{
	}

	FAnimCharacterMainState(const EAnimCharacterMainState InitialState) { *this = InitialState; }

	const bool& Normal() const { return Normal_; }
	const bool& Locking() const { return Locking_; }
	const bool& None() const { return None_; }

	operator EAnimCharacterMainState() const { return CharacterMainState; }

	void operator=(const EAnimCharacterMainState NewGait)
	{
		CharacterMainState = NewGait;
		Normal_ = CharacterMainState == EAnimCharacterMainState::Normal;
		Locking_ = CharacterMainState == EAnimCharacterMainState::Locking;
		None_ = CharacterMainState == EAnimCharacterMainState::None;
	}
};

/**
* 角色移动状态
*/
UENUM(BlueprintType)
enum class EAnimMovementState : uint8
{
	None		UMETA(ToolTip = "无"),
	Grounded	UMETA(ToolTip = "处于地面"),
	InAir		UMETA(ToolTip = "空中"),
	Ragdoll		UMETA(ToolTip = "布娃娃")
};

USTRUCT(BlueprintType)
struct FAnimMovementState
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "MovementState")
		EAnimMovementState AnimMovementState = EAnimMovementState::None;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "MovementState")
		bool None_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "MovementState")
		bool Grounded_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "MovementState")
		bool InAir_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "MovementState")
		bool Ragdoll_ = false;
public:
	FAnimMovementState() {}
	FAnimMovementState(const EAnimMovementState InitialValue) { *this = InitialValue; }
	const bool& None() const { return None_; }
	const bool& Grounded() const { return Grounded_; }
	const bool& InAir() const { return InAir_; }
	const bool& Ragdoll() const { return Ragdoll_; }
	operator EAnimMovementState() const { return AnimMovementState; }

	void operator=(const EAnimMovementState NewValue)
	{
		AnimMovementState = NewValue;
		None_ = AnimMovementState == EAnimMovementState::None;
		Grounded_ = AnimMovementState == EAnimMovementState::Grounded;
		InAir_ = AnimMovementState == EAnimMovementState::InAir;
		Ragdoll_ = AnimMovementState == EAnimMovementState::Ragdoll;
	}
};

USTRUCT(BlueprintType)
struct FAnimBlendCurves
{
	GENERATED_BODY()
public:
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	// 	UCurveFloat* DiagonalScaleAmountCurve = nullptr;

	/*行走步幅混合曲线*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
		UCurveFloat* StrideBlend_N_Walk = nullptr;

	/*小跑步幅混合曲线*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
		UCurveFloat* StrideBlend_N_Run = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
		UCurveFloat* LandPredictionCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
		UCurveFloat* LeanInAirCurve = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Configuration|Blend Curves")
		UCurveVector* YawOffset_FB = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Configuration|Blend Curves")
		UCurveVector* YawOffset_LR = nullptr;

public:
	bool IsValidConfig(TArray<FText>& Errors) const;
};

USTRUCT(BlueprintType)
struct FDLAnimMovementSetting
{
	GENERATED_BODY()
public:
	/*移动曲线*/
	// UPROPERTY(EditAnywhere)
	// 	UCurveVector* MovementCurve = nullptr;

	/*混合用曲线设置  纯表现 */
	UPROPERTY(EditAnywhere)
		FAnimBlendCurves BlendCurvesConfig;

public:
	bool IsValidConfig(TArray<FText>& Errors) const;
};

USTRUCT(BlueprintType)
struct FDLAnimMovementSettings
{
	GENERATED_BODY()
public:
	/*普通*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FDLAnimMovementSetting Normal;

	/*锁定*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FDLAnimMovementSetting Locking;

public:
	bool IsValidConfig(TArray<FText>& Errors) const;

	const FAnimBlendCurves& GetBlendCurveConfigByState(const EAnimCharacterMainState State) const;
};



USTRUCT(BlueprintType)
struct FAnimVelocityBlend
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
		float Forward = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
		float Backward = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
		float Left = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
		float Right = 0.0f;
};

/**
* 扭动方向
*/
UENUM(BlueprintType)
enum class EDirectionOfHipAnim : uint8
{
	None,
	Forward,
	Back,
	RightForward,
	RightBackward,
	LeftForward,
	LeftBackward
};

USTRUCT(BlueprintType)
struct FDirectionOfHip
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "HipsDirection")
		EDirectionOfHipAnim DirectionOfHip = EDirectionOfHipAnim::None;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "HipsDirection")
		bool None_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "HipsDirection")
		bool Forward_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "HipsDirection")
		bool Back_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "HipsDirection")
		bool RightForward_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "HipsDirection")
		bool RightBackward_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "HipsDirection")
		bool LeftForward_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "HipsDirection")
		bool LeftBackward_ = false;
public:
	FDirectionOfHip() {}
	FDirectionOfHip(const EDirectionOfHipAnim InitialValue) { *this = InitialValue; }
	const bool& None() const { return None_; }
	const bool& Forward() const { return Forward_; }
	const bool& Back() const { return Back_; }
	const bool& RightForward() const { return RightForward_; }
	const bool& RightBackward() const { return RightBackward_; }
	const bool& LeftForward() const { return LeftForward_; }
	const bool& LeftBackward() const { return LeftBackward_; }
	operator EDirectionOfHipAnim() const { return DirectionOfHip; }

	void operator=(const EDirectionOfHipAnim NewValue)
	{
		DirectionOfHip = NewValue;
		None_ = DirectionOfHip == EDirectionOfHipAnim::None;
		Forward_ = DirectionOfHip == EDirectionOfHipAnim::Forward;
		Back_ = DirectionOfHip == EDirectionOfHipAnim::Back;
		RightForward_ = DirectionOfHip == EDirectionOfHipAnim::RightForward;
		RightBackward_ = DirectionOfHip == EDirectionOfHipAnim::RightBackward;
		LeftForward_ = DirectionOfHip == EDirectionOfHipAnim::LeftForward;
		LeftBackward_ = DirectionOfHip == EDirectionOfHipAnim::LeftBackward;
	}
};

USTRUCT(BlueprintType)
struct FBodyLeanAmount
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
		float LeftRight = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
		float ForwardBack = 0.0f;
};

/**
* 脚步类型
*/
UENUM(BlueprintType)
enum class EDLFootstepType : uint8
{
	Default UMETA(ToolTip = "默认"),
	WalkRun	UMETA(ToolTip = "走跑"),
	Rush    UMETA(ToolTip = "冲刺"),
	Scrape	UMETA(ToolTip = "急停"),
	Step	UMETA(ToolTip = "踏步"),
	Jump	UMETA(ToolTip = "跳跃"),
	Land	UMETA(ToolTip = "降落"),
	
};


/**
* 角色移动动作状态
*/
UENUM(BlueprintType)
enum class EDLMovementAction : uint8
{
	None		UMETA(ToolTip = "无"),
	Rolling		UMETA(ToolTip = "滚动"),
	GettingUp	UMETA(ToolTip = "起身")
};

USTRUCT(BlueprintType)
struct FDLMovementAction
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "")
		EDLMovementAction DLMovementAction = EDLMovementAction::None;
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "")
		bool None_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "")
		bool Rolling_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "")
		bool GettingUp_ = false;
public:
	FDLMovementAction() {}
	FDLMovementAction(const EDLMovementAction InitialValue) { *this = InitialValue; }
	const bool& None() const { return None_; }
	const bool& Rolling() const { return Rolling_; }
	const bool& GettingUp() const { return GettingUp_; }
	operator EDLMovementAction() const { return DLMovementAction; }

	void operator=(const EDLMovementAction NewValue)
	{
		DLMovementAction = NewValue;
		None_ = DLMovementAction == EDLMovementAction::None;
		Rolling_ = DLMovementAction == EDLMovementAction::Rolling;
		GettingUp_ = DLMovementAction == EDLMovementAction::GettingUp;
	}
};

/**
* 角色移滚动方向
*/
UENUM(BlueprintType)
enum class EDLAnimRollDirection : uint8
{
	None		UMETA(ToolTip = "无"),
	Forward,
	Backward,
	Left,
	Right,
	ForwardLeft,
	ForwardRight,
	BackwardLeft,
	BackwardRight,
};