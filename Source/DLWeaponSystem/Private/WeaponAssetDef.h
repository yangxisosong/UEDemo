#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "WeaponAssetDef.generated.h"


class ADLWeaponBase;

UCLASS(BlueprintType)
class UDLWeaponAsset
	: public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UDLWeaponAsset();

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FGameplayTagContainer BaseWeaponTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<TSoftClassPtr<ADLWeaponBase>> WeaponActorClassArr;

	UPROPERTY(BlueprintReadOnly)
		FPrimaryAssetType AssetType;

public:

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(AssetType, GetFName());
	}
};



