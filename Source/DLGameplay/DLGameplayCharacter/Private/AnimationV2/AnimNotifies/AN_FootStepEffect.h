// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationV2/DLAnimCommonDef.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AN_FootStepEffect.generated.h"

class UNiagaraSystem;


/**
* 鞋子类型
*/
UENUM(BlueprintType)
enum class EDLShoeType : uint8
{
	Default		UMETA(ToolTip = "默认"),
	HighHeel	UMETA(ToolTip = "高跟鞋"),
	FlatShoes	UMETA(ToolTip = "平底鞋"),
};

USTRUCT(BlueprintType)
struct FFootStepFX : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Surface")
		TEnumAsByte<enum EPhysicalSurface> SurfaceType = SurfaceType_Default;

	UPROPERTY(EditAnywhere, Category = "Sound")
		TSoftObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, Category = "Sound")
		FVector SoundLocationOffset;

	UPROPERTY(EditAnywhere, Category = "Sound")
		FRotator SoundRotationOffset;

	UPROPERTY(EditAnywhere, Category = "Decal")
		TSoftObjectPtr<UMaterialInterface> DecalMaterial;

	UPROPERTY(EditAnywhere, Category = "Decal")
		float DecalLifeSpan = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Decal")
		FVector DecalSize;

	UPROPERTY(EditAnywhere, Category = "Decal")
		FVector DecalLocationOffset;

	UPROPERTY(EditAnywhere, Category = "Decal")
		FRotator DecalRotationOffset;

	UPROPERTY(EditAnywhere, Category = "ParticleEffect")
		bool bIsNiagara = false;

	UPROPERTY(EditAnywhere, Category = "ParticleEffect", meta = (EditCondition = "!bIsNiagara"))
		TSoftObjectPtr<UParticleSystem> ParticleEffect;

	UPROPERTY(EditAnywhere, Category = "ParticleEffect", meta = (EditCondition = "bIsNiagara"))
		TSoftObjectPtr<UNiagaraSystem> NiagaraParticleEffect;

	UPROPERTY(EditAnywhere, Category = "ParticleEffect")
		TEnumAsByte<enum EAttachLocation::Type> ParticleEffectAttachmentType;

	UPROPERTY(EditAnywhere, Category = "ParticleEffect")
		FVector ParticleEffectLocationOffset;

	UPROPERTY(EditAnywhere, Category = "ParticleEffect")
		FRotator ParticleEffectRotationOffset;

	UPROPERTY(EditAnywhere, Category = "ParticleEffect")
		FVector ParticleEffectScale = FVector(1.0f);
};

/**
 *
 */
UCLASS()
class DLGAMEPLAYCHARACTER_API UAN_FootStepEffect : public UAnimNotify
{
	GENERATED_BODY()
public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual FString GetNotifyName_Implementation() const override;

private:
	void SpawnDecal(FFootStepFX* HitFX, const FHitResult& HitResult, const FRotator& FootRotation, USkeletalMeshComponent* MeshComp);
	void SpawnSound(FFootStepFX* HitFX, const FHitResult& HitResult, USkeletalMeshComponent* MeshComp);
	void SpawnParticleEffect(FFootStepFX* HitFX, const FHitResult& HitResult, const FRotator& FootRotation,
					  USkeletalMeshComponent* MeshComp);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		UDataTable* HitDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Socket")
		FName FootSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
		TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;

	UPROPERTY(EditAnywhere, Category = "Trace")
		FVector TraceStartOffset = FVector(0.0f, 0.0f, 100.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
		TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
		float TraceLength = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
		bool bSpawnDecal = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
		bool bMirrorDecalX = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
		bool bMirrorDecalY = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
		bool bMirrorDecalZ = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		bool bSpawnSound = true;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	// 	FName SoundParameterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		EDLFootstepType FootstepType = EDLFootstepType::Step;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		EDLShoeType ShoeType = EDLShoeType::Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		bool bOverrideMaskCurve = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParticleEffect")
		bool bSpawnParticleEffect = false;

private:
	TMap<EPhysicalSurface, FFootStepFX*> FootStepFXMap;
};
