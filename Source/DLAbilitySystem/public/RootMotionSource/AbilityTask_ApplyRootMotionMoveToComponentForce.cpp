// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTask_ApplyRootMotionMoveToComponentForce.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/RootMotionSource.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Net/UnrealNetwork.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "AbilitySystemLog.h"


UAbilityTask_ApplyRootMotionMoveToComponentForce* UAbilityTask_ApplyRootMotionMoveToComponentForce::
ApplyRootMotionMoveToComponentForce(UGameplayAbility* OwningAbility, FName TaskInstanceName,
	USceneComponent* TargetComponent, FVector TargetLocationOffset,
	ERootMotionMoveToActorTargetOffsetType OffsetAlignment, float Duration, UCurveFloat* TargetLerpSpeedHorizontal,
	UCurveFloat* TargetLerpSpeedVertical, bool bSetNewMovementMode, EMovementMode MovementMode,
	bool bRestrictSpeedToExpected, UCurveVector* PathOffsetCurve, UCurveFloat* TimeMappingCurve,
	ERootMotionFinishVelocityMode VelocityOnFinishMode, FVector SetVelocityOnFinish, float ClampVelocityOnFinish,
	bool bDisableDestinationReachedInterrupt)
{
	UAbilityTask_ApplyRootMotionMoveToComponentForce* MyTask = NewAbilityTask<UAbilityTask_ApplyRootMotionMoveToComponentForce>(OwningAbility, TaskInstanceName);

	//UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Duration(Duration);

	MyTask->ForceName = TaskInstanceName;
	MyTask->TargetComponent = TargetComponent;
	MyTask->TargetLocationOffset = TargetLocationOffset;
	MyTask->OffsetAlignment = OffsetAlignment;
	MyTask->Duration = FMath::Max(Duration, KINDA_SMALL_NUMBER); // Avoid negative or divide-by-zero cases
	MyTask->bDisableDestinationReachedInterrupt = bDisableDestinationReachedInterrupt;
	MyTask->TargetLerpSpeedHorizontalCurve = TargetLerpSpeedHorizontal;
	MyTask->TargetLerpSpeedVerticalCurve = TargetLerpSpeedVertical;
	MyTask->bSetNewMovementMode = bSetNewMovementMode;
	MyTask->NewMovementMode = MovementMode;
	MyTask->bRestrictSpeedToExpected = bRestrictSpeedToExpected;
	MyTask->PathOffsetCurve = PathOffsetCurve;
	MyTask->TimeMappingCurve = TimeMappingCurve;
	MyTask->FinishVelocityMode = VelocityOnFinishMode;
	MyTask->FinishSetVelocity = SetVelocityOnFinish;
	MyTask->FinishClampVelocity = ClampVelocityOnFinish;
	if (MyTask->GetAvatarActor() != nullptr)
	{
		MyTask->StartLocation = MyTask->GetAvatarActor()->GetActorLocation();
	}
	else
	{
		checkf(false, TEXT("UAbilityTask_ApplyRootMotionMoveToActorForce called without valid avatar actor to get start location from."));
		MyTask->StartLocation = TargetComponent ? TargetComponent->GetComponentLocation() : FVector(0.f);
	}
	MyTask->SharedInitAndApply();

	return MyTask;
}

void UAbilityTask_ApplyRootMotionMoveToComponentForce::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAbilityTask_ApplyRootMotionMoveToComponentForce, TargetComponent);

}

FVector UAbilityTask_ApplyRootMotionMoveToComponentForce::CalculateTargetOffsetv2() const
{
	check(TargetComponent != nullptr);

	const FVector TargetActorLocation = TargetComponent->GetComponentLocation();
	FVector CalculatedTargetLocation = TargetActorLocation;
	
	if (OffsetAlignment == ERootMotionMoveToActorTargetOffsetType::AlignFromTargetToSource)
	{
		if (MovementComponent)
		{
			FVector ToSource = MovementComponent->GetActorLocation() - TargetActorLocation;
			ToSource.Z = 0.f;
			CalculatedTargetLocation += ToSource.ToOrientationQuat().RotateVector(TargetLocationOffset);
		}

	}
	else if (OffsetAlignment == ERootMotionMoveToActorTargetOffsetType::AlignToTargetForward)
	{
		CalculatedTargetLocation += TargetComponent->GetComponentQuat().RotateVector(TargetLocationOffset);
	}
	else if (OffsetAlignment == ERootMotionMoveToActorTargetOffsetType::AlignToWorldSpace)
	{
		CalculatedTargetLocation += TargetLocationOffset;
	}
	
	return CalculatedTargetLocation;
}

