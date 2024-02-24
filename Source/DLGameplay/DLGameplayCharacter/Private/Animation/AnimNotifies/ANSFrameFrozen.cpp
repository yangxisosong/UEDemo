// Fill out your copyright notice in the Description page of Project Settings.


#include "ANSFrameFrozen.h"

#include "Animation/AnimationTestCharacter.h"
#include "Animation/Components/DLAnimComponentHumanLocomotion.h"

#if WITH_EDITOR
void UANSFrameFrozen::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	CurAnimSeq = const_cast<UAnimSequence*>(Cast<UAnimSequence>(ContainingAnimNotifyEvent.GetLinkedSequence()));
}
#endif

void UANSFrameFrozen::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	EndTimeOffset = StartTimeOffset + TotalDuration;
	TimeOffset = 0.0f;
	UAnimMontage* TargetMontage = Cast<UAnimMontage>(Animation);
	if (TargetMontage && MeshComp->GetAnimInstance())
	{
		DefaultPlayRate = MeshComp->GetAnimInstance()->Montage_GetPlayRate(TargetMontage);
	}
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UANSFrameFrozen::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (FullBodySlomoCurve && MeshComp->GetOwner())
	{
		TimeOffset += FrameDeltaTime;
		const float CurveValue = FullBodySlomoCurve->GetFloatValue(TimeOffset / (EndTimeOffset - StartTimeOffset));
		const float Slomo = FMath::Clamp(CurveValue, 0.1f, 1.0f);
		UAnimMontage* TargetMontage = Cast<UAnimMontage>(Animation);
		if (TargetMontage && MeshComp->GetAnimInstance())
		{
			MeshComp->GetAnimInstance()->Montage_SetPlayRate(TargetMontage, Slomo);
		}
	}

	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UANSFrameFrozen::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	TimeOffset = 0.0f;
	UAnimMontage* TargetMontage = Cast<UAnimMontage>(Animation);
	if (TargetMontage && MeshComp->GetAnimInstance())
	{
		MeshComp->GetAnimInstance()->Montage_SetPlayRate(TargetMontage, DefaultPlayRate);
	}
	Super::NotifyEnd(MeshComp, Animation);
}

FString UANSFrameFrozen::GetNotifyName_Implementation() const
{
	return TEXT("ANS_FrameFrozen");
}