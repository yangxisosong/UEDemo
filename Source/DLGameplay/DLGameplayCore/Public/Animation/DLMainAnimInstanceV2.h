// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLAnimInstanceBaseV2.h"
#include "DLMainAnimInstanceV2.generated.h"

/**
 *
 */
UCLASS()
class DLGAMEPLAYCORE_API UDLMainAnimInstanceV2 : public UDLAnimInstanceBaseV2
{
	GENERATED_BODY()
protected:
	virtual bool OnPreInit(const FDLAnimInsInitParams& InitParams) override;

	virtual bool ShouldMoveCheck() { return false; };
private:
	void InitLinkedAnimInstance(const FDLAnimInsInitParams& InitParams) const;
};
