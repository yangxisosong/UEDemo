// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLAnimInstanceBase.h"
#include "DLRiderAnimMainInstance.generated.h"

/**
 *
 */
UCLASS()
class DLGAMEPLAYCHARACTER_API UDLRiderAnimMainInstance : public UDLAnimInstanceBase
{
	GENERATED_BODY()

protected:

	virtual void OnUpdateAnimation(float DeltaSeconds) override { Super::OnUpdateAnimation(DeltaSeconds); };
};
