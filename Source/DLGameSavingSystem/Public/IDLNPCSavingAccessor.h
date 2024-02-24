#pragma once

#include "CoreMinimal.h"
#include "DLCharacterSetupDef.h"
#include "UObject/Interface.h"
#include "IDLNPCSavingAccessor.generated.h"

class UGameplayEffect;
class UGameplayAbility;


USTRUCT(BlueprintType)
struct FDTNPCSetupInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TArray<TSoftClassPtr<UGameplayAbility>> SetupAbility;

	UPROPERTY(EditAnywhere)
		TArray<TSoftClassPtr<UGameplayEffect>> SetupGameplayEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FDLSetupWeaponInfo> WeaponInfoArray;
};


UINTERFACE()
class UDLNPCSavingAccessor : public UInterface
{
	GENERATED_BODY()
};


class DLGAMESAVINGSYSTEM_API IDLNPCSavingAccessor: public IInterface
{
	GENERATED_BODY()

public:

	/**
	 * @brief 读取初始化的信息
	 * @param CharacterID  NPC 的ID
	 * @param OutSetupInfo 返回NPC的初始化信息
	 * @return 
	 */
	virtual bool ReadSetupInfo(FName CharacterID, FDTNPCSetupInfo& OutSetupInfo) = 0;
};

