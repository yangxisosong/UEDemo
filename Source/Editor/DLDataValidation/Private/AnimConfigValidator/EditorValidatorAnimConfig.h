// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataValidation/Public/EditorValidatorBase.h"
#include "EditorValidatorAnimConfig.generated.h"

/**
 * 
 */
UCLASS()
class DLDATAVALIDATION_API UEditorValidatorAnimConfig : public UEditorValidatorBase
{
	GENERATED_BODY()
public:
	UEditorValidatorAnimConfig();

	virtual  bool CanValidateAsset_Implementation(UObject* InAsset) const override;

	virtual EDataValidationResult ValidateLoadedAsset_Implementation(UObject* InAsset, TArray<FText>& ValidationErrors) override;
};
