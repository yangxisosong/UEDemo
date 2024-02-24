#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "DLAbilityValidator.generated.h"


UCLASS()
class DLDATAVALIDATION_API UDLAbilityValidator : public UEditorValidatorBase
{
	GENERATED_BODY()
public:

	virtual  bool CanValidateAsset_Implementation(UObject* InAsset) const override;

	virtual EDataValidationResult ValidateLoadedAsset_Implementation(UObject* InAsset, TArray<FText>& ValidationErrors) override;
};
