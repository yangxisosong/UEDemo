#pragma once

#include "CoreMinimal.h"
#include "DLAbilityTargetSelector.h"
#include "Factories/Factory.h"
#include "TargetSelectorAssetFactory.generated.h"


class UDLGameplayTargetSelectorBase;

UCLASS()
class UTargetSelectorAssetFactory : public UFactory
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
	TSubclassOf<UDLGameplayTargetSelectorBase> TargetSelectorAssetClass;
};
