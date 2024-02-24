#pragma once
#include "CoreMinimal.h"
#include "IDLNPCSavingAccessor.h"
#include "Engine/DataTable.h"
#include "DLNPCSaved.generated.h"



/**
* @brief 为 NPC 提供初始化的信息，先比与 Play NPC 的初始化信息是固定的 直接读表就可以
*/
USTRUCT(BlueprintType)
struct FDTNPCSetupInfoRow
	: public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TArray<TSoftClassPtr<UGameplayAbility>> SetupAbility;

	UPROPERTY(EditAnywhere)
		TArray<TSoftClassPtr<UGameplayEffect>> SetupGameplayEffect;

	UPROPERTY(EditAnywhere)
		TArray<FDLSetupWeaponInfo> SetupWeaponArray;
};


UCLASS()
class UDLNPCSaved
	: public UObject
	, public IDLNPCSavingAccessor
{
	GENERATED_BODY()
public:


	virtual bool ReadSetupInfo(FName CharacterID, FDTNPCSetupInfo& OutSetupInfo) override;

};

