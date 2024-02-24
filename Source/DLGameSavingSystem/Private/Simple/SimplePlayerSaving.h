#pragma once

#include "CoreMinimal.h"
#include "DLPlayerSetupInfo.h"
#include "IDLPlayerSavingAccessor.h"
#include "SimplePlayerSaving.generated.h"


UCLASS(Blueprintable)
class USimplePlayerSaving
	: public UObject
	, public IDLPlayerSavingAccessor
{
private:
	GENERATED_BODY()


public:

	virtual bool ReadSetupInfo(FDLPlayerSetupInfo& OutSetupInfo) override
	{

		OutSetupInfo = SetupInfo;

		return true;
	}

public:

	UPROPERTY(EditDefaultsOnly)
		FDLPlayerSetupInfo SetupInfo;
};
