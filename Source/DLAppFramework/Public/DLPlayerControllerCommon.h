#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DLPlayerControllerCommon.generated.h"

UCLASS(Blueprintable)
class DLAPPFRAMEWORK_API ADLPlayerControllerCommon : public APlayerController
{
	GENERATED_BODY()

public:
	ADLPlayerControllerCommon();

protected:

	virtual void ReceivedPlayer() override;
};
