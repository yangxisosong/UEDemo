// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DLAnimDebugInterface.generated.h"

class UDLAnimInstanceBaseV2;
class UDLCharacterMovementComponent;
class UCapsuleComponent;
// This class does not need to be modified.
UINTERFACE()
class UDLAnimDebugInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class DLGAMEPLAYCORE_API IDLAnimDebugInterface
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable ,Category = "Animation | Debug")
		FRotator GetLastVelocityRotation_Debug();
	virtual FRotator GetLastVelocityRotation_Debug_Implementation() { return FRotator::ZeroRotator; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable ,Category = "Animation | Debug")
		FRotator GetLastMovementInputRotation_Debug();
	virtual FRotator GetLastMovementInputRotation_Debug_Implementation() { return FRotator::ZeroRotator; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable ,Category = "Animation | Debug")
		FRotator GetTargetRotation_Debug();
	virtual FRotator GetTargetRotation_Debug_Implementation() { return FRotator::ZeroRotator; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable ,Category = "Animation | Debug")
		FString GetCharacterMainState_Debug();
	virtual FString GetCharacterMainState_Debug_Implementation() { return ""; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable ,Category = "Animation | Debug")
		FString GetOverlayState_Debug();
	virtual FString GetOverlayState_Debug_Implementation() { return ""; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable ,Category = "Animation | Debug")
		float GetCurveValue_Debug(FName Curve);
	virtual float GetCurveValue_Debug_Implementation(FName Curve) { return 0.0f; }
};