bool UAbilityTask_ApplyRootMotionMoveToComponentForce::UpdateTargetLocationv2(float DeltaTime)
{
	if (TargetComponent && GetWorld())
	{
		const FVector PreviousTargetLocation = TargetLocation;
		FVector ExactTargetLocation = CalculateTargetOffsetv2();

		const float CurrentTime = GetWorld()->GetTimeSeconds();
		const float CompletionPercent = (CurrentTime - StartTime) / Duration;

		const float TargetLerpSpeedHorizontal = TargetLerpSpeedHorizontalCurve ? TargetLerpSpeedHorizontalCurve->GetFloatValue(CompletionPercent) : 1000.f;
		const float TargetLerpSpeedVertical = TargetLerpSpeedVerticalCurve ? TargetLerpSpeedVerticalCurve->GetFloatValue(CompletionPercent) : 500.f;

		const float MaxHorizontalChange = FMath::Max(0.f, TargetLerpSpeedHorizontal * DeltaTime);
		const float MaxVerticalChange = FMath::Max(0.f, TargetLerpSpeedVertical * DeltaTime);

		FVector ToExactLocation = ExactTargetLocation - PreviousTargetLocation;
		FVector TargetLocationDelta = ToExactLocation;

		// Cap vertical lerp
		if (FMath::Abs(ToExactLocation.Z) > MaxVerticalChange)
		{
			if (ToExactLocation.Z >= 0.f)
			{
				TargetLocationDelta.Z = MaxVerticalChange;
			}
			else
			{
				TargetLocationDelta.Z = -MaxVerticalChange;
			}
		}

		// Cap horizontal lerp
		if (FMath::Abs(ToExactLocation.SizeSquared2D()) > MaxHorizontalChange*MaxHorizontalChange)
		{
			FVector ToExactLocationHorizontal(ToExactLocation.X, ToExactLocation.Y, 0.f);
			ToExactLocationHorizontal.Normalize();
			ToExactLocationHorizontal *= MaxHorizontalChange;

			TargetLocationDelta.X = ToExactLocationHorizontal.X;
			TargetLocationDelta.Y = ToExactLocationHorizontal.Y;
		}

		TargetLocation += TargetLocationDelta;

		return true;
	}

	return false;
}

void UAbilityTask_ApplyRootMotionMoveToComponentForce::TickTask(float DeltaTime)
{
	if (bIsFinished)
	{
		return;
	}

	Super::TickTask(DeltaTime);

	AActor* MyActor = GetAvatarActor();
	if (MyActor)
	{
		const bool bTimedOut = HasTimedOut();

		// Update target location
		{
			const FVector PreviousTargetLocation = TargetLocation;
			if (UpdateTargetLocationv2(DeltaTime))
			{
				SetRootMotionTargetLocation(TargetLocation);
			}
			else
			{
				// TargetLocation not updated - TargetActor not around anymore, continue on to last set TargetLocation
			}
		}
		

		const float ReachedDestinationDistanceSqr = 50.f * 50.f;
		const bool bReachedDestination = FVector::DistSquared(TargetLocation, MyActor->GetActorLocation()) < ReachedDestinationDistanceSqr;

		if (bTimedOut || (bReachedDestination && !bDisableDestinationReachedInterrupt))
		{
			// Task has finished
			bIsFinished = true;
			if (!bIsSimulating)
			{
				MyActor->ForceNetUpdate();
				if (ShouldBroadcastAbilityTaskDelegates())
				{
					OnFinished.Broadcast(bReachedDestination, bTimedOut, TargetLocation);
				}
				EndTask();
			}
		}
	}
	else
	{
		bIsFinished = true;
		EndTask();
	}
}

void UAbilityTask_ApplyRootMotionMoveToComponentForce::SharedInitAndApply()
{
	if (AbilitySystemComponent->AbilityActorInfo->MovementComponent.IsValid())
	{
		MovementComponent = Cast<UCharacterMovementComponent>(AbilitySystemComponent->AbilityActorInfo->MovementComponent.Get());
		StartTime = GetWorld()->GetTimeSeconds();
		EndTime = StartTime + Duration;

		if (MovementComponent)
		{
			if (bSetNewMovementMode)
			{
				PreviousMovementMode = MovementComponent->MovementMode;
				MovementComponent->SetMovementMode(NewMovementMode);
			}

			// Set initial target location
			if (TargetComponent)
			{
				TargetLocation = CalculateTargetOffsetv2();
			}

			ForceName = ForceName.IsNone() ? FName("AbilityTaskApplyRootMotionMoveToActorForce") : ForceName;
			TSharedPtr<FRootMotionSource_MoveToDynamicForce> MoveToActorForce = MakeShared<FRootMotionSource_MoveToDynamicForce>();
			MoveToActorForce->InstanceName = ForceName;
			MoveToActorForce->AccumulateMode = ERootMotionAccumulateMode::Override;
			MoveToActorForce->Settings.SetFlag(ERootMotionSourceSettingsFlags::UseSensitiveLiftoffCheck);
			MoveToActorForce->Priority = 900;
			MoveToActorForce->InitialTargetLocation = TargetLocation;
			MoveToActorForce->TargetLocation = TargetLocation;
			MoveToActorForce->StartLocation = StartLocation;
			MoveToActorForce->Duration = FMath::Max(Duration, KINDA_SMALL_NUMBER);
			MoveToActorForce->bRestrictSpeedToExpected = bRestrictSpeedToExpected;
			MoveToActorForce->PathOffsetCurve = PathOffsetCurve;
			MoveToActorForce->TimeMappingCurve = TimeMappingCurve;
			MoveToActorForce->FinishVelocityParams.Mode = FinishVelocityMode;
			MoveToActorForce->FinishVelocityParams.SetVelocity = FinishSetVelocity;
			MoveToActorForce->FinishVelocityParams.ClampVelocity = FinishClampVelocity;
			RootMotionSourceID = MovementComponent->ApplyRootMotionSource(MoveToActorForce);

			if (Ability)
			{
				//Ability->SetMovementSyncPoint(ForceName);
			}
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_ApplyRootMotionMoveToActorForce called in Ability %s with null MovementComponent; Task Instance Name %s."), 
			Ability ? *Ability->GetName() : TEXT("NULL"), 
			*InstanceName.ToString());
	}
}
