#pragma once
#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "GameFramework/Character.h"
#include "AbilitySystemAutomationTest.generated.h"


UCLASS()
class AAbilitySystemTestCharacter
		: public ACharacter
		, public IGameplayTagAssetInterface
{

	GENERATED_BODY()
public:

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		TagContainer.AppendTags(MyTags);
	}


	FGameplayTagContainer MyTags;
};
