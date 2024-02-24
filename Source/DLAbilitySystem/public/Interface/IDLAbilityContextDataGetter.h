#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDLAbilityContextDataGetter.generated.h"

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint), MinimalAPI)
class UDLAbilityContextDataGetter : public UInterface
{
	GENERATED_BODY()
};


USTRUCT(BlueprintType)
struct FAbilityRollContext
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator TurnToRotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bImmediatelyTurn = false;


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator InputOrientationRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator InputRollRotation = FRotator::ZeroRotator;

	UPROPERTY()
		bool bValid = false;

public:

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << bValid;

		// 如果无效的数据，那么就跳过序列化与反序列化 节省带宽
		if (bValid)
		{
			Ar << bImmediatelyTurn;
			Ar << TurnToRotation;
			Ar << InputOrientationRotation;
			Ar << InputRollRotation;
		}

		bOutSuccess = true;
		return true;
	}

};

template<>
struct TStructOpsTypeTraits<FAbilityRollContext> : public TStructOpsTypeTraitsBase2<FAbilityRollContext>
{
	enum
	{
		WithNetSerializer = true,
	};
};


#define ENSURE_IMP(FUNC_NAME)  ensureAlwaysMsgf(false, TEXT("%s Not Imp"), TEXT(#FUNC_NAME)); return {}; 

class DLABILITYSYSTEM_API IDLAbilityContextDataGetter
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = AbilityContextDataGetter)
		virtual  FAbilityRollContext GetRollContext() const { ENSURE_IMP(GetRollContext) }

};
