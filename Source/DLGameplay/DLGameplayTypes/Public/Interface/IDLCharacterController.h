#pragma once

#include "CoreMinimal.h"

#include "IDLCharacterController.generated.h"



UINTERFACE(Meta = (CannotImplementInterfaceInBlueprint), MinimalAPI)
class UDLCharacterController : public UInterface
{
	GENERATED_BODY()
};


class DLGAMEPLAYTYPES_API IDLCharacterController : public IInterface
{
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintCallable)
	virtual void TryTurnToRotation(FRotator Rotator, bool ImmediatelyTurn = false, float OverrideYawSpeed = 0.f) {};


};