#pragma once


#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IDLPlayerSavingAccessor.generated.h"


struct FDLPlayerSetupInfo;
UINTERFACE(MinimalAPI)
class UDLPlayerSavingAccessor
	: public UInterface
{
	GENERATED_BODY()
};



class DLGAMESAVINGSYSTEM_API IDLPlayerSavingAccessor
	: public IInterface
{
	GENERATED_BODY()

public:

	virtual bool ReadSetupInfo(FDLPlayerSetupInfo& OutSetupInfo) = 0;

};
