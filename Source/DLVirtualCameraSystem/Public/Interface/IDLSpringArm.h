// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDLSpringArm.generated.h"


UINTERFACE(BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UDLSpringArm : public UInterface
{
	GENERATED_BODY()
};

class IDLSpringArm
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,Category = "VirtualCamera")
	virtual  void BeginArmLength(float Duration,UAnimSequenceBase* Anim) = 0;
	
	UFUNCTION(BlueprintCallable,Category = "VirtualCamera")
	virtual  void TickArmLength(const UCurveFloat* Curve,float TickTime) = 0;
	
	UFUNCTION(BlueprintCallable,Category = "VirtualCamera")
	virtual  void EndArmLength() = 0;
	
};
