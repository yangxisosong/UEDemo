#include "Animation/DLAnimCommonLibrary.h"
#include "Components/CapsuleComponent.h"

TPair<float, float> UDLAnimCommonLibrary::FixDiagonalGamepadValues(float X, float Y)
{
	float ResultY = X * FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 0.6f),
														  FVector2D(1.0f, 1.2f), FMath::Abs(Y));
	ResultY = FMath::Clamp(ResultY, -1.0f, 1.0f);
	float ResultX = Y * FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 0.6f),
														  FVector2D(1.0f, 1.2f), FMath::Abs(X));
	ResultX = FMath::Clamp(ResultX, -1.0f, 1.0f);
	return TPair<float, float>(ResultY, ResultX);
}

FVector UDLAnimCommonLibrary::GetCapsuleBaseLocation(float ZOffset, UCapsuleComponent* Capsule)
{
	return Capsule->GetComponentLocation() -
		Capsule->GetUpVector() * (Capsule->GetScaledCapsuleHalfHeight() + ZOffset);
}

FVector UDLAnimCommonLibrary::GetCapsuleLocationFromBase(FVector BaseLocation, float ZOffset,
	UCapsuleComponent* Capsule)
{
	BaseLocation.Z += Capsule->GetScaledCapsuleHalfHeight() + ZOffset;
	return BaseLocation;
}

bool UDLAnimCommonLibrary::CapsuleHasRoomCheck(UCapsuleComponent* Capsule, FVector TargetLocation, float HeightOffset,
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

bool UDLAnimCommonLibrary::AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool IncreaseBuffer)
{
	if (IncreaseBuffer)
	{
		return Angle >= MinAngle - Buffer && Angle <= MaxAngle + Buffer;
	}
	return Angle >= MinAngle + Buffer && Angle <= MaxAngle - Buffer;
}

EMovementDirection UDLAnimCommonLibrary::CalculateQuadrant(EMovementDirection Current, float FRThreshold,
	float FLThreshold, float BRThreshold, float BLThreshold, float Buffer, float Angle)
{
	if (AngleInRange(Angle, FLThreshold, FRThreshold, Buffer,
		Current != EMovementDirection::Forward || Current != EMovementDirection::Backward))
	{
		return EMovementDirection::Forward;
	}

	if (AngleInRange(Angle, FRThreshold, BRThreshold, Buffer,
		Current != EMovementDirection::Right || Current != EMovementDirection::Left))
	{
		return EMovementDirection::Right;
	}

	if (AngleInRange(Angle, BLThreshold, FLThreshold, Buffer,
		Current != EMovementDirection::Right || Current != EMovementDirection::Left))
	{
		return EMovementDirection::Left;
	}

	return EMovementDirection::Backward;
}

FRotator UDLAnimCommonLibrary::FixRootRotationBias(const FRotator& RootRotation, const FRotator& TargetRotation)
{
	const FQuat RootQuat(RootRotation);
	const FQuat TargetQuat(TargetRotation);
	return (RootQuat.Inverse() * TargetQuat).Rotator();
}

EMovementDirection UDLAnimCommonLibrary::GetMovementDirection(const FRotator& ActorRotation, const FVector& Velocity)
{
	const FVector LocRelativeVelocityDir = ActorRotation.UnrotateVector(Velocity.GetSafeNormal(0.1f));

	const float Sum = FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y) +
		FMath::Abs(LocRelativeVelocityDir.Z);
	const FVector RelativeDir = LocRelativeVelocityDir / Sum;
	FVelocityBlend Result;
	Result.Forward = FMath::Clamp(RelativeDir.X, 0.0f, 1.0f);
	Result.Backward = FMath::Abs(FMath::Clamp(RelativeDir.X, -1.0f, 0.0f));
	Result.Left = FMath::Abs(FMath::Clamp(RelativeDir.Y, -1.0f, 0.0f));
	Result.Right = FMath::Clamp(RelativeDir.Y, 0.0f, 1.0f);

	if (Result.Forward > 0.5f)
	{
		return EMovementDirection::Forward;
	}
	else if (Result.Backward > 0.5f)
	{
		return EMovementDirection::Backward;
	}
	else if (Result.Left > 0.5f)
	{
		return EMovementDirection::Left;
	}
	else if (Result.Right > 0.5f)
	{
		return EMovementDirection::Right;
	}
	return EMovementDirection::Forward;
}

