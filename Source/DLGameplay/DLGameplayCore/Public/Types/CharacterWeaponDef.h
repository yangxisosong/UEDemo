#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "CharacterWeaponDef.generated.h"


USTRUCT(BlueprintType)
struct FDLCharacterWeaponAttachInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName WeaponId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FGameplayTag BoneSocketTag;


	bool operator==(FName InWeaponId)const
	{
		return WeaponId == InWeaponId;
	}
};


USTRUCT(BlueprintType)
struct FDLCharacterWeaponInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FPrimaryAssetId WeaponAssetId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FDLCharacterWeaponAttachInfo> WeaponAttachInfos;
};
