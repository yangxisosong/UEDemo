// Fill out your copyright notice in the Description page of Project Settings.


#include "DLAbilityActorInfo.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Interface/IDLCharacterController.h"

void FDLAbilityActorInfo::InitFromActor(AActor* InOwnerActor, AActor* InAvatarActor,
                                        UAbilitySystemComponent* InAbilitySystemComponent)
{
	AActor *TestActor = InOwnerActor;
	while (TestActor)
	{
		if (IDLCharacterController * CastPC = Cast<IDLCharacterController>(TestActor))
		{
			CharacterController = TestActor;
			break;
		}

		TestActor = TestActor->GetOwner();
	}


	FGameplayAbilityActorInfo::InitFromActor(InOwnerActor, InAvatarActor, InAbilitySystemComponent);
}

IDLCharacterController* FDLAbilityActorInfo::GetCharacterController() const
{
	return Cast<IDLCharacterController>(CharacterController.Get());
}
