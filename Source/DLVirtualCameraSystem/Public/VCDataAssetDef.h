#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveVector.h"
#include "Engine/DataAsset.h"
#include "VCDataAssetDef.generated.h"



class UVCProcessor;
UENUM()
enum class EVCBlendMode : uint8
{
	Inherit,
	Override,
	Add,
	Lerp,
};

UENUM()
enum class EVCBlendValueType : uint8
{
	Value,
	Curve,
};

UENUM()
enum class EVCBlendCoordinate : uint8
{
	World,
	Local,
};

UENUM()
enum class EVCBlendBool : uint8
{
	Inherit,
	BoolTrue,
	BoolFalse,
};


USTRUCT(BlueprintType)
struct FVCBlendFloat
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVCBlendMode BlendMode = EVCBlendMode::Inherit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(EditCondition = "BlendMode != EVCBlendMode::Inherit", EditConditionHides))
	EVCBlendValueType ValueType = EVCBlendValueType::Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value",
		meta=(EditCondition = "BlendMode != EVCBlendMode::Inherit && ValueType == EVCBlendValueType::Value",
			EditConditionHides))
	float Value = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value",
		meta=(EditCondition = "BlendMode != EVCBlendMode::Inherit && BlendMode != EVCBlendMode::Lerp && ValueType == EVCBlendValueType::Curve",
			EditConditionHides))
	UCurveFloat* CurveValue = nullptr;

	FVCBlendFloat(){}
};

USTRUCT(BlueprintType)
struct FVCBlendVector
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVCBlendMode BlendMode = EVCBlendMode::Inherit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(EditCondition = "BlendMode != EVCBlendMode::Inherit", EditConditionHides))
	EVCBlendCoordinate CoordinateType = EVCBlendCoordinate::World;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(EditCondition = "BlendMode != EVCBlendMode::Inherit && BlendMode != EVCBlendMode::Lerp", EditConditionHides))
	EVCBlendValueType ValueType = EVCBlendValueType::Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value",
		meta=(EditCondition = "BlendMode != EVCBlendMode::Inherit", EditConditionHides))
	bool IgnoreX = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value",
		meta=(EditCondition = "BlendMode != EVCBlendMode::Inherit", EditConditionHides))
	bool IgnoreY = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value",
		meta=(EditCondition = "BlendMode != EVCBlendMode::Inherit", EditConditionHides))
	bool IgnoreZ = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value",
		meta=(EditCondition = "BlendMode != EVCBlendMode::Inherit && ValueType == EVCBlendValueType::Value || BlendMode == EVCBlendMode::Lerp",
			EditConditionHides))
	FVector Value = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value",
		meta=(EditCondition = "BlendMode != EVCBlendMode::Inherit && BlendMode != EVCBlendMode::Lerp && ValueType == EVCBlendValueType::Curve",
			EditConditionHides))
	UCurveVector* CurveValue = nullptr;

	FVCBlendVector(){}
};

USTRUCT(BlueprintType)
struct FVCCamBlendData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BlendData Time ")
	float CurveMaxTime = 1.0f;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BlendData Target")
	EVCBlendBool LookAtTargetOffsetWithArmRot = EVCBlendBool::Inherit;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BlendData Target")
	FVCBlendVector LookAtTarget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BlendData Target ")
	EVCBlendBool EnableTargetLag = EVCBlendBool::Inherit;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BlendData Target ")
	FVCBlendFloat TargetLagSpeed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BlendData Target ")
	FVCBlendFloat LagMaxDistance;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BlendData Rot ")
	FVCBlendVector ArmRot;
	
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BlendData Rot ")
	// EVCBlendBool EnableMoveRotFix = EVCBlendBool::Inherit;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BlendData Rot ")
	// FVCBlendFloat MoveRotFixSpeed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BlendData Rot ")
	EVCBlendBool EnableArmRotLag = EVCBlendBool::Inherit;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BlendData Rot ")
	FVCBlendFloat ArmRotLagSpeed;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BlendData ArmLength ")
	FVCBlendFloat ArmLength;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BlendData CamFOV ")
	FVCBlendFloat CamFOV;
};




UCLASS(BlueprintType)
class DLVIRTUALCAMERASYSTEM_API UVCDataAssetDef
	: public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UVCProcessor> PreProcessor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UVCProcessor> PostProcessor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVCCamBlendData CamBlendData;
};

inline FVector RotToVec(FRotator Rot)
{
	return FVector(0, Rot.Pitch, Rot.Yaw);
};

inline FRotator VecToRot(FVector Vec)
{
	return FRotator(Vec.Y, Vec.Z, 0);
};
