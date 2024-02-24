#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "UObject/Interface.h"
#include "GameplayTags.h"
#include "Delegates\IDelegateInstance.h"
#include "ICharacterStateAccessor.h"
#include "ICharacterStateListener.generated.h"


UINTERFACE(MinimalAPI)
class UCharacterStateListener : public UInterface
{
	GENERATED_BODY()
};




class DLGAMEPLAYTYPES_API ICharacterStateListener
{
	GENERATED_BODY()
	
public:

	virtual FOnGameplayAttributeValueChange& OnGameplayAttributeValueChange(const FGameplayAttribute& Attribute) = 0;
};
