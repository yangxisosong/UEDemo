#pragma once
#include "GameplayEffectTypes.h"
#include "Engine/DataAsset.h"
#include "GameplayFramwork/DLCharacterBase.h"
#include "DLCharacterAssetDef.generated.h"

class UDLCharacterAnimationAssetBase;
USTRUCT(BlueprintType)
struct FCharacterUnderAttackEffectTaskInfo
{
	GENERATED_BODY()
public:

	/**
	 * @brief 使用这个Task需要满足的 Tag
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTagRequirements RequirementsTag;

	/**
	 * @brief 任务的 Class
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowedClasses = "DLUnderAttackTask"))
		TSubclassOf<UObject> TaskClass;
};


UCLASS(BlueprintType)
class DLGAMEPLAYCORE_API UDLCharacterAsset
	: public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UDLCharacterAsset();

public:

	/**
	 * @brief 角色的资产类
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AssetBundles = "Default"))
		TSoftClassPtr<class ADLCharacterBase> CharacterClass;

	/**
	 * @brief 受击后的负面效果的任务
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<FCharacterUnderAttackEffectTaskInfo> UnderAttackEffectTaskArray;


	/**
	 * @brief 定帧的任务资产
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowedClasses = "AnimFrameFrozenTask", AssetBundles = "Default"))
		TSoftClassPtr<UObject> FrameFrozenTaskClass;


	/**
	 * @brief 出破绽的任务资产
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "DLLoopholesTask", AssetBundles = "Default"))
		TSoftClassPtr<UObject> LoopholesTaskClass;


	/**
	 * @brief 角色动画配置资产
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AssetBundles = "Default"))
		TSoftObjectPtr<UDLCharacterAnimationAssetBase> CharacterAnimationAsset;

	/**
	 * @brief 资产类型
	 */
	UPROPERTY(BlueprintReadOnly)
		FPrimaryAssetType AssetType;

public:

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(AssetType, GetFName());
	}
};

