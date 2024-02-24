// Fill out your copyright notice in the Description page of Project Settings.


#include "DLLinkedAnimInstance.h"

#include "AnimationTestCharacter.h"
#include "DLMainAnimInstance.h"

UDLMainAnimInstance* UDLLinkedAnimInstance::GetMainAnimIns() const
{
	if (GetOwnerCharacter())
	{
		return Cast<UDLMainAnimInstance>(GetOwnerCharacter()->GetMesh()->GetAnimInstance());
	}
	return nullptr;
}
