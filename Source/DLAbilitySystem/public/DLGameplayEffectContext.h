#pragma once

#include "GameplayEffectTypes.h"
#include "DLGameplayEffectContext.generated.h"

USTRUCT()
struct FDLGameplayEffectContext : public  FGameplayEffectContext
{
	GENERATED_BODY()

public:

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override
	{
		if (!FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess))
		{
			return false;
		}

		if (!AbilityApplyGETags.NetSerialize(Ar, Map, bOutSuccess))
		{
			return false;
		}

		// TODO  使用 位标记 来进行优化

		Ar << GameplayEffectTrigger;

		bOutSuccess = true;
		return true;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FDLGameplayEffectContext::StaticStruct();
	}

	virtual FGameplayEffectContext* Duplicate() const override
	{
		FDLGameplayEffectContext* NewContext = new FDLGameplayEffectContext();
		*NewContext = *this;
		NewContext->AddActors(Actors);
		if (GetHitResult())
		{
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

public:

	// 技能 ApplyGE 的 Tags
	UPROPERTY()
		FGameplayTagContainer AbilityApplyGETags;


	// GE 的触发者，可能是子物体、召唤物 等等
	UPROPERTY()
		TWeakObjectPtr<AActor> GameplayEffectTrigger;
};


template<>
struct TStructOpsTypeTraits<FDLGameplayEffectContext>
	: public TStructOpsTypeTraitsBase2<FDLGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};