void UDLAnimCommonLibrary::GetBonePosesForTime(const UAnimSequence* AnimationSequence, TArray<FName> BoneNames,
	float Time, bool bExtractRootMotion, TArray<FTransform>& Poses, const USkeletalMesh* PreviewMesh)
{
	Poses.Empty(BoneNames.Num());
	if (AnimationSequence && AnimationSequence->GetSkeleton())
	{
		Poses.AddDefaulted(BoneNames.Num());

		// Need this for FCompactPose
		FMemMark Mark(FMemStack::Get());

		const FReferenceSkeleton& RefSkeleton = (PreviewMesh) ? PreviewMesh->GetRefSkeleton() : AnimationSequence->GetSkeleton()->GetReferenceSkeleton();

		if (IsValidTimeInternal(AnimationSequence, Time))
		{
			if (BoneNames.Num())
			{
				for (int32 BoneNameIndex = 0; BoneNameIndex < BoneNames.Num(); ++BoneNameIndex)
				{
					const FName& BoneName = BoneNames[BoneNameIndex];

					FTransform& Transform = Poses[BoneNameIndex];
					if (IsValidRawAnimationTrackName(AnimationSequence, BoneName, RefSkeleton))
					{
						//AnimationSequence->ExtractBoneTransform(GetRawAnimationTrackByName(AnimationSequence, BoneName, PreviewMesh), Transform, Time);
					}
					else
					{

						// otherwise, get ref pose if exists
						const int32 BoneIndex = RefSkeleton.FindBoneIndex(BoneName);
						if (BoneIndex != INDEX_NONE)
						{
							Transform = RefSkeleton.GetRefBonePose()[BoneIndex];
						}
						else
						{
							DL_ANIM_LOG(Warning, TEXT("Invalid bone name %s for Animation Sequence %s supplied for GetBonePosesForTime"), *BoneName.ToString(), *AnimationSequence->GetName());
							Transform = FTransform::Identity;
						}
					}
				}
			}
			else
			{
				DL_ANIM_LOG(Error, TEXT("Invalid or no bone names specified to retrieve poses given Animation Sequence %s in GetBonePosesForTime"), *AnimationSequence->GetName());
			}
		}
		else
		{
			DL_ANIM_LOG(Warning, TEXT("Invalid time value %f for Animation Sequence %s supplied for GetBonePosesForTime"), Time, *AnimationSequence->GetName());
		}
	}
	else
	{
		DL_ANIM_LOG_WITHOUT_VALUE(Warning, TEXT("Invalid Animation Sequence supplied for GetBonePosesForTime"));
	}
}

void UDLAnimCommonLibrary::GetBonePoseForTime(const UAnimSequence* AnimationSequence, FName BoneName, float Time,
	bool bExtractRootMotion, FTransform& Pose, const USkeletalMesh* PreviewMesh)
{
	TArray<FTransform> Poses;
	GetBonePosesForTime(AnimationSequence, { BoneName }, Time, bExtractRootMotion, Poses, PreviewMesh);
	Pose = Poses[0];
}

bool UDLAnimCommonLibrary::IsValidTimeInternal(const UAnimSequence* AnimationSequence, const float Time)
{
	return FMath::IsWithinInclusive(Time, 0.0f, AnimationSequence->GetPlayLength());
}

bool UDLAnimCommonLibrary::IsValidRawAnimationTrackName(const UAnimSequence* AnimationSequence, const FName TrackName, const FReferenceSkeleton& RefSkeleton)
{
	if (TrackName != NAME_None)
	{
		return RefSkeleton.FindBoneIndex(TrackName) != INDEX_NONE;
	}
	else
	{

		DL_ANIM_LOG_WITHOUT_VALUE(Warning, TEXT("Invalid Animation Sequence supplied for IsValidRawAnimationTrackName"));
	}

	return false;
}

//const FRawAnimSequenceTrack& UDLAnimCommonLibrary::GetRawAnimationTrackByName(const UAnimSequence* AnimationSequence,
//	const FName TrackName, const USkeletalMesh* PreviewMesh /*nullptr*/)
//{
//
//	ensureMsgf(AnimationSequence, TEXT("Invalid Animation Sequence supplied for GetRawAnimationTrackByName"));
//
//	const int32 TrackIndex = GetBoneIndex(AnimationSequence, TrackName, PreviewMesh);
//	ensureMsgf(TrackIndex != INDEX_NONE, TEXT("Raw Animation Track %s does not exist in Animation Sequence %s"), *TrackName.ToString(), *AnimationSequence->GetName());
//	return AnimationSequence->GetRawAnimationTrack(TrackIndex);
//}

int32 UDLAnimCommonLibrary::GetBoneIndex(const UAnimSequence* AnimationSequence, FName BoneName, const USkeletalMesh* PreviewMesh /*nullptr*/)
{
	if (AnimationSequence)
	{
		int32 BoneIndex = INDEX_NONE;
		const FReferenceSkeleton& RefSkeleton = (PreviewMesh) ? PreviewMesh->GetRefSkeleton() : AnimationSequence->GetSkeleton()->GetReferenceSkeleton();

		if (BoneName != NAME_None)
		{
			BoneIndex = RefSkeleton.FindBoneIndex(BoneName);
		}

		return BoneIndex;
	}
	return INDEX_NONE;
}

FVector UDLAnimCommonLibrary::ClampedVectorToMaxSize(const FVector& Target, float MaxSize)
{
	return Target.GetClampedToMaxSize(MaxSize);
}
