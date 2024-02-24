#pragma once

#include "CoreMinimal.h"
#include "DLCalculateAbilityAttackInfo.h"
#include "Abilities/GameplayAbility.h"
#include "DLAbilitySubObjectLib.generated.h"



USTRUCT(BlueprintType)
struct FDLSubObjectOutgoingHitData
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
		FGameplayTag DefineID;

	UPROPERTY(BlueprintReadWrite)
		FGameplayTagContainer HitTags;

	UPROPERTY(BlueprintReadWrite)
		TArray<FGameplayEffectSpecHandle> OutgoingGESpecHandleArray;

	UPROPERTY(BlueprintReadWrite)
		TSubclassOf<UCalculateAbilityAttackInfo> CalculateAbilityAttackInfoClass;

public:

	bool operator==(const FGameplayTag& Id)const
	{
		return DefineID == Id;
	}
};

USTRUCT(BlueprintType)
struct FDLSubObjectSetupData
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
		TArray<FDLSubObjectOutgoingHitData> OutgoingHitData;

	UPROPERTY(BlueprintReadWrite)
		float LifeTime = 3.f;

	UPROPERTY(BlueprintReadWrite)
		TWeakObjectPtr<UGameplayAbility> InstanceAbility;
};



UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class DLABILITYSYSTEM_API UDLSubObjectSetup : public UInterface
{
	GENERATED_BODY()
};


class DLABILITYSYSTEM_API IDLSubObjectSetup : public IInterface
{
	GENERATED_BODY()
public:

	virtual bool SetupSubObject(const FDLSubObjectSetupData& Data) = 0;

};



UCLASS()
class DLABILITYSYSTEM_API UDLAbilitySubObjectLib
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintCallable)
		static bool MakeSubObjOutgoingHitDefData(UGameplayAbility* Ability, const TArray<FGameplayTag>& HitDefName, TArray<FDLSubObjectOutgoingHitData>& OutHitData);


	UFUNCTION(BlueprintCallable)
		static AActor* MakeSubObjWithTransform(UGameplayAbility* Ability, FGameplayTag SubObjDefName, FTransform Transform);
};



