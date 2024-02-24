// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "IDLNPCState.generated.h"



UINTERFACE(MinimalAPI)
class UDLNPCStateInterface : public UInterface
{
	GENERATED_BODY()
};


class DLGAMEPLAYTYPES_API IDLNPCStateInterface
{
	GENERATED_BODY()
	
public:

	virtual FName GetNPCStartPointId() const = 0;
	virtual void SetNPCStartPointId(FName Id) = 0;

};
