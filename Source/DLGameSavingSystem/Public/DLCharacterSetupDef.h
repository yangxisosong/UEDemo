#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DLCharacterSetupDef.generated.h"




USTRUCT(BlueprintType)
struct FDLSetupWeaponAttachInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName WeaponId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta= (Categories="Gameplay.Character.BoneSocket"))
		FGameplayTag BoneSocketTag;
};


USTRUCT(BlueprintType)
struct FDLSetupWeaponInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FPrimaryAssetId WeaponAssetId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FDLSetupWeaponAttachInfo> WeaponAttachInfos;	
};