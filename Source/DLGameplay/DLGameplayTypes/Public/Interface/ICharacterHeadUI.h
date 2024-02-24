#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ICharacterHeadUI.generated.h"


UINTERFACE(MinimalAPI)
class UCharacterHeadUI
	: public UInterface
{
	GENERATED_BODY()
};


class DLGAMEPLAYTYPES_API ICharacterHeadUI : public IInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHeath(float CurrentValue, float MaxValue);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMana(float CurrentValue, float MaxValue);
};
