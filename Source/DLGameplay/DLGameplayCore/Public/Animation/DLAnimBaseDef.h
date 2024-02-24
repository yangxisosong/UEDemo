// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "DLAnimBaseDef.generated.h"

USTRUCT(BlueprintType)
struct DLGAMEPLAYCORE_API FAnimCharacterInfoBase
{
	GENERATED_USTRUCT_BODY()
public:
	FAnimCharacterInfoBase() { }
	virtual ~FAnimCharacterInfoBase() { }
};

USTRUCT(BlueprintType)
struct DLGAMEPLAYCORE_API FDLAnimSkeletonConfig
{
	GENERATED_BODY()
public:
	FDLAnimSkeletonConfig();

	/*骨骼映射*/
	UPROPERTY(EditAnywhere, EditFixedSize)
		TMap<FGameplayTag, FName> Bones;

public:
	const FName GetBoneByTag(const FGameplayTag Name) const;
};

USTRUCT(BlueprintType)
struct DLGAMEPLAYCORE_API FAnimConfigBase :public FTableRowBase
{
	GENERATED_BODY()
public:

	virtual bool IsValidConfig(TArray<FText>& Errors) const { return false; }
};


struct FDLAnimInsInitParams
{
public:
	FDLAnimInsInitParams(const FAnimConfigBase& InAnimConfig)
		:AnimConfig(InAnimConfig)
	{

	}

	const FAnimConfigBase& AnimConfig;
};

/**
* 移动方向
*/
UENUM(BlueprintType)
enum class EAnimMovementDirection : uint8
{
	None,
	Forward,
	Right,
	Left,
	Backward,
};

USTRUCT(BlueprintType)
struct DLGAMEPLAYCORE_API FAnimMovementDirection
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "MovementDirection")
		EAnimMovementDirection AnimMovementDirection = EAnimMovementDirection::None;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "MovementDirection")
		bool None_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "MovementDirection")
		bool Forward_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "MovementDirection")
		bool Right_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "MovementDirection")
		bool Left_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "MovementDirection")
		bool Backward_ = false;
public:
	FAnimMovementDirection() {}
	FAnimMovementDirection(const EAnimMovementDirection InitialValue) { *this = InitialValue; }
	const bool& None() const { return None_; }
	const bool& Forward() const { return Forward_; }
	const bool& Right() const { return Right_; }
	const bool& Left() const { return Left_; }
	const bool& Backward() const { return Backward_; }
	operator EAnimMovementDirection() const { return AnimMovementDirection; }

	void operator=(const EAnimMovementDirection NewValue)
	{
		AnimMovementDirection = NewValue;
		None_ = AnimMovementDirection == EAnimMovementDirection::None;
		Forward_ = AnimMovementDirection == EAnimMovementDirection::Forward;
		Right_ = AnimMovementDirection == EAnimMovementDirection::Right;
		Left_ = AnimMovementDirection == EAnimMovementDirection::Left;
		Backward_ = AnimMovementDirection == EAnimMovementDirection::Backward;
	}
};

UENUM(BlueprintType)
enum class EAnimSpecialState : uint8
{
	None,
	Dead,
};

USTRUCT(BlueprintType)
struct FAnimSpecialState
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation | SpecialState")
		EAnimSpecialState AnimSpecialState = EAnimSpecialState::None;
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation | SpecialState")
		bool None_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation | SpecialState")
		bool Dead_ = false;
public:
	FAnimSpecialState() {}
	FAnimSpecialState(const EAnimSpecialState InitialValue) { *this = InitialValue; }
	const bool& None() const { return None_; }
	const bool& Dead() const { return Dead_; }
	operator EAnimSpecialState() const { return AnimSpecialState; }

	void operator=(const EAnimSpecialState NewValue)
	{
		AnimSpecialState = NewValue;
		None_ = AnimSpecialState == EAnimSpecialState::None;
		Dead_ = AnimSpecialState == EAnimSpecialState::Dead;
	}
};

USTRUCT(BlueprintType)
struct FDLAnimationAssetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float StartPos = 0.0f;
};

USTRUCT(BlueprintType)
struct FDLAnimationAsset : public FDLAnimationAssetBase
{
	GENERATED_BODY()
public:
	/*动画资产*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UAnimationAsset* Animation = nullptr;
};

USTRUCT(BlueprintType)
struct FDLAnimMontageAsset : public FDLAnimationAssetBase
{
	GENERATED_BODY()
public:
	/*动画资产*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UAnimMontage* Animation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName StartSection = NAME_None;
};