#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTags.h"
#include "IDLAbilitySysComponent.generated.h"


class UGameplayAbility;


UINTERFACE(Meta = (CannotImplementInterfaceInBlueprint), MinimalAPI)
class UDLAbilitySysComponent
	: public UInterface
{
	GENERATED_BODY()
};


class DLABILITYSYSTEM_API IDLAbilitySysComponent : public IInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Displayname = GetAnimatingAbility)
		virtual UGameplayAbility* K2_GetAnimatingAbility() { return nullptr; };


	UFUNCTION(BlueprintCallable)
		virtual void AppendAbilityBeCancelAbilityTag(UGameplayAbility* TargetAbility, const FGameplayTagContainer& AbilityTag) = 0;

	UFUNCTION(BlueprintCallable)
		virtual void RemoveAbilityBeCancelAbilityTag(UGameplayAbility* TargetAbility, const FGameplayTagContainer& AbilityTag) = 0;
};
