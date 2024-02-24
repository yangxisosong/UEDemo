// Fill out your copyright notice in the Description page of Project Settings.


#include "UnderAttackDialUpTaskV2.h"

void UUnderAttackDialUpTaskV2::BeginFly(const FRotator& Direction)
{
	if (!ensureMsgf(FlyDuration > 0.0f, TEXT("飞行时长为0！！！")))
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(ActorInfo.MoveComp);
	if (ensureMsgf(MovementComponent, TEXT("没有MovementComp！！！")))
	{
		RootMotionFly = MakeShared<FRootMotionSource_JumpForce>();
		RootMotionFly->InstanceName = FName(GetName() + TEXT("Fly"));
		RootMotionFly->AccumulateMode = ERootMotionAccumulateMode::Override;
		RootMotionFly->Settings.SetFlag(ERootMotionSourceSettingsFlags::UseSensitiveLiftoffCheck);
		RootMotionFly->Priority = Priority;
		RootMotionFly->Duration = FlyDuration;
		RootMotionFly->Distance = FlyDistance;
		RootMotionFly->Height = FlyHeight;
		RootMotionFly->Rotation = Direction;
		RootMotionFly->bDisableTimeout = false;
		RootMotionFly->PathOffsetCurve = Fly_PathOffsetCurve;
		RootMotionFly->TimeMappingCurve = TimeMappingCurve;
		RootMotionFly->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
		RootMotionFly->FinishVelocityParams.SetVelocity = FVector::ZeroVector;
		RootMotionId = MovementComponent->ApplyRootMotionSource(RootMotionFly);
	}
}

void UUnderAttackDialUpTaskV2::BeginSlide(const FVector& Direction)
{
	if (SlideDuration <= 0.0f)
	{
		return;
	}
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(ActorInfo.MoveComp);
	if (ensureAlwaysMsgf(MovementComponent, TEXT("没有MoveComp！！！")))
	{
		EndFly();

		RootMotionSlide = MakeShared<FRootMotionSource_MoveToForce>();
		RootMotionSlide->InstanceName = FName(GetName() + TEXT("Slide"));
		RootMotionSlide->AccumulateMode = ERootMotionAccumulateMode::Override;
		// RootMotion->Settings.SetFlag(ERootMotionSourceSettingsFlags::UseSensitiveLiftoffCheck);
		RootMotionSlide->Priority = Priority;
		RootMotionSlide->TargetLocation = ActorInfo.AvatarActor->GetActorLocation() + Direction * SlideDistance;
		RootMotionSlide->StartLocation = ActorInfo.AvatarActor->GetActorLocation();
		RootMotionSlide->Duration = SlideDuration;
		RootMotionSlide->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
		RootMotionSlide->FinishVelocityParams.SetVelocity = FVector::ZeroVector;
		RootMotionId = MovementComponent->ApplyRootMotionSource(RootMotionSlide);
	}
}

void UUnderAttackDialUpTaskV2::EndFly()
{
	if (RootMotionFly)
	{
		UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(ActorInfo.MoveComp);
		if (ensureAlwaysMsgf(MovementComponent, TEXT("没有MoveComp！！！")))
		{
			MovementComponent->RemoveRootMotionSourceByID(RootMotionId);
			RootMotionId = static_cast<uint16>(ERootMotionSourceID::Invalid);

			RootMotionFly = nullptr;

			this->OnEndFly();
		}
	}
}

void UUnderAttackDialUpTaskV2::EndSlide()
{
	if (RootMotionSlide)
	{
		UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(ActorInfo.MoveComp);
		if (ensureAlwaysMsgf(MovementComponent, TEXT("没有MoveComp！！！")))
		{
			MovementComponent->RemoveRootMotionSourceByID(RootMotionId);
			RootMotionId = static_cast<uint16>(ERootMotionSourceID::Invalid);

			RootMotionSlide = nullptr;

			this->OnEndSlide();
		}
	}
}

void UUnderAttackDialUpTaskV2::Tick(float Dt)
{
	Super::Tick(Dt);
	if (RootMotionFly && RootMotionFly->Status.HasFlag(ERootMotionSourceStatusFlags::Finished))
	{
		EndFly();
	}
	else if (RootMotionSlide && RootMotionSlide->Status.HasFlag(ERootMotionSourceStatusFlags::Finished))
	{
		EndSlide();
	}
	OnTick(Dt);
}

void UUnderAttackDialUpTaskV2::EndTask(const bool Cancel)
{
	Super::EndTask(Cancel);
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(ActorInfo.MoveComp);
	if (ensureAlwaysMsgf(MovementComponent, TEXT("没有MoveComp！！！")))
	{
		if (RootMotionFly)
		{
			MovementComponent->RemoveRootMotionSourceByID(RootMotionId);
			RootMotionId = static_cast<uint16>(ERootMotionSourceID::Invalid);

			RootMotionFly = nullptr;
		}
		else if (RootMotionSlide)
		{
			MovementComponent->RemoveRootMotionSourceByID(RootMotionId);
			RootMotionId = static_cast<uint16>(ERootMotionSourceID::Invalid);

			RootMotionSlide = nullptr;
		}
	}
}
