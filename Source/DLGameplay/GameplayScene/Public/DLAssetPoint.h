// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/Actor.h"
#include "DLAssetPoint.generated.h"



UCLASS()
class GAMEPLAYSCENE_API ADLAssetPoint : public AActor
{
	GENERATED_BODY()

public:

	
	UPROPERTY(EditAnywhere)
		TArray<FPrimaryAssetId> PrimaryAssetIds;

	UPROPERTY(EditAnywhere)
		TArray<FName> LoadBundles;

protected:

	virtual void OnLoadAssetComplate();

	virtual void OnUnloadAsset();


protected:

	void StartLoadAsset();

	void UnLoadAsset();

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	TSharedPtr<FStreamableHandle> StreamableHandle;
};
