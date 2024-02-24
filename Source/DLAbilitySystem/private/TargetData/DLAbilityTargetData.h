#pragma once

#include "CoreMinimal.h"
#include "DLGameplayEffectContext.h"
#include "DLTypeCast.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "DLAbilityTargetData.generated.h"


USTRUCT()
struct FGameplayAbilityTargetData_AttackHitResult : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()
public:

	// ReSharper disable once CppHidingFunction
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		bool OutSuccess = false;

		FGameplayAbilityTargetData_SingleTargetHit::NetSerialize(Ar, Map, bOutSuccess);
		bOutSuccess &= OutSuccess;

		AttackTags.NetSerialize(Ar, Map, OutSuccess);
		bOutSuccess &= OutSuccess;
		
		return true;
	}


	virtual void AddTargetDataToContext(FGameplayEffectContextHandle& Context, bool bIncludeActorArray) const override
	{
		FGameplayAbilityTargetData_SingleTargetHit::AddTargetDataToContext(Context, bIncludeActorArray);

		if (const auto MyContext = StructCast<FDLGameplayEffectContext>(Context.Get()))
		{
			MyContext->AbilityApplyGETags.AppendTags(AttackTags);
		}
	}


	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_AttackHitResult::StaticStruct();
	}


public:

	UPROPERTY()
		FGameplayTagContainer AttackTags;
};


template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_AttackHitResult>
	: public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_AttackHitResult>
{
	enum
	{
		WithNetSerializer = true
	};
};


UCLASS()
class UDLAbilityTargetDataLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
		static FGameplayAbilityTargetDataHandle MakeAttackHitResultsTargetData(TArray<FHitResult> HitRets, const FGameplayTagContainer AttackTags)
	{
		FGameplayAbilityTargetDataHandle Handle;

		for (const auto& HitRet : HitRets)
		{
			auto* Data = new FGameplayAbilityTargetData_AttackHitResult;
			Data->HitResult = HitRet;
			Data->AttackTags = AttackTags;
			Handle.Data.Add(TSharedPtr<FGameplayAbilityTargetData>(Data));
		}

		return Handle;
	}
};
