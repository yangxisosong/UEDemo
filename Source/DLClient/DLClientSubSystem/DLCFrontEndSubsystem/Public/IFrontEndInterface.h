#pragma once
#include "CoreMinimal.h"
#include "IFrontEndInterface.generated.h"

class UDLCFrontEndSubsystem;
//前端系统的业务接口
UINTERFACE()
class DLCFRONTENDSUBSYSTEM_API UDLIFrontEndInterface : public UInterface
{
	GENERATED_BODY()
};

class DLCFRONTENDSUBSYSTEM_API IDLIFrontEndInterface : public IInterface
{
	GENERATED_BODY()
public:

	virtual void InitCurrentSubsystem(UDLCFrontEndSubsystem* Subsystem) = 0;
};
