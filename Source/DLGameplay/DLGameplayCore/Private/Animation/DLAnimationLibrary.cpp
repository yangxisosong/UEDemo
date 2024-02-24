// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/DLAnimationLibrary.h"
#include "Components/CapsuleComponent.h"
#include "AssetRegistry/AssetRegistryModule.h"

TPair<float, float> UDLAnimationLibrary::FixDiagonalGamepadValues(float X, float Y)
{
	float ResultY = X * FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 0.6f),
													  FVector2D(1.0f, 1.2f), FMath::Abs(Y));
	ResultY = FMath::Clamp(ResultY, -1.0f, 1.0f);
	float ResultX = Y * FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 0.6f),
														  FVector2D(1.0f, 1.2f), FMath::Abs(X));
	ResultX = FMath::Clamp(ResultX, -1.0f, 1.0f);
	return TPair<float, float>(ResultY, ResultX);
}

FVector UDLAnimationLibrary::GetCapsuleBaseLocation(float ZOffset, UCapsuleComponent* Capsule)
{
	return Capsule->GetComponentLocation() -
		Capsule->GetUpVector() * (Capsule->GetScaledCapsuleHalfHeight() + ZOffset);
}

FVector UDLAnimationLibrary::GetCapsuleLocationFromBase(FVector BaseLocation, float ZOffset, UCapsuleComponent* Capsule)
{
	BaseLocation.Z += Capsule->GetScaledCapsuleHalfHeight() + ZOffset;
	return BaseLocation;
}

bool UDLAnimationLibrary::CapsuleHasRoomCheck(UCapsuleComponent* Capsule, FVector TargetLocation, float HeightOffset,
	float RadiusOffset, EDrawDebugTrace::Type DebugType, bool DrawDebugTrace)
{
	const float ZTarget = Capsule->GetScaledCapsuleHalfHeight_WithoutHemisphere() - RadiusOffset + HeightOffset;
	FVector TraceStart = TargetLocation;
	TraceStart.Z += ZTarget;
	FVector TraceEnd = TargetLocation;
	TraceEnd.Z -= ZTarget;
	const float Radius = Capsule->GetUnscaledCapsuleRadius() + RadiusOffset;

	const UWorld* World = Capsule->GetWorld();
	check(World);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Capsule->GetOwner());

	FHitResult HitResult;
	const FCollisionShape SphereCollisionShape = FCollisionShape::MakeSphere(Radius);
	const bool bHit = World->SweepSingleByChannel(HitResult, TraceStart, TraceEnd, FQuat::Identity,
												  ECC_Visibility, FCollisionShape::MakeSphere(Radius), Params);

	// if (DrawDebugTrace)
	// {
	// 	UALSDebugComponent::DrawDebugSphereTraceSingle(World,
	// 												   TraceStart,
	// 												   TraceEnd,
	// 												   SphereCollisionShape,
	// 												   DebugType,
	// 												   bHit,
	// 												   HitResult,
	// 												   FLinearColor(0.130706f, 0.896269f, 0.144582f, 1.0f),  // light green
	// 												   FLinearColor(0.932733f, 0.29136f, 1.0f, 1.0f),        // light purple
	// 												   1.0f);
	// }

	return !(HitResult.bBlockingHit || HitResult.bStartPenetrating);
}

bool UDLAnimationLibrary::AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool IncreaseBuffer)
{
	if (IncreaseBuffer)
	{
		return Angle >= MinAngle - Buffer && Angle <= MaxAngle + Buffer;
	}
	return Angle >= MinAngle + Buffer && Angle <= MaxAngle - Buffer;
}

FVector UDLAnimationLibrary::ClampedVectorToMaxSize(const FVector& Target, float MaxSize)
{
	return Target.GetClampedToMaxSize(MaxSize);

}

