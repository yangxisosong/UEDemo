#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "IPlayerAttributeAccessor.generated.h"


UINTERFACE(MinimalAPI)
class UPlayerAttributeAccessor
	: public UInterface
{
	GENERATED_BODY()
};

struct FGameplayAttribute;

class DLGAMEPLAYTYPES_API IPlayerAttributeAccessor
{
	GENERATED_BODY()

public:

	virtual  float GetAttributeValue(const FGameplayAttribute& Attribute) const = 0;

	virtual FOnGameplayAttributeValueChange& OnGameplayAttributeValueChange(const FGameplayAttribute& Attribute) = 0;

};

