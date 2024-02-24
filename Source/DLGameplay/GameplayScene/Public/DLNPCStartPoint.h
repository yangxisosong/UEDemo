// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLAssetPoint.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerStart.h"
#include "DLNPCStartPoint.generated.h"

class ADLCharacterBase;
UCLASS()
class GAMEPLAYSCENE_API ADLNPCStartPoint : public APlayerStart
{
	GENERATED_BODY()

public:

	ADLNPCStartPoint(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void StartLoadCharacterAsset();

	void UnLoadAsset();

	void OnLoadAssetComplate(FPrimaryAssetId AssetId);

	void OnUnloadAsset();

public:

	UPROPERTY(EditAnywhere)
		FName StartPointId;

	UPROPERTY(VisibleAnywhere)
		FName NpcCharacterId;

	UPROPERTY(EditAnywhere, meta=(GetOptions=GetNPCNameOption))
		FString NPCName;

	UFUNCTION(meta = (CallInEditor = "true"))
		TArray<FName> GetNPCNameOption() const;

public:

	TSharedPtr<FStreamableHandle> StreamableHandle;

	TWeakObjectPtr<ADLCharacterBase> HoldCharacter;

protected:

#if	WITH_EDITORONLY_DATA

	UPROPERTY()
		USkeletalMeshComponent* PreviewMesh;

#endif

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif


};
