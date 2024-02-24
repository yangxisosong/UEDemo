#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AbilityChainNodeAssetFactory.generated.h"

class UTreeNodeAsset;
class UAbilityChainAsset;

UCLASS()
class UAbilityChainNodeAssetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	virtual UObject* FactoryCreateNew(UClass* InClass,
	                                  UObject* InParent,
	                                  FName InName,
	                                  EObjectFlags Flags,
	                                  UObject* Context,
	                                  FFeedbackContext* Warn) override;

	virtual bool ConfigureProperties() override;

protected:
	UPROPERTY()
	TSubclassOf<UAbilityChainAsset> AbilityChainNodeAssetClass;
};
