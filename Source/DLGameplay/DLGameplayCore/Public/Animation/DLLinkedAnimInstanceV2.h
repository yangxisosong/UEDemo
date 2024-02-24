// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLAnimInstanceBaseV2.h"

#include "DLLinkedAnimInstanceV2.generated.h"

class UDLMainAnimInstanceV2;

/**
 *
 */
UCLASS()
class DLGAMEPLAYCORE_API UDLLinkedAnimInstanceV2 : public UDLAnimInstanceBaseV2
{
	GENERATED_BODY()
protected:
	//UDLAnimInstanceBaseV2 implements
	virtual bool OnPreInit(const FDLAnimInsInitParams& InitParams) override;

	virtual void OnUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure)
		UDLMainAnimInstanceV2* GetMainAnimInstance() const;

	virtual FAnimCharacterInfoBase& GetPrevAnimCharacterInfoRef() final override;

	virtual void CopyPrevAnimCharacterInfo(FAnimCharacterInfoBase& NewInfo) final override;

	virtual void CopyNewAnimCharacterInfo(float DeltaSeconds)final override {};

	virtual FAnimConfigBase& GetAnimConfigRef() final override;

	virtual void SetAnimConfig(const FAnimConfigBase* InAnimConfig) override {  };
};
