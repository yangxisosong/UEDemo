// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLMainAnimInstance.h"
#include "DLZhuZhuQingMainAnimIns.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class DLGAMEPLAYCHARACTER_API UDLZhuZhuQingMainAnimIns : public UDLMainAnimInstance
{
	GENERATED_BODY()
public:
#pragma region override animins method

	virtual void OnUpdateAnimation(float DeltaSeconds) override;

#pragma endregion override animins method
};
