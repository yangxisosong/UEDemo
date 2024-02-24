// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimationTestCharacter.h"

#include "DLAnimCommonLibrary.h"
#include "DLLinkedAnimInstance.h"
#include "DLMainAnimInstance.h"
#include "Components/DLAnimComponentBase.h"
#include "Components/DLAnimComponentHumanLocomotion.h"
#include "Components/DLCharacterMovementComponent.h"

// Sets default values
AAnimationTestCharacter::AAnimationTestCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UDLCharacterMovementComponent>(CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = 0;
	//TODO:后面这里改成用策略实现
	AnimComponent = CreateDefaultSubobject<UDLAnimComponentHumanLocomotion>("AnimComponent");
}

// void AAnimationTestCharacter::PostInitProperties()
// {
// 	Super::PostInitProperties();
// 	// LoadAnimConfig();
// }
//
// void AAnimationTestCharacter::PreInitializeComponents()
// {
// 	Super::PreInitializeComponents();
//
// }

void AAnimationTestCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	LoadAnimConfig();
}

// Called when the game starts or when spawned
void AAnimationTestCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->AddTickPrerequisiteActor(this);
	AnimComponent->GetAnimCharacterInfoRef().MovementState = EMovementState::Grounded;

	UDLCharacterMovementComponent* MyMovementComponent = Cast<UDLCharacterMovementComponent>(GetMovementComponent());
	MyMovementComponent->SetCurrentMovementSettings(GetTargetMovementSettings());

}

void AAnimationTestCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	AnimComponent->OnLanded(Hit);
}

// Called every frame
void AAnimationTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AnimComponent->OnTick(DeltaTime);
}

FRotator AAnimationTestCharacter::GetLastVelocityRotation_Debug_Implementation()
{
	return GetAnimComp<UDLAnimComponentHumanLocomotion>()->LastVelocityRotation;
}

FRotator AAnimationTestCharacter::GetLastMovementInputRotation_Debug_Implementation()
{
	return GetAnimComp<UDLAnimComponentHumanLocomotion>()->LastMovementInputRotation;
}

FRotator AAnimationTestCharacter::GetTargetRotation_Debug_Implementation()
{
	return GetAnimComp<UDLAnimComponentHumanLocomotion>()->TargetRotation;
}

FString AAnimationTestCharacter::GetCharacterMainState_Debug_Implementation()
{
	return UDLAnimCommonLibrary::GetEnumerationToString(GetAnimComp<UDLAnimComponentHumanLocomotion>()->GetAnimCharacterInfo().CharacterMainState.CharacterMainState) ;
}

FString AAnimationTestCharacter::GetOverlayState_Debug_Implementation()
{
	return UDLAnimCommonLibrary::GetEnumerationToString(GetAnimComp<UDLAnimComponentHumanLocomotion>()->GetAnimCharacterInfo().OverlayState.DLAnimOverlayState);
}

float AAnimationTestCharacter::GetCurveValue_Debug_Implementation(FName Curve)
{
	return GetMainAnimInstance()->GetCurveValue(Curve);
}

// void AAnimationTestCharacter::CharacterMove(const FInputActionValue& Value)
// {
// 	if (AnimComponent->GetAnimCharacterInfo().MovementState == EMovementState::Grounded
// 		|| AnimComponent->GetAnimCharacterInfo().MovementState == EMovementState::InAir)
// 	{
// 		// float Scale = UDLAnimCommonLibrary::FixDiagonalGamepadValues(Value[1], GetControlRotation().Yaw).Key;
// 		// FRotator DirRotation(0.0f, AnimComponent->GetAnimCharacterInfo().AimingRotation.Yaw, 0.0f);
// 		// AddMovementInput(UKismetMathLibrary::GetForwardVector(DirRotation), Scale);
// 		//
// 		// Scale = UDLAnimCommonLibrary::FixDiagonalGamepadValues(Value[0], GetControlRotation().Yaw).Value;
// 		// DirRotation = { 0.0f, AnimComponent->GetAnimCharacterInfo().AimingRotation.Yaw, 0.0f };
// 		// AddMovementInput(UKismetMathLibrary::GetRightVector(DirRotation), Scale);
//
// 		const FRotator Rotation(0.0f, GetControlRotation().Yaw, 0.0f);
// 		AddMovementInput(UKismetMathLibrary::GetForwardVector(Rotation), Value[1]);
// 		AddMovementInput(UKismetMathLibrary::GetRightVector(Rotation), Value[0]);
// 	}
// }

