// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DLAnimationDebugComponent.generated.h"


class IDLAnimDebugInterface;
UCLASS(Blueprintable, BlueprintType)
class DLGAMEPLAYCORE_API UDLAnimationDebugComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDLAnimationDebugComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Implemented on BP to update layering colors */
	UFUNCTION(BlueprintImplementableEvent, Category = "Animation|Debug")
		void UpdateColoringSystem();

	/** Implement on BP to draw debug spheres */
	UFUNCTION(BlueprintImplementableEvent, Category = "Animation|Debug")
		void DrawDebugSpheres();

	/** Implemented on BP to set/reset layering colors */
	UFUNCTION(BlueprintImplementableEvent, Category = "Animation|Debug")
		void SetResetColors();

	/** Implemented on BP to set dynamic color materials for debugging */
	UFUNCTION(BlueprintImplementableEvent, Category = "Animation|Debug")
		void SetDynamicMaterials();

	UFUNCTION(BlueprintImplementableEvent)
		void OpenDebugView();

	UFUNCTION(BlueprintImplementableEvent)
		void CloseDebugView();

	UFUNCTION(BlueprintImplementableEvent)
		void UpdateDebugView();

	UFUNCTION(BlueprintPure)
		static UDLAnimationDebugComponent* GetCurrentActiveAnimationDebugComp();
private:
	UFUNCTION(BlueprintPure)
		TScriptInterface<IDLAnimDebugInterface> GetDebugTarget() const;

private:
	bool bNeedsColorReset = false;

	bool bDebugMeshVisible = false;
private:
	UPROPERTY()
		USkeletalMesh* DefaultSkeletalMesh = nullptr;
};
