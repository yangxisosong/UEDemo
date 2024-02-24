// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLCommonBlueprintFunctionLibrary.generated.h"

/**
 *
 */
UCLASS()
class DLKIT_API UDLCommonBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DLKit")
		static bool K2_EnsureMsg(const bool bIsValid, const FString Category = "Tmp", const FString Msg = "", bool bIsAlways = false);

	UFUNCTION(BlueprintCallable, Category = "DLKit")
		static bool K2_EnsureObjectMsg(const UObject* TargetObject, const FString Category = "Tmp", const FString Msg = "", bool bIsAlways = false);

	/**
	 * @brief 检测一个点是否在三角形内
	 * @param TargetPoint
	 * @param Point1
	 * @param Point2
	 * @param Point3
	 * @return
	 */
	UFUNCTION(BlueprintPure, Category = "DLKit", DisplayName = "CheckIsPointInTriangle")
		static bool K2_CheckIsPointInTriangle(const FVector2D& TargetPoint, const FVector2D& Point1, const FVector2D& Point2
			, const FVector2D& Point3);

	/**
	 * @brief 检测一个点是否在三角形内
	 * @param TargetPoint
	 * @param Point1
	 * @param Point2
	 * @param Point3
	 * @return
	 */
	UFUNCTION(BlueprintPure, Category = "DLKit", DisplayName = "CheckIsPointInTriangle")
		static bool K2_CheckIsPointInTriangle_Vector(const FVector& TargetPoint, const FVector& Point1, const FVector& Point2
			, const FVector& Point3);

	UFUNCTION(BlueprintPure, Category = "DLKit", DisplayName = "PointsCrossCircle")
		static void K2_PointsCrossCircle(const FVector& LineStart, const FVector& LineEnd, const FVector& Center, const float& Radius, FVector& Point1, FVector& Point2);

	UFUNCTION(BlueprintCallable, Category = "Debug")
		static FString GetLogTimeInfo();

	UFUNCTION(BlueprintCallable, Category = "Debug")
		static bool GetIsDedicatedServer(UWorld* InWorld);

	UFUNCTION(BlueprintCallable, Category = "Debug")
		static TArray<UClass*> GetAllClass(TArray<AActor*> Actors);

	UFUNCTION(BlueprintCallable, Category = "Debug")
		static TArray<FString> GetObjectProperty(UClass* InClass);

	UFUNCTION(BlueprintCallable, Category = "Debug")
		static TArray<FString> GetObjectFunction(UClass* InClass);

	UFUNCTION(BlueprintCallable, Category = "Debug")
		static void GetTipsArray(FString InTokens, TMap<FString, FString>& OutNames, FString& OutPrevToken, FString& OutLastToken);

	UFUNCTION(BlueprintPure, Category = "DLKit")
		static FRotator VectorToRotator(const FVector& TargetVector);

	UFUNCTION(BlueprintPure, Category = "DLKit")
		static FRotator AddRotator(const FRotator& RotatorA, const FRotator& RotatorB);

	UFUNCTION(BlueprintPure, Category = "DLKit")
		static bool IsPreview();
};
