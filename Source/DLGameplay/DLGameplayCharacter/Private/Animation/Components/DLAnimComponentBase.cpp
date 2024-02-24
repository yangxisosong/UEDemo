// Fill out your copyright notice in the Description page of Project Settings.


#include "DLAnimComponentBase.h"

#include "DLCharacterMovementComponent.h"
#include "Animation/AnimationTestCharacter.h"
#include "Animation/DLAnimInstanceBase.h"
#include "Animation/DLLinkedAnimInstance.h"
#include "Animation/DLMainAnimInstance.h"
#include "GameFramework/Character.h"


// Sets default values for this component's properties
UDLAnimComponentBase::UDLAnimComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDLAnimComponentBase::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

UDLMainAnimInstance* UDLAnimComponentBase::GetMainAnimIns() const
{
	return Cast<UDLMainAnimInstance>(GetCharacterOwner()->GetMesh()->GetAnimInstance());
}

UDLCharacterMovementComponent* UDLAnimComponentBase::GetMovementComponent()
{
	const auto Comp = GetCharacterOwner()->GetMovementComponent();
	return Cast<UDLCharacterMovementComponent>(Comp);
}

// UDLLinkedAnimInstance* UDLAnimComponentBase::GetCurrentLinkedInstance() const
// {
// 	switch (GetAnimCharacterInfo().CharacterMainState)
// 	{
// 	case ECharacterMainState::Normal:
// 		return Cast<UDLLinkedAnimInstance>(GetMainAnimIns()->GetLinkedAnimGraphInstanceByTag("Normal"));
// 	case ECharacterMainState::Locking:
// 		return Cast<UDLLinkedAnimInstance>(GetMainAnimIns()->GetLinkedAnimGraphInstanceByTag("Locking"));
// 	case ECharacterMainState::Aiming:
// 		return Cast<UDLLinkedAnimInstance>(GetMainAnimIns()->GetLinkedAnimGraphInstanceByTag("Aiming"));
// 	default:
// 		return Cast<UDLLinkedAnimInstance>(GetMainAnimIns()->GetLinkedAnimGraphInstanceByTag("Normal"));;
// 	}
// }

AAnimationTestCharacter* UDLAnimComponentBase::GetCharacterOwner() const
{
	return  Cast<AAnimationTestCharacter>(GetOwner());
}

