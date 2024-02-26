// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonUtilities/Public/JsonUtilities.h"
#include "Animation/AnimDef/DLAnimStruct.h"
#include "Animation/BlendSpace.h"
#include "Animation/BlendSpace1D.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DLAnimCommonLibrary.generated.h"

class UCapsuleComponent;
class UAnimInstance;
struct FDLAnimAsset;

/**
 * 动画通用蓝图函数库
 */
UCLASS()
class UDLAnimCommonLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template<typename _StructType>
	static FString ConvertUStructToJson(const _StructType& InStruct)
	{
		FString Result;
		if (!FJsonObjectConverter::UStructToJsonObjectString<_StructType>(InStruct, Result))
		{
			UE_LOG(LogTemp, Error, TEXT("UDLAnimCommonLibrary::ConvertUStructToJson failed!!"));
		}
		return Result;
	}

	template<typename _StructType>
	static bool ConvertJsonToUStruct(const FString& JsonStr, _StructType& InStruct)
	{
		if (FJsonObjectConverter::JsonObjectStringToUStruct<_StructType>(JsonStr, &InStruct))
		{
			UE_LOG(LogTemp, Error, TEXT("UDLAnimCommonLibrary::ConvertJsonToUStruct failed!!,jsonstr:%s"), *JsonStr);
			return true;
		}
		InStruct = _StructType();
		return false;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation | Tool")
		static UBlendSpace1D* AnimAssetToBlendSpace1D(const FDLAnimAsset& AnimAsset)
	{
		return Cast< UBlendSpace1D>(AnimAsset.Animation);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation | Tool")
		static UBlendSpace* AnimAssetToBlendSpace(const  FDLAnimAsset& AnimAsset)
	{
		return Cast< UBlendSpace>(AnimAsset.Animation);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation | Tool")
		static UAnimSequence* AnimAssetToAnimSeq(const  FDLAnimAsset& AnimAsset)
	{
		return Cast< UAnimSequence>(AnimAsset.Animation);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation | Tool")
		static UAnimMontage* AnimAssetToAnimMontage(const  FDLAnimAsset& AnimAsset)
	{
		return Cast<UAnimMontage>(AnimAsset.Animation);
	}

	/**
	* 返回枚举索引值。
	*/
	template <typename Enumeration>
	static FORCEINLINE int32 GetEnumerationIndex(const Enumeration InValue)
	{
		return StaticEnum<Enumeration>()->GetIndexByValue(static_cast<int64>(InValue));
	}

	/**
	* 将枚举值转换为字符串。
	*/
	template <typename Enumeration>
	static FORCEINLINE FString GetEnumerationToString(const Enumeration InValue)
	{
		return StaticEnum<Enumeration>()->GetNameStringByValue(static_cast<int64>(InValue));
	}

	/**
	* 将枚举值转换为FName。
	*/
	template <typename Enumeration>
	static FORCEINLINE FName GetEnumerationToName(const Enumeration InValue)
	{

		return  FName(StaticEnum<Enumeration>()->GetNameStringByIndex(static_cast<int64>(InValue)));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation | Tool")
		static float GetCurveValueByEnum(const UAnimInstance* AnimIns, const EDLAnimCurveName Type)
	{
		if (AnimIns)
		{
			return AnimIns->GetCurveValue(GetEnumerationToName<EDLAnimCurveName>(Type));
		}
		return 0.0f;
	}

	static TPair<float, float> FixDiagonalGamepadValues(float X, float Y);

	UFUNCTION(BlueprintCallable, Category = "Animation|Math Utils")
		static FTransform TransformSub(const FTransform& T1, const FTransform& T2)
	{
		return FTransform(T1.GetRotation().Rotator() - T2.GetRotation().Rotator(),
						  T1.GetLocation() - T2.GetLocation(), T1.GetScale3D() - T2.GetScale3D());
	}

	UFUNCTION(BlueprintCallable, Category = "Animation|Math Utils")
		static FTransform TransformAdd(const FTransform& T1, const FTransform& T2)
	{
		return FTransform(T1.GetRotation().Rotator() + T2.GetRotation().Rotator(),
						  T1.GetLocation() + T2.GetLocation(), T1.GetScale3D() + T2.GetScale3D());
	}

	UFUNCTION(BlueprintCallable, Category = "Animation|Math Utils")
		static FVector GetCapsuleBaseLocation(float ZOffset, UCapsuleComponent* Capsule);

	UFUNCTION(BlueprintCallable, Category = "Animation|Math Utils")
		static FVector GetCapsuleLocationFromBase(FVector BaseLocation, float ZOffset, UCapsuleComponent* Capsule);

	UFUNCTION(BlueprintCallable, Category = "Animation|Math Utils")
		static bool CapsuleHasRoomCheck(UCapsuleComponent* Capsule, FVector TargetLocation, float HeightOffset,
										float RadiusOffset, EDrawDebugTrace::Type DebugType = EDrawDebugTrace::Type::None, bool DrawDebugTrace = false);

	UFUNCTION(BlueprintCallable, Category = "Animation|Math Utils")
		static bool AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool IncreaseBuffer);

	UFUNCTION(BlueprintCallable, Category = "Animation|Math Utils")
		static EMovementDirection CalculateQuadrant(EMovementDirection Current, float FRThreshold, float FLThreshold,
													   float BRThreshold,
													   float BLThreshold, float Buffer, float Angle);

	UFUNCTION(BlueprintPure, Category = "Animation|Math Utils")
		static FRotator FixRootRotationBias(const FRotator& RootRotation, const FRotator& TargetRotation);

	UFUNCTION(BlueprintPure, Category = "Animation|Math Utils")
		static EMovementDirection GetMovementDirection(const FRotator& ActorRotation, const FVector& Velocity);

	UFUNCTION(BlueprintPure, Category = "Animation|Pose")
		static void GetBonePosesForTime(const UAnimSequence* AnimationSequence, TArray<FName> BoneNames, float Time, bool bExtractRootMotion, TArray<FTransform>& Poses, const USkeletalMesh* PreviewMesh = nullptr);

	UFUNCTION(BlueprintPure, Category = "Animation|Pose")
		static void GetBonePoseForTime(const UAnimSequence* AnimationSequence, FName BoneName, float Time, bool bExtractRootMotion, FTransform& Pose, const USkeletalMesh* PreviewMesh = nullptr);

	static bool IsValidTimeInternal(const UAnimSequence* AnimationSequence, const float Time);

	/** Checks whether or not the given Animation Track Name is contained within the Animation Sequence */
	static bool IsValidRawAnimationTrackName(const UAnimSequence* AnimationSequence, const FName TrackName, const FReferenceSkeleton& RefSkeleton);

	//static const FRawAnimSequenceTrack& GetRawAnimationTrackByName(const UAnimSequence* AnimationSequence, const FName TrackName, const USkeletalMesh* PreviewMesh = nullptr);

	UFUNCTION(BlueprintPure, Category = "Animation|Tool")
		static int32 GetBoneIndex(const UAnimSequence* AnimationSequence, FName BoneName, const USkeletalMesh* PreviewMesh = nullptr);

	UFUNCTION(BlueprintPure, Category = "Animation|Tool")
		static FVector ClampedVectorToMaxSize(const FVector& Target,float MaxSize);

};
