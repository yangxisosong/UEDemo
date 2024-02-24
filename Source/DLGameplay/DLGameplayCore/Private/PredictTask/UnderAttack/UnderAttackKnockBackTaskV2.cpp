// Fill out your copyright notice in the Description page of Project Settings.


#include "UnderAttackKnockBackTaskV2.h"

void UUnderAttackKnockBackTaskV2::BeginMove(const FVector TargetLocation, const float Duration, const int32 Priority)
{
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(ActorInfo.MoveComp);
	if (ensureAlwaysMsgf(MovementComponent, TEXT("没有MoveComp！！！")))
	{
		RootMotion = MakeShared<FRootMotionSource_MoveToForce>();
		RootMotion->InstanceName = GetFName();
		RootMotion->AccumulateMode = ERootMotionAccumulateMode::Override;
		// RootMotion->Settings.SetFlag(ERootMotionSourceSettingsFlags::UseSensitiveLiftoffCheck);
		RootMotion->Priority = Priority;
		RootMotion->TargetLocation = TargetLocation;
		RootMotion->StartLocation = ActorInfo.AvatarActor->GetActorLocation();
		RootMotion->Duration = Duration;
		RootMotion->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
		RootMotion->FinishVelocityParams.SetVelocity = FVector::ZeroVector;

		RootMotionId = MovementComponent->ApplyRootMotionSource(RootMotion);
	}
}

void UUnderAttackKnockBackTaskV2::EndTask(const bool Cancel)
{
	Super::EndTask(Cancel);
	if (RootMotion.IsValid())
	{
		UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(ActorInfo.MoveComp);
		if (ensureAlwaysMsgf(MovementComponent, TEXT("没有MoveComp！！！")))
		{
			MovementComponent->RemoveRootMotionSourceByID(RootMotionId);
			RootMotionId = static_cast<uint16>(ERootMotionSourceID::Invalid);

			RootMotion = nullptr;
		}
	}
}

void UUnderAttackKnockBackTaskV2::Tick(float Dt)
{
	Super::Tick(Dt);
	if (RootMotion.IsValid())
	{
		if (RootMotion->Status.HasFlag(ERootMotionSourceStatusFlags::Finished))
		{
			UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(ActorInfo.MoveComp);
			if (ensureAlwaysMsgf(MovementComponent, TEXT("没有MoveComp！！！")))
			{
				MovementComponent->RemoveRootMotionSourceByID(RootMotionId);
				RootMotionId = static_cast<uint16>(ERootMotionSourceID::Invalid);

				RootMotion = nullptr;

				this->EndMove();
			}
		}
	}
}
