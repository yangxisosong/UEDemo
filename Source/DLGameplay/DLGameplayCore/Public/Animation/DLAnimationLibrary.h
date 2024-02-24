// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonUtilities/Public/JsonUtilities.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DLGameplayCore/Public/Animation/DLAnimBaseDef.h"
#include "DLAnimationLibrary.generated.h"

class UCapsuleComponent;
class UAnimInstance;


/**
 *
 */
USTRUCT(BlueprintType)
struct FAnimationAssetFamily
{
	GENERATED_BODY()
public:
	/** The skeleton that links all assets */
	UPROPERTY(BlueprintReadOnly)
		TArray< USkeleton*> Skeletons;

	/** The last mesh that was encountered */
	UPROPERTY(BlueprintReadOnly)
		TArray< USkeletalMesh*> Meshes;

	/** The last anim blueprint that was encountered */
	UPROPERTY(BlueprintReadOnly)
		TArray< UAnimBlueprint*> AnimBlueprints;

	/** The last animation asset that was encountered */
	UPROPERTY(BlueprintReadOnly)
		TArray< UAnimationAsset*> AnimationAssets;

	/** The last animation asset that was encountered */
	UPROPERTY(BlueprintReadOnly)
		TArray< UAnimMontage*> AnimMontages;

	/** The last physics asset that was encountered */
	UPROPERTY(BlueprintReadOnly)
		TArray< UPhysicsAsset*> PhysicsAssets;
};

/**
 * 动画通用蓝图函数库
 */
UCLASS()
class DLGAMEPLAYCORE_API UDLAnimationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template<typename _StructType>
	static FString ConvertUStructToJson(const _StructType& InStruct)
	{
		FString Result;
		if (!FJsonObjectConverter::UStructToJsonObjectString<_StructType>(InStruct, Result))
		{
			UE_LOG(LogTemp, Error, TEXT("UDLAnimationLibrary::ConvertUStructToJson failed!!"));
		}
		return Result;
	}

	template<typename _StructType>
	static bool ConvertJsonToUStruct(const FString& JsonStr, _StructType& InStruct)
	{
		if (FJsonObjectConverter::JsonObjectStringToUStruct<_StructType>(JsonStr, &InStruct))
		{
			UE_LOG(LogTemp, Error, TEXT("UDLAnimationLibrary::ConvertJsonToUStruct failed!!,jsonstr:%s"), *JsonStr);
			return true;
		}
		InStruct = _StructType();
		return false;
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

	static TPair<float, float> FixDiagonalGamepadValues(float X, float Y);

	UFUNCTION(BlueprintCallable, Category = "Animation|Math Utils")
		static FTransform TransfromSub(const FTransform& T1, const FTransform& T2)
	{
		return FTransform(T1.GetRotation().Rotator() - T2.GetRotation().Rotator(),
						  T1.GetLocation() - T2.GetLocation(), T1.GetScale3D() - T2.GetScale3D());
	}

	UFUNCTION(BlueprintCallable, Category = "Animation|Math Utils")
		static FTransform TransfromAdd(const FTransform& T1, const FTransform& T2)
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

	UFUNCTION(BlueprintPure, Category = "Animation|Math Utils")
		static bool AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool IncreaseBuffer);

	UFUNCTION(BlueprintPure, Category = "Animation|Tool")
		static FVector ClampedVectorToMaxSize(const FVector& Target, float MaxSize);

	UFUNCTION(BlueprintPure, Category = "Animation|Math Utils")
		static EAnimMovementDirection CalculateRelativeDirection(const FRotator& Rotation, const FVector& Velocity);

	UFUNCTION(BlueprintCallable, Category = "Animation|Math Utils")
		static EAnimMovementDirection CalculateQuadrant(EAnimMovementDirection Current, float FRThreshold, float FLThreshold,
													   float BRThreshold,
													   float BLThreshold, float Buffer, float Angle);

	/** 获取动画实例中所有活跃的蒙太奇 */
	UFUNCTION(BlueprintPure, Category = "AnimationBlueprintLibrary|Helpers")
		static void GetAllActiveMontages(class UAnimInstance* TargetAnimInstance, TArray<class UAnimMontage*>& ActiveMontages);

	/** 通过名字获取蒙太奇中Section的长度 */
	UFUNCTION(BlueprintPure, Category = "AnimationBlueprintLibrary|Helpers")
		static float GetAnimMontageSectionLengthByName(const class UAnimMontage* AnimMontage, const FName SectionName);


	UFUNCTION(BlueprintCallable, Category = "AnimationBlueprintLibrary|Helpers")
		static UAnimMontage* SetMontageSlot(FName SlotName, const class UAnimMontage* Montage);

	UFUNCTION(BlueprintPure, Category = "AnimationBlueprintLibrary|Helpers")
		static void FindAssetFamily(const USkeletalMesh* SkeletalMesh, FAnimationAssetFamily& OutAssetFamily);

	UFUNCTION(BlueprintPure)
		static void FilterOutTargetAssets(const UClass* AssetClass, const FName& Tag, const FString& Value, TArray<UObject*>& Assets);

	UFUNCTION(BlueprintPure)
		static TSubclassOf<UAnimInstance> GetAnimClassFromAnimBlueprint(UObject* AnimBlueprint);

	/*检测蒙太奇是否能在对应动画实例上播放*/
	UFUNCTION(BlueprintPure)
		static bool IsMontagePlayable(const UAnimInstance* AnimIns, const UAnimMontage* AnimMontage);

	/*检测蒙太奇是否与mesh兼容*/
	UFUNCTION(BlueprintPure)
		static bool IsMontageCompatibleWithMesh(const USkeletalMesh* Mesh, const UAnimMontage* AnimMontage);

	UFUNCTION(BlueprintPure)
		static bool IsAnimCompatibleWithMesh(const USkeletalMesh* Mesh, const UAnimSequenceBase* Anim);
};

