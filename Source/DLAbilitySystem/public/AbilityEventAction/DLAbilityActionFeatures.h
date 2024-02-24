#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "DLAbilityActionFeatures.generated.h"

class IDLWeapon;
UINTERFACE()
class UDLAbilityAFHitActor
	: public UInterface
{
	GENERATED_BODY()
};


class IDLAbilityAFHitActor
	: public IInterface
{
	GENERATED_BODY()
public:

	virtual FGameplayAbilityTargetDataHandle GetTargetData() const = 0;
};

UINTERFACE()
class UDLAbilityAFTags
	: public UInterface
{
	GENERATED_BODY()
};

class IDLAbilityAFTags
	: public IInterface
{
	GENERATED_BODY()
public:

	virtual FGameplayTagContainer GetGameplayTags() const = 0;
};

UINTERFACE()
class UDLAbilityAFHitResults
	: public UInterface
{
	GENERATED_BODY()
};


class IDLAbilityAFHitResults
	: public IInterface
{
	GENERATED_BODY()
public:

	virtual const TArray<FHitResult>& GetHitResults() const = 0;
};


UINTERFACE()
class UDLAbilityAFDefenseMaterialAttr
	: public UInterface
{
	GENERATED_BODY()
};


class IDLAbilityAFDefenseMaterialAttr
	: public IInterface
{
	GENERATED_BODY()
public:

	virtual FGameplayTag GetDefenseMaterialAttr() = 0;
};


UINTERFACE()
class UDLAbilityAFAttackWeapon
	: public UInterface
{
	GENERATED_BODY()
};


class IDLAbilityAFAttackWeapon
	: public IInterface
{
	GENERATED_BODY()
public:

	virtual TScriptInterface<IDLWeapon> GetAttackWeapon() = 0;
};


UINTERFACE()
class UDLAbilityAFTimeInterval
	: public UInterface
{
	GENERATED_BODY()
};


class IDLAbilityAFTimeInterval
	: public IInterface
{
	GENERATED_BODY()
public:

	virtual float GetTimeInterval() = 0;
};

