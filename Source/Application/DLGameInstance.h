#pragma once


#include "CoreMinimal.h"
#include "IDLClientAppInstance.h"
#include "DLGameInstance.generated.h"


UCLASS()
class DLAPPLICATION_API UDLGameInstance
	: public UGameInstance
	, public IDLClientAppInstance
{
	GENERATED_BODY()
public:

	virtual void Init() override;

	virtual int32 AddLocalPlayer(ULocalPlayer* NewPlayer, int32 ControllerId) override;

public:

	virtual IDLLocalPlayerGameplayInterface* GetGameplayInterface() override;

};