EAnimMovementDirection UDLAnimationLibrary::CalculateRelativeDirection(const FRotator& Rotation,
	const FVector& Velocity)
{

	const FVector LocRelativeVelocityDir = Rotation.UnrotateVector(Velocity.GetSafeNormal(0.1f));

	const float Sum = FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y) +
		FMath::Abs(LocRelativeVelocityDir.Z);
	const FVector RelativeDir = LocRelativeVelocityDir / Sum;

	struct VelocityBlend
	{
		float Forward = 0.0f;
		float Backward = 0.0f;
		float Left = 0.0f;
		float Right = 0.0f;
	};

	VelocityBlend Result;
	Result.Forward = FMath::Clamp(RelativeDir.X, 0.0f, 1.0f);
	Result.Backward = FMath::Abs(FMath::Clamp(RelativeDir.X, -1.0f, 0.0f));
	Result.Left = FMath::Abs(FMath::Clamp(RelativeDir.Y, -1.0f, 0.0f));
	Result.Right = FMath::Clamp(RelativeDir.Y, 0.0f, 1.0f);

	if (Result.Forward > 0.5f)
	{
		return EAnimMovementDirection::Forward;
	}
	else if (Result.Backward > 0.5f)
	{
		return EAnimMovementDirection::Backward;
	}
	else if (Result.Left > 0.5f)
	{
		return EAnimMovementDirection::Left;
	}
	else if (Result.Right > 0.5f)
	{
		return EAnimMovementDirection::Right;
	}
	return EAnimMovementDirection::Forward;

}

EAnimMovementDirection UDLAnimationLibrary::CalculateQuadrant(EAnimMovementDirection Current, float FRThreshold,
	float FLThreshold, float BRThreshold, float BLThreshold, float Buffer, float Angle)
{
	if (AngleInRange(Angle, FLThreshold, FRThreshold, Buffer,
		Current != EAnimMovementDirection::Forward || Current != EAnimMovementDirection::Backward))
	{
		return EAnimMovementDirection::Forward;
	}

	if (AngleInRange(Angle, FRThreshold, BRThreshold, Buffer,
		Current != EAnimMovementDirection::Right || Current != EAnimMovementDirection::Left))
	{
		return EAnimMovementDirection::Right;
	}

	if (AngleInRange(Angle, BLThreshold, FLThreshold, Buffer,
		Current != EAnimMovementDirection::Right || Current != EAnimMovementDirection::Left))
	{
		return EAnimMovementDirection::Left;
	}

	return EAnimMovementDirection::Backward;

}

void UDLAnimationLibrary::GetAllActiveMontages(UAnimInstance* TargetAnimInstance, TArray<UAnimMontage*>& ActiveMontages)
{
	if (!ensureAlwaysMsgf(TargetAnimInstance, TEXT("目标动画实例为空！！")))
	{
		return;
	}
	// Start from end, as most recent instances are added at the end of the queue.
	int32 const NumInstances = TargetAnimInstance->MontageInstances.Num();
	for (int32 InstanceIndex = NumInstances - 1; InstanceIndex >= 0; InstanceIndex--)
	{
		const FAnimMontageInstance* MontageInstance = TargetAnimInstance->MontageInstances[InstanceIndex];
		if (MontageInstance && MontageInstance->IsActive())
		{
			ActiveMontages.Add(MontageInstance->Montage);
		}
	}
}

float UDLAnimationLibrary::GetAnimMontageSectionLengthByName(const UAnimMontage* AnimMontage, const FName SectionName)
{
	if (!ensureMsgf(AnimMontage, TEXT("AnimMontage 为空！")))
	{
		return 0.0f;
	}

	const auto SectionIndex = AnimMontage->GetSectionIndex(SectionName);
	if (SectionIndex == INDEX_NONE)
	{
		UE_LOG(LogAnimation, Log, TEXT("在蒙太奇%s中未找到Section%s"), *AnimMontage->GetPathName(), *SectionName.ToString());
		return 0.0f;
	}
	return AnimMontage->GetSectionLength(SectionIndex);
}

UAnimMontage* UDLAnimationLibrary::SetMontageSlot(FName SlotName, const UAnimMontage* Montage)
{
	if (!ensureMsgf(Montage, TEXT("AnimMontage 为空！")))
	{
		return nullptr;
	}
	if (!ensureMsgf(SlotName != NAME_None, TEXT("SlotName 为空！")))
	{
		return nullptr;
	}
	UAnimMontage* NewMontage = DuplicateObject<UAnimMontage>(Montage, nullptr);

	auto& AnimTracks = NewMontage->SlotAnimTracks;
	for (auto& Track : AnimTracks)
	{
		Track.SlotName = SlotName;
	}
	return NewMontage;
}

template<typename AssetType>
static void FindAssets(const USkeleton* InSkeleton, TArray<FAssetData>& OutAssetData, FName SkeletonTag)
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FARFilter Filter;
	Filter.bRecursiveClasses = true;
	Filter.ClassNames.Add(AssetType::StaticClass()->GetFName());
	Filter.TagsAndValues.Add(SkeletonTag, FAssetData(InSkeleton).GetExportTextName());

	AssetRegistryModule.Get().GetAssets(Filter, OutAssetData);
}

