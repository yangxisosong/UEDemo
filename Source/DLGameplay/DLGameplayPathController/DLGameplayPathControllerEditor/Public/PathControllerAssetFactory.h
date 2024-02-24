#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "PathControllerAssetFactory.generated.h"


class UPathControllerTraceAsset;

UCLASS()
class UPathControllerAssetFactory : public UFactory
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
	TSubclassOf<UPathControllerTraceAsset> PCAssetClass;
};
