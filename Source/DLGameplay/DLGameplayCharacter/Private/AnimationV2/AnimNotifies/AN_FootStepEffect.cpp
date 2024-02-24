// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_FootStepEffect.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Animation/DLAnimationLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

void UAN_FootStepEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (!MeshComp)
	{
		return;
	}

	AActor* MeshOwner = MeshComp->GetOwner();
	if (!MeshOwner)
	{
		return;
	}

	if (HitDataTable)
	{
		const UWorld* World = MeshComp->GetWorld();
		check(World);

		const FVector FootLocation = MeshComp->GetSocketLocation(FootSocketName);
		const FRotator FootRotation = MeshComp->GetSocketRotation(FootSocketName);
		const FVector TraceEnd = FootLocation - MeshOwner->GetActorUpVector() * TraceLength;
		const UCapsuleComponent* CapsuleComp = Cast<UCapsuleComponent>(
			MeshOwner->GetComponentByClass(UCapsuleComponent::StaticClass()));
		if (!CapsuleComp)
		{
			return;
		}
		const FVector TraceStart = FootLocation + TraceStartOffset;
		FHitResult Hit;


		//要获取到重载的物理资产，这里的bTraceComplex必须填False
		if (UKismetSystemLibrary::LineTraceSingle(MeshOwner /*used by bIgnoreSelf*/, TraceStart, TraceEnd,
			TraceChannel, false /*bTraceComplex*/, MeshOwner->Children,
			DrawDebugType, Hit, true /*bIgnoreSelf*/))
		{
			if (!Hit.PhysMaterial.Get())
			{
				return;
			}

			const EPhysicalSurface SurfaceType = Hit.PhysMaterial.Get()->SurfaceType;

			check(IsInGameThread());
			checkNoRecursion();


			//第一次使用的时候，构建一个映射表
			if (FootStepFXMap.Num() == 0)
			{
				static TArray<FFootStepFX*> HitFXRows;
				HitDataTable->GetAllRows<FFootStepFX>(FString(), HitFXRows);
				for (int i = 0; i < HitFXRows.Num(); ++i)
				{
					FFootStepFX* HitFX = HitFXRows[i];
					if (HitFX)
					{
						FootStepFXMap.Add(HitFX->SurfaceType, HitFX);
					}
				}
			}

			FFootStepFX* HitFX = *FootStepFXMap.Find(SurfaceType);
			if (!HitFX)
			{
				//找一下有没有默认的
				HitFX = *FootStepFXMap.Find(EPhysicalSurface::SurfaceType_Default);
			}
			if (!HitFX)
			{
				return;
			}

			SpawnDecal(HitFX, Hit, FootRotation, MeshComp);
			SpawnSound(HitFX, Hit, MeshComp);
			SpawnParticleEffect(HitFX, Hit, FootRotation, MeshComp);
		}
	}
}

FString UAN_FootStepEffect::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("%s__%s"), *FootSocketName.ToString(),
						   *UDLAnimationLibrary::GetEnumerationToString(FootstepType));
}

void UAN_FootStepEffect::SpawnDecal(FFootStepFX* HitFX, const FHitResult& HitResult, const FRotator& FootRotation,
									USkeletalMeshComponent* MeshComp)
{
	if (bSpawnDecal && HitFX->DecalMaterial.LoadSynchronous())
	{
		const FVector Location = HitResult.Location + MeshComp->GetOwner()->GetTransform().TransformVector(
			HitFX->DecalLocationOffset);

		const FVector DecalSize = FVector(bMirrorDecalX ? -HitFX->DecalSize.X : HitFX->DecalSize.X,
										  bMirrorDecalY ? -HitFX->DecalSize.Y : HitFX->DecalSize.Y,
										  bMirrorDecalZ ? -HitFX->DecalSize.Z : HitFX->DecalSize.Z);

		UDecalComponent* SpawnedDecal = nullptr;

		SpawnedDecal = UGameplayStatics::SpawnDecalAtLocation(
			MeshComp->GetWorld(), HitFX->DecalMaterial.Get(), DecalSize, Location,
			FootRotation + HitFX->DecalRotationOffset, HitFX->DecalLifeSpan);
	}
}

void UAN_FootStepEffect::SpawnSound(FFootStepFX* HitFX, const FHitResult& HitResult, USkeletalMeshComponent* MeshComp)
{
	if (bSpawnSound && HitFX->Sound.LoadSynchronous())
	{
		const float MaskCurveValue = MeshComp->GetAnimInstance()->GetCurveValue("Mask_FootstepSound");
		const float FinalVolMult = bOverrideMaskCurve ? VolumeMultiplier : VolumeMultiplier * (1.0f - MaskCurveValue);

		UAudioComponent* SpawnedSound = UGameplayStatics::SpawnSoundAtLocation(
			MeshComp->GetWorld(), HitFX->Sound.Get(), HitResult.Location + HitFX->SoundLocationOffset,
			HitFX->SoundRotationOffset, FinalVolMult, PitchMultiplier);
		static FName FootStepTypeName = "FootStepType";
		static FName ShoeTypeName = "ShoeType";

		if (SpawnedSound)
		{
			SpawnedSound->SetIntParameter(ShoeTypeName, static_cast<int32>(ShoeType));
			SpawnedSound->SetIntParameter(FootStepTypeName, static_cast<int32>(FootstepType));
		}
	}
}

void UAN_FootStepEffect::SpawnParticleEffect(FFootStepFX* HitFX, const FHitResult& HitResult,
											 const FRotator& FootRotation,
											 USkeletalMeshComponent* MeshComp)
{
	if (bSpawnParticleEffect)
	{
		const FVector Location = HitResult.Location + MeshComp->GetOwner()->GetTransform().TransformVector(
			HitFX->DecalLocationOffset);

		UFXSystemComponent* SpawnedParticle;
		if (HitFX->bIsNiagara && HitFX->NiagaraParticleEffect.LoadSynchronous())
		{
			SpawnedParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				MeshComp->GetWorld(), HitFX->NiagaraParticleEffect.Get(), Location,
				FootRotation + HitFX->ParticleEffectRotationOffset
				, HitFX->ParticleEffectScale);
		}
		else if (HitFX->ParticleEffect.LoadSynchronous())
		{
			SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(MeshComp->GetWorld()
																	   , HitFX->ParticleEffect.Get(), Location,
																	   FootRotation + HitFX->
																	   ParticleEffectRotationOffset
																	   , HitFX->ParticleEffectScale);
		}
	}
}
