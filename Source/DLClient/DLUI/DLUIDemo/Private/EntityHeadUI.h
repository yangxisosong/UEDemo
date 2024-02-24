#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/ICharacterHeadUI.h"
#include "EntityHeadUI.generated.h"


UCLASS()
class UEntityHeadUI
	: public UUserWidget
	, public ICharacterHeadUI
{
	GENERATED_BODY()
};