static void FindAssetsOfType(const USkeleton* Skeleton, const USkeletalMesh* Mesh, const UClass* InAssetClass, TArray<FAssetData>& OutAssets)
{
	if (InAssetClass)
	{
		if (InAssetClass->IsChildOf<USkeleton>())
		{
			// we should always have a skeleton here, this asset family is based on it
			OutAssets.Add(FAssetData(Skeleton));
		}
		else if (InAssetClass->IsChildOf<UAnimationAsset>())
		{
			FindAssets<UAnimationAsset>(Skeleton, OutAssets, "Skeleton");
		}
		else if (InAssetClass->IsChildOf<USkeletalMesh>())
		{
			FindAssets<USkeletalMesh>(Skeleton, OutAssets, "Skeleton");
		}
		else if (InAssetClass->IsChildOf<UAnimBlueprint>())
		{
			FindAssets<UAnimBlueprint>(Skeleton, OutAssets, "TargetSkeleton");
		}
		else if (InAssetClass->IsChildOf<UPhysicsAsset>())
		{
			const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
			FARFilter Filter;
			Filter.bRecursiveClasses = true;
			Filter.ClassNames.Add(UPhysicsAsset::StaticClass()->GetFName());
#if WITH_EDITOR
			if (Mesh != nullptr)
			{
				Filter.TagsAndValues.Add(GET_MEMBER_NAME_CHECKED(UPhysicsAsset, PreviewSkeletalMesh), FAssetData(Mesh).ObjectPath.ToString());
			}
#endif
			AssetRegistryModule.Get().GetAssets(Filter, OutAssets);
		}
	}
}

void UDLAnimationLibrary::FindAssetFamily(const USkeletalMesh* SkeletalMesh, FAnimationAssetFamily& OutAssetFamily)
{
	OutAssetFamily = FAnimationAssetFamily();
	if (!SkeletalMesh || !SkeletalMesh->GetSkeleton())
	{
		return;
	}

	TArray<FAssetData> Assets;
	FindAssetsOfType(SkeletalMesh->GetSkeleton(), nullptr, USkeleton::StaticClass(), Assets);
	for (auto Asset : Assets)
	{
		OutAssetFamily.Skeletons.Add(Cast<USkeleton>(Asset.GetAsset()));
	}

	FindAssetsOfType(SkeletalMesh->GetSkeleton(), nullptr, USkeletalMesh::StaticClass(), Assets);
	for (auto Asset : Assets)
	{
		OutAssetFamily.Meshes.Add(Cast<USkeletalMesh>(Asset.GetAsset()));
	}

	FindAssetsOfType(SkeletalMesh->GetSkeleton(), nullptr, UAnimBlueprint::StaticClass(), Assets);
	for (auto Asset : Assets)
	{
		OutAssetFamily.AnimBlueprints.Add(Cast<UAnimBlueprint>(Asset.GetAsset()));
	}

	FindAssetsOfType(SkeletalMesh->GetSkeleton(), nullptr, UAnimationAsset::StaticClass(), Assets);
	for (auto Asset : Assets)
	{
		OutAssetFamily.AnimationAssets.Add(Cast<UAnimationAsset>(Asset.GetAsset()));

		UObject* AnimMontage = Asset.GetAsset();

		if (AnimMontage && AnimMontage->IsA(UAnimMontage::StaticClass()))
		{
			OutAssetFamily.AnimMontages.Add(Cast<UAnimMontage>(AnimMontage));

		}
	}

	FindAssetsOfType(SkeletalMesh->GetSkeleton(), SkeletalMesh, UPhysicsAsset::StaticClass(), Assets);
	for (auto Asset : Assets)
	{
		OutAssetFamily.PhysicsAssets.Add(Cast<UPhysicsAsset>(Asset.GetAsset()));
	}
}

void UDLAnimationLibrary::FilterOutTargetAssets(const UClass* AssetClass, const FName& Tag,
	const FString& Value, TArray<UObject*>& Assets)
{
	FARFilter Filter;
	Filter.ClassNames.Add(AssetClass->GetFName());

	Filter.bRecursiveClasses = true;
	if (!Tag.IsNone() && !Value.IsEmpty())
	{
		Filter.TagsAndValues.Add(Tag, Value);
	}

	TArray<FAssetData> AssetList;
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

	for (auto Asset : AssetList)
	{
		Assets.Add(Asset.GetAsset());
	}
}

