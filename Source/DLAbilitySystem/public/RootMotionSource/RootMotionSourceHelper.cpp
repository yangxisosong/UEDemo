// Fill out your copyright notice in the Description page of Project Settings.


#include "RootMotionSourceHelper.h"

#include "AbilityTask_ApplyRootMotionMoveToComponentForce.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionJumpForce.h"
#include "GameFramework/RootMotionSource.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToActorForce.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "Interface/ICharacterStateAccessor.h"
#include "Interface/IDLLockableUnit.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


UAbilityTask_ApplyRootMotion_Base* URootMotionSourceHelper::ApplyRootMotionBP(UGameplayAbility* OwningAbility,
                                                                              FName TaskInstanceName,
                                                                              UDataAsset_RootMotion*
                                                                              RootMotionDataAsset)
{
	check(OwningAbility)
	if (RootMotionDataAsset->Type == ERootMotionType::ToActor)
	{
		AActor* AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();
		bool bIsLock = ICharacterStateAccessor::Execute_K2_IsLockTargetUnit(AvatarActor);
		UDLLockPointComponent* TargetComponent = ICharacterStateAccessor::Execute_K2_GetLockPoint(AvatarActor);
		if (bIsLock)
		{
			FVector TargetToAvatar = AvatarActor->GetActorLocation() -TargetComponent->GetComponentLocation();
			bool ToActor = true;
			FVector TargetLocationOffset = RootMotionDataAsset->TargetLocationOffset;
			if (RootMotionDataAsset->IgnoreZ)
			{
				if (RootMotionDataAsset->LockMaxDistance < TargetToAvatar.Size2D())
				{
					ToActor = false;
				}
				TargetLocationOffset.Z += TargetToAvatar.Z;
			}
			else
			{
				if (RootMotionDataAsset->LockMaxDistance < TargetToAvatar.Size())
				{
					ToActor = false;
				}
			}
			//if (ToActor)
			if (false)
			{
				return  UAbilityTask_ApplyRootMotionMoveToComponentForce::ApplyRootMotionMoveToComponentForce(
					OwningAbility,
					TaskInstanceName,
					TargetComponent,
					TargetLocationOffset,
					RootMotionDataAsset->OffsetAlignment,
					FMath::Max(RootMotionDataAsset->Duration, KINDA_SMALL_NUMBER),
					nullptr,
					nullptr,
					false,
					EMovementMode::MOVE_None,
					false,
					RootMotionDataAsset->PathOffsetCurve,
					RootMotionDataAsset->TimeMappingCurve,
					ERootMotionFinishVelocityMode::ClampVelocity,
					FVector::ZeroVector,
					0,
					false
				);
			}
			else
			{
				FVector RotatedDir = TargetComponent->GetComponentLocation() - AvatarActor->GetActorLocation();
				RotatedDir = RotatedDir.GetSafeNormal2D();
				FVector TargetLocation = AvatarActor->GetActorLocation() + RotatedDir * RootMotionDataAsset->LockMaxDistance;
				
				FHitResult HitRet;
				TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType = {
					EObjectTypeQuery::ObjectTypeQuery1,
					EObjectTypeQuery::ObjectTypeQuery3,
				};
				bool IsHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
					AvatarActor,
					AvatarActor->GetActorLocation(),
					TargetLocation,
					20,
					ObjectType,
					false,
					{AvatarActor},
					false ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
					HitRet,
					true
				);
				if (IsHit)
				{
					TargetLocation = HitRet.ImpactPoint;
				}
				
				return UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
					OwningAbility,
					TaskInstanceName,
					TargetLocation,
					RootMotionDataAsset->Duration,
					false,
					EMovementMode::MOVE_None,
					false,
					RootMotionDataAsset->PathOffsetCurve,
					ERootMotionFinishVelocityMode::ClampVelocity,
					FVector::ZeroVector,
					0
				);
			}
		}
		else
		{
			FVector Dir = AvatarActor->GetActorForwardVector();
			FVector RotatedDir = RootMotionDataAsset->Rotation.RotateVector(Dir);
			FVector TargetLocation = AvatarActor->GetActorLocation() + RotatedDir * RootMotionDataAsset->Distance;

			FHitResult HitRet;
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType = {
				EObjectTypeQuery::ObjectTypeQuery1,
				EObjectTypeQuery::ObjectTypeQuery3,
			};
			bool IsHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
				AvatarActor,
				AvatarActor->GetActorLocation(),
				TargetLocation,
				20,
				ObjectType,
				false,
				{AvatarActor},
				false ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
				HitRet,
				true
			);
			if (IsHit)
			{
				TargetLocation = HitRet.ImpactPoint;
			}
			
			return UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
				OwningAbility,
				TaskInstanceName,
				TargetLocation,
				RootMotionDataAsset->Duration,
				false,
				EMovementMode::MOVE_None,
				false,
				RootMotionDataAsset->PathOffsetCurve,
				ERootMotionFinishVelocityMode::ClampVelocity,
				FVector::ZeroVector,
				0
			);
		}
	}
	else if (RootMotionDataAsset->Type == ERootMotionType::ToLocation)
	{
		AActor* AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();
		FVector CalculatedTargetLocation = FVector::ZeroVector;
		bool bIsLock = ICharacterStateAccessor::Execute_K2_IsLockTargetUnit(AvatarActor);
		if (bIsLock)
		{
			UDLLockPointComponent* TargetComponent = ICharacterStateAccessor::Execute_K2_GetLockPoint(AvatarActor);
			
			const FVector AvatarActorLocation = AvatarActor->GetActorLocation();
			FVector TargetActorLocation = TargetComponent->GetComponentLocation();
			if (RootMotionDataAsset->IgnoreZ)
			{
				TargetActorLocation.Z = AvatarActorLocation.Z;
			}
			CalculatedTargetLocation = TargetActorLocation;
	
			if (RootMotionDataAsset->OffsetAlignment == ERootMotionMoveToActorTargetOffsetType::AlignFromTargetToSource)
			{
				FVector ToSource = AvatarActorLocation - TargetActorLocation;
				ToSource.Z = 0.f;
				CalculatedTargetLocation += ToSource.ToOrientationQuat().RotateVector(RootMotionDataAsset->TargetLocationOffset);

				if (RootMotionDataAsset->LockMaxDistance < (AvatarActorLocation - CalculatedTargetLocation).Size2D())
				{
					FVector dir = (CalculatedTargetLocation - AvatarActorLocation).GetSafeNormal();
					CalculatedTargetLocation = AvatarActorLocation + dir * RootMotionDataAsset->LockMaxDistance;
				}
			
			}
			else if (RootMotionDataAsset->OffsetAlignment == ERootMotionMoveToActorTargetOffsetType::AlignToTargetForward)
			{
				CalculatedTargetLocation += TargetComponent->GetComponentQuat().RotateVector(RootMotionDataAsset->TargetLocationOffset);
			}
			else if (RootMotionDataAsset->OffsetAlignment == ERootMotionMoveToActorTargetOffsetType::AlignToWorldSpace)
			{
				CalculatedTargetLocation = AvatarActor->GetActorLocation() + AvatarActor->GetActorRotation().RotateVector(RootMotionDataAsset->TargetLocationOffset);
			}
			
		}
		else
		{
			FVector Dir = AvatarActor->GetActorForwardVector();
			FVector RotatedDir = RootMotionDataAsset->Rotation.RotateVector(Dir);
			CalculatedTargetLocation = AvatarActor->GetActorLocation() + RotatedDir * RootMotionDataAsset->Distance;
		}

		FHitResult HitRet;
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType = {
			EObjectTypeQuery::ObjectTypeQuery1,
			EObjectTypeQuery::ObjectTypeQuery3,
		};
		bool IsHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
			AvatarActor,
			AvatarActor->GetActorLocation(),
			CalculatedTargetLocation,
			20,
			ObjectType,
			false,
			{AvatarActor},
			false ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			HitRet,
			true
		);
		if (IsHit)
		{
			CalculatedTargetLocation = HitRet.ImpactPoint;
		}
		
		return UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
				OwningAbility,
				TaskInstanceName,
				CalculatedTargetLocation,
				RootMotionDataAsset->Duration,
				false,
				EMovementMode::MOVE_None,
				false,
				RootMotionDataAsset->PathOffsetCurve,
				ERootMotionFinishVelocityMode::ClampVelocity,
				FVector::ZeroVector,
				0
			);
		
	}
	else if (RootMotionDataAsset->Type == ERootMotionType::Jump)
	{
		AActor* AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();
		FRotator Rot = UKismetMathLibrary::ComposeRotators(AvatarActor->GetActorRotation(),RootMotionDataAsset->Rotation);
		return  UAbilityTask_ApplyRootMotionJumpForce::ApplyRootMotionJumpForce(
		OwningAbility,
		TaskInstanceName,
		Rot,
		RootMotionDataAsset->Distance,
		RootMotionDataAsset->Height,
		RootMotionDataAsset->Duration,
		RootMotionDataAsset->landedTriggerTime,
		true,
		ERootMotionFinishVelocityMode::ClampVelocity,
		FVector::ZeroVector,
		0,
		RootMotionDataAsset->PathOffsetCurve,
		RootMotionDataAsset->TimeMappingCurve
		);
	}
	else
	{
		return nullptr;
	}
}
