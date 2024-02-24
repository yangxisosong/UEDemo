// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLDrawDebugDef.h"
#include "DLDrawDebugSubsystem.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class DLDRAWDEBUGSYSTEM_API UDLDrawDebugSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		void ShowCategory(const EDrawDebugCategory Category);

	UFUNCTION(BlueprintCallable)
		void ShowCategory_CustomCategory(FString Category);

	UFUNCTION(BlueprintCallable)
		void HideCategory(const EDrawDebugCategory Category);

	UFUNCTION(BlueprintCallable)
		void HideCategory_CustomCategory(FString Category);

	UFUNCTION(BlueprintCallable)
		void HideAllCategories();

	UFUNCTION(BlueprintCallable)
		void ShowAllCategories();

public:

	UFUNCTION(BlueprintCallable)
		void DrawDebugArrow(const EDrawDebugCategory Category, const FVector LineStart, const FVector LineEnd, float ArrowSize, FLinearColor LineColor, float Duration = 0.f, float Thickness = 0.f);

	UFUNCTION(BlueprintCallable)
		void DrawDebugArrow_CustomCategory(FString Category , const FVector LineStart, const FVector LineEnd, float ArrowSize, FLinearColor LineColor, float Duration = 0.f, float Thickness = 0.f);

	UFUNCTION(BlueprintCallable)
		void DrawDebugPoint(const EDrawDebugCategory Category, const FVector Position, float Size, FLinearColor PointColor, float Duration = 0.f);

	UFUNCTION(BlueprintCallable)
		void DrawDebugPoint_CustomCategory(FString Category , const FVector Position, float Size, FLinearColor PointColor, float Duration = 0.f);

	UFUNCTION(BlueprintCallable)
		void DrawDebugBox(const EDrawDebugCategory Category, const FVector Center, FVector Extent, FLinearColor LineColor, const FRotator Rotation = FRotator::ZeroRotator, float Duration = 0.f, float Thickness = 0.f);

	UFUNCTION(BlueprintCallable)
		void DrawDebugBox_CustomCategory(FString Category , const FVector Center, FVector Extent, FLinearColor LineColor, const FRotator Rotation = FRotator::ZeroRotator, float Duration = 0.f, float Thickness = 0.f);

	UFUNCTION(BlueprintCallable)
		void DrawDebugSphere(const EDrawDebugCategory Category, const FVector Center, float Radius = 100.f, int32 Segments = 12, FLinearColor LineColor = FLinearColor::White, float Duration = 0.f, float Thickness = 0.f);

	UFUNCTION(BlueprintCallable)
		void DrawDebugSphere_CustomCategory(FString Category , const FVector Center, float Radius = 100.f, int32 Segments = 12, FLinearColor LineColor = FLinearColor::White, float Duration = 0.f, float Thickness = 0.f);

	UFUNCTION(BlueprintCallable)
		void DrawDebugCapsule(const EDrawDebugCategory Category, const FVector Center, float HalfHeight, float Radius, const FRotator Rotation, FLinearColor LineColor = FLinearColor::White, float Duration = 0.f, float Thickness = 0.f);

	UFUNCTION(BlueprintCallable)
		void DrawDebugCapsule_CustomCategory(FString Category , const FVector Center, float HalfHeight, float Radius, const FRotator Rotation, FLinearColor LineColor = FLinearColor::White, float Duration = 0.f, float Thickness = 0.f);

	UFUNCTION(BlueprintCallable)
		void DrawDebugLine(const EDrawDebugCategory Category, const FVector LineStart, const FVector LineEnd, FLinearColor LineColor, float Duration = 0.f, float Thickness = 0.f);

	UFUNCTION(BlueprintCallable)
		void DrawDebugLine_CustomCategory(FString Category , const FVector LineStart, const FVector LineEnd, FLinearColor LineColor, float Duration = 0.f, float Thickness = 0.f);

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
};
