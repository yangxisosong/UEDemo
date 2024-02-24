#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "DLGameplayCoreSetting.generated.h"



USTRUCT(BlueprintType)
struct FDLCharacterBoneTypeConfig
{
	GENERATED_BODY()

public:

	// 骨架类型
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (Categories = "Gameplay.Character.BoneType"))
		FGameplayTag BoneTypeTag;

	// 骨骼插槽的根节点
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (Categories = "Gameplay.Character.BoneSocket"))
		FGameplayTag BoneSocketRootTag;

	bool operator== (const FGameplayTag& BoneType)const
	{
		return BoneTypeTag == BoneType;
	}
};


USTRUCT(BlueprintType)
struct FDLCharacterBoneSocketTagMapName
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (Categories = "Gameplay.Character.BoneSocket"))
		FGameplayTag BoneSocketTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FName BoneSocketName;

	bool operator== (const FGameplayTag& InBoneSocketTag)const
	{
		return BoneSocketTag == InBoneSocketTag;
	}
};


UCLASS(Config = DLGameplayCore)
class DLGAMEPLAYCORE_API UDLGameplayCoreSetting
	: public UDeveloperSettings
{
public:
	virtual FName GetSectionName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetContainerName() const override;

#if WITH_EDITOR
	virtual FText GetSectionText() const override;
#endif

private:
	GENERATED_BODY()


public:

	/**
	 * @brief  获取配置文件的实例  只读配置文件
	 * @return
	 */
	static const UDLGameplayCoreSetting* Get();

	UFUNCTION(BlueprintPure, Category = DLSettings)
		static const UDLGameplayCoreSetting* GetDLGameplayCoreSetting();

public:

	/**
	 * @brief 不同类型的角色的骨架 的配置信息
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere)
		TArray<FDLCharacterBoneTypeConfig> CharacterBoneTypeConfigs;


	/**
	 * @brief 骨骼插槽Tag -> 骨骼插槽名字， 不管什么骨架类型，都加进来
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere)
		TArray<FDLCharacterBoneSocketTagMapName> TagMapCharacterBoneSocketName;


	/**
	 * @brief 骨骼类型的根Tag
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere)
		FGameplayTag CharacterBoneTypeRootTag;


	/**
	* @brief 单位角色的根Tag
	*/
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere)
		FGameplayTag UnitRoleRootTag;


	/**
	 * @brief 在锁定目标 And Idle状态  转向目标的最大角度差 
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = CharacterMovement)
		float LockUnitIdleRotationYawThreshold = 15.f;


	/**
	 * @brief 锁定目标 转向  右向量 Yaw 的偏移值
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category= CharacterMovement)
		float LockUnitTurnToRightVectorRotationOffset = 5.f;

public:

	
	/**
	 * @brief 标志受击任务 只有 触发 僵直是才 执行
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = Tags)
		FGameplayTag UnderAttackTaskSpasticityTag;


	/**
	 * @brief 角色在大破绽状态
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = Tags)
		FGameplayTag CharacterLoopholesStateTag;


	/**
	* @brief 角色开始回精力 的事件
	*/
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = Tags)
		FGameplayTag StartRecoveryStaminaEventTag;


	/**
	 * @brief 单位死亡的 Tag 状态
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = Tags)
		FGameplayTag UnitDiedStateTag;
};