// void AAnimationTestCharacter::CameraMove(const FInputActionValue& Value)
// {
// 	Super::CameraMove(Value);
// }

void AAnimationTestCharacter::SetHasHorizontalInput(bool NewValue)
{
	GetAnimComponent()->GetAnimCharacterInfoRef().bHasHorizontalInput = NewValue;
}

void AAnimationTestCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	AnimComponent->OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}

void AAnimationTestCharacter::LoadAnimConfig()
{
	if (!AnimConfigDataTable)
	{
		ensureMsgf(false, TEXT("<UDLAnimInstanceBase::LoadAnimConfig> load anim config data table fail!!!"));
		return;
	}

	FAnimConfig* Data = AnimConfigDataTable->FindRow<FAnimConfig>(FName(AnimConfigName)
			, TEXT("<UDLAnimInstanceBase::LoadAnimConfig> load anim config data table"));
	if (!Data)
	{
		ensureMsgf(false, TEXT("<UDLAnimInstanceBase::LoadAnimConfig> Not find target row:%s "), *AnimConfigName);
		return;
	}
	AnimConfig = *Data;
}

UDLMainAnimInstance* AAnimationTestCharacter::GetMainAnimInstance() const
{
	return Cast<UDLMainAnimInstance>(GetMesh()->GetAnimInstance());
}

// UDLLinkedAnimInstance* AAnimationTestCharacter::GetCurrentLinkedInstance() const
// {
// 	switch (AnimComponent->GetAnimCharacterInfo().CharacterMainState)
// 	{
// 	case ECharacterMainState::Normal:
// 		return Cast<UDLLinkedAnimInstance>(GetMainAnimInstance()->GetLinkedAnimGraphInstanceByTag("Normal"));
// 	case ECharacterMainState::Locking:
// 		return Cast<UDLLinkedAnimInstance>(GetMainAnimInstance()->GetLinkedAnimGraphInstanceByTag("Locking"));
// 	case ECharacterMainState::Aiming:
// 		return Cast<UDLLinkedAnimInstance>(GetMainAnimInstance()->GetLinkedAnimGraphInstanceByTag("Aiming"));
// 	default:
// 		return nullptr;
// 	}
// }

const FAnimConfig& AAnimationTestCharacter::GetAnimConfig() const
{
	return AnimConfig;
}

const FMovementSettings& AAnimationTestCharacter::GetTargetMovementSettings() const
{
	switch (AnimComponent->GetAnimCharacterInfo().CharacterMainState)
	{
		// case ECharacterMainState::Aiming:
		// 	return AnimConfig.MovementStateSettings.Aiming;
	case ECharacterMainState::Locking:
		return AnimConfig.MovementStateSettings.Locking;
	case ECharacterMainState::Normal:
		return AnimConfig.MovementStateSettings.Normal;
	default:
		ensureMsgf(false, TEXT("为什么有未知的枚举值？？？？？？？？？"));
		return AnimConfig.MovementStateSettings.Normal;
	}
}

void AAnimationTestCharacter::ChangeMainState(ECharacterMainState State)
{
	AnimComponent->GetAnimCharacterInfoRef().CharacterMainState = State;
}

void AAnimationTestCharacter::ChangeOverlayState(EDLAnimOverlayState State)
{
	AnimComponent->GetAnimCharacterInfoRef().OverlayState = State;
}

void AAnimationTestCharacter::ChangeMovementAction(const EMovementAction NewAction)
{
	AnimComponent->GetAnimCharacterInfoRef().MovementAction = NewAction;
}