TSubclassOf<UAnimInstance> UDLAnimationLibrary::GetAnimClassFromAnimBlueprint(UObject* AnimBlueprint)
{
	const UAnimBlueprint* Blueprint = Cast<UAnimBlueprint>(AnimBlueprint);
	if (!Blueprint)
	{
		return nullptr;
	}
	return IAnimClassInterface::GetActualAnimClass(Blueprint->GetAnimBlueprintGeneratedClass());
}

bool UDLAnimationLibrary::IsMontagePlayable(const UAnimInstance* AnimIns, const UAnimMontage* AnimMontage)
{
	if (!AnimIns || !AnimMontage)
	{
		UE_LOG(LogTemp, Log, TEXT("<UDLAnimationLibrary::IsMontagePlayable> 动画实例或蒙太奇为null"));
		return false;
	}

	//检测骨骼是否兼容
	if (!AnimIns->CurrentSkeleton)
	{
		UE_LOG(LogTemp, Log, TEXT("<UDLAnimationLibrary::IsMontagePlayable> 动画实例:%s,没有设置骨骼"), *AnimIns->GetName());
		return false;
	}

	if (!AnimIns->CurrentSkeleton->IsCompatible(AnimMontage->GetSkeleton()))

	{
		UE_LOG(LogTemp, Log, TEXT("<UDLAnimationLibrary::IsMontagePlayable> 动画实例%s的骨骼与蒙太奇%s的骨骼不兼容")
			, *AnimIns->GetName(), *AnimMontage->GetName());
		return false;
	}

	//检测插槽是否有效
	const auto& SlotAnimTracks = AnimMontage->SlotAnimTracks;
	for (auto SlotTrack : SlotAnimTracks)
	{
		if (!AnimIns->CurrentSkeleton->ContainsSlotName(SlotTrack.SlotName))
		{
			UE_LOG(LogTemp, Log, TEXT("<UDLAnimationLibrary::IsMontagePlayable> 骨骼%s不存在%s插槽")
			, *AnimIns->CurrentSkeleton->GetName(), *SlotTrack.SlotName.ToString());
			return false;
		}
	}

	//检测动画时长是否有效
	if (FMath::IsNearlyZero(AnimMontage->SequenceLength))
	{
		UE_LOG(LogTemp, Log, TEXT("<UDLAnimationLibrary::IsMontagePlayable> 蒙太奇%s时长为0")
			, *AnimMontage->GetName());
		return false;
	}

	return true;
}

bool UDLAnimationLibrary::IsMontageCompatibleWithMesh(const USkeletalMesh* Mesh, const UAnimMontage* AnimMontage)
{
	if (!IsAnimCompatibleWithMesh(Mesh, AnimMontage))
	{
		UE_LOG(LogTemp, Log, TEXT("<UDLAnimationLibrary::IsMontageCompatibleWithMesh> 蒙太奇或Mesh为nullptr"));
		return false;
	}

	//检测插槽是否有效
	const auto& SlotAnimTracks = AnimMontage->SlotAnimTracks;
	for (auto SlotTrack : SlotAnimTracks)
	{
		if (!Mesh->GetSkeleton()->ContainsSlotName(SlotTrack.SlotName))
		{
			UE_LOG(LogTemp, Log, TEXT("<UDLAnimationLibrary::IsMontageCompatibleWithMesh> 骨骼%s不存在%s插槽")
			, *Mesh->GetSkeleton()->GetName(), *SlotTrack.SlotName.ToString());
			return false;
		}
	}

	return true;
}

bool UDLAnimationLibrary::IsAnimCompatibleWithMesh(const USkeletalMesh* Mesh, const UAnimSequenceBase* Anim)
{
	if (!Mesh || !Anim)
	{
		UE_LOG(LogTemp, Log, TEXT("<UDLAnimationLibrary::IsAnimCompatibleWithMesh> 蒙太奇或Mesh为nullptr"));
		return false;
	}

	//检测骨骼是否兼容
	if (!Mesh->GetSkeleton())
	{
		UE_LOG(LogTemp, Log, TEXT("<UDLAnimationLibrary::IsAnimCompatibleWithMesh> mesh:%s,没有设置骨骼"), *Mesh->GetName());
		return false;
	}

	if (!Mesh->GetSkeleton()->IsCompatible(Anim->GetSkeleton()))
	{
		UE_LOG(LogTemp, Log, TEXT("<UDLAnimationLibrary::IsAnimCompatibleWithMesh> Mesh：%s的骨骼与蒙太奇%s的骨骼不兼容")
	, *Mesh->GetName(), *Anim->GetName());

		return false;
	}
	return true;

}
