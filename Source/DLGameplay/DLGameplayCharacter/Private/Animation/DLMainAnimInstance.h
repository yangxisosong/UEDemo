// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/DLAnimInstanceBase.h"
#include "DLMainAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class DLGAMEPLAYCHARACTER_API UDLMainAnimInstance
	: public UDLAnimInstanceBase
{
	GENERATED_BODY()

public:

	virtual void OnUpdateAnimation(float DeltaSeconds) override {  };
};
