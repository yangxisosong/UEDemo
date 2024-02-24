// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDLPlayerController.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDLPlayerControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class DLABILITYSYSTEM_API IDLPlayerControllerInterface 
{
	GENERATED_BODY()

public:

	virtual FVector GetInputVector() const = 0;
};
