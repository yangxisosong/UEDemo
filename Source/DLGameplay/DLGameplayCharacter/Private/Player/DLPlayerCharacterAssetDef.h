#pragma once
#include "VCDataAssetDef.h"
#include "Interface/IDLVirtualCameraController.h"
#include "Types/DLCharacterAssetDef.h"
#include "DLPlayerCharacterAssetDef.generated.h"

class UAbilityChainAsset;

USTRUCT(BlueprintType)
struct FDLCharacterMovementVCDefine
{
	GENERATED_BODY()

public:

	/**
	* @brief 移动状态
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FGameplayTag MoveState;

	/**
	* @brief 虚拟相机的资产列表
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<UVCDataAssetDef*> VCDataAssetArray;

	/**
	* @brief 结束后的行为
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		EVCTimeOutAction InTimeOutAction;

public:

	bool operator==(const FGameplayTag InMoveState) const
	{
		return InMoveState.MatchesTag(MoveState);
	}

};

UCLASS(BlueprintType)
class DLGAMEPLAYCHARACTER_API UDLPlayerCharacterAsset
	: public UDLCharacterAsset
{
	GENERATED_BODY()

public:

	/**
	* @brief 不同移动状态下的镜头
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<FDLCharacterMovementVCDefine> MovementVCDefines;

	/**
	* @brief 移动状态的虚拟相机的通道ID
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FGameplayTag MovementVirtualCameraChannelTag;

	/**
	 * @brief 技能连招资产
	 */
	UPROPERTY(EditDefaultsOnly, meta = (AssetBundles = "Client"))
		TSoftObjectPtr<UAbilityChainAsset> AbilityChainAsset;
};

