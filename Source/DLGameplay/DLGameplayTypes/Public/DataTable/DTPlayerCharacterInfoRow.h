#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CharacterTypesDef.h"
#include "Engine/DataTable.h"
#include "DataRegistryId.h"
#include "DTPlayerCharacterInfoRow.generated.h"


USTRUCT(BlueprintType)
struct FDLCharacterDefenseAttr
{
	GENERATED_BODY()

public:

	// 物理抗性
	UPROPERTY(EditAnywhere, Category = AttackAttrDefense, BlueprintReadOnly)
		int32 PhysicsDefense = 0;

	// 闪电抗性
	UPROPERTY(EditAnywhere, Category = AttackAttrDefense, BlueprintReadOnly)
		int32 FlashDefense = 0;

	// 冰霜抗性
	UPROPERTY(EditAnywhere, Category = AttackAttrDefense, BlueprintReadOnly)
		int32 IceDefense = 0;

	// 火焰抗性
	UPROPERTY(EditAnywhere, Category = AttackAttrDefense, BlueprintReadOnly)
		int32 FlameDefense = 0;


	// 基础打击抗性
	UPROPERTY(EditAnywhere, Category = AttackTypeDefense, BlueprintReadOnly)
		int32 StrikeDefense = 0;

	// 基础穿刺抗性
	UPROPERTY(EditAnywhere, Category = AttackTypeDefense, BlueprintReadOnly)
		int32 PiercingDefense = 0;

	// 基础切割抗性
	UPROPERTY(EditAnywhere, Category = AttackTypeDefense, BlueprintReadOnly)
		int32 CuttingDefense = 0;

	// 基础爆炸抗性
	UPROPERTY(EditAnywhere, Category = AttackTypeDefense, BlueprintReadOnly)
		int32 BombDefense = 0;


};

USTRUCT(BlueprintType)
struct FDLDestroyPartsAttr
{
	GENERATED_BODY()

public:
	// 部位的索引
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 1, ClampMax = 10))
		int32 Index = 1;

	// 部位Atk破坏值
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 AtkDestroyValue = 0;

	// 部位破坏几率
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 1))
		float DestroyOdds = 0;
};


USTRUCT(BlueprintType)
struct FDLNPCPartsAttribute
{
	GENERATED_BODY()

public:

	// 部位的 Tag
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Gameplay.Character.Parts"))
		FGameplayTag PartTag;

	// 部位的 Index
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 PartIndex = 0;

	// 部位破坏值    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 DestroyMaxValue = 0;

	// 断肢抵抗     
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 MutilationDefense = 0;

	// 基础防御类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayTag DefenseMaterialAttr;

	// 防御属性
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FDLCharacterDefenseAttr DefenseAttr;

public:

	bool operator==(const FGameplayTag& InPartTag)const
	{
		return PartTag == InPartTag;
	}
};


USTRUCT(BlueprintType)
struct FCharacterMovementInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayTag MoveState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float MoveSpeedCoefficient = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float TurnSpeedYaw = 560.f;
};


USTRUCT(BlueprintType)
struct FDTCharacterInfoBaseRow
	: public FTableRowBase
{
	GENERATED_BODY()

public:


	/**
	* Can be overridden by subclasses; Called on every row when the owning data table is modified
	* Allows for custom fix-ups, parsing, etc for user changes
	* This will be called in addition to OnPostDataImport when importing
	*
	* @param InDataTable					The data table that owns this row
	* @param InRowName						The name of the row we're performing fix-up on
	*/
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		BaseInfo.Id = InRowName;
	}

public:

	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FCharacterInfoBase BaseInfo;

	// 生命值上限值
	UPROPERTY(EditAnywhere, Category = BaseInfoValue, BlueprintReadOnly)
		int32 MaxHealth = 0;

	// 攻击力
	UPROPERTY(EditAnywhere, Category = BaseInfoValue, BlueprintReadOnly)
		int32 Attack = 0;

	// 防御值
	UPROPERTY(EditAnywhere, Category = BaseInfoValue, BlueprintReadOnly)
		int32 Defense = 0;

	// 基础的韧性上限值
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 MaxTenacity = 0;

	// 韧性快速恢复速度  单位 恢复点数/0.1s
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		float FastRecoveryTenacitySpeed = 10.f;

	// 韧性慢速恢复速度  单位 恢复点数/0.1s
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		float LowRecoveryTenacitySpeed = 5.f;

	// 隐身等级
	UPROPERTY(EditAnywhere, Category = FieldOfView, BlueprintReadOnly)
		int32 CloakingLevel = 0;

	// 视野距离
	UPROPERTY(EditAnywhere, Category = FieldOfView, BlueprintReadOnly)
		int32 FOVDistance = 0;

	// 基础的速度值  
	UPROPERTY(EditAnywhere, Category = BaseSpeed, BlueprintReadOnly)
		int32 SpeedBase = 0;

	// 锁定状态下的  角度-速度系数 曲线
	UPROPERTY(EditAnywhere, Category = BaseSpeed, BlueprintReadOnly)
		UCurveFloat* LockSpeedCurve = nullptr;

	// 玩家的各种移动配置数据，理论上可以扩展为多种，系统自动根据 Tag 来匹配数据
	UPROPERTY(EditAnywhere, Category = BaseSpeed, BlueprintReadOnly)
		TArray<FCharacterMovementInfo> CharacterMovementInfos;

	// 如果 CharacterMovementInfos  都不比配 则用这个
	UPROPERTY(EditAnywhere, Category = BaseSpeed, BlueprintReadOnly)
		FCharacterMovementInfo DefaultMovementInfo;
};


USTRUCT(BlueprintType)
struct FCharacterBodyCollection
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<FName> BodyName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (Categories = "Gameplay.Character.Parts"))
		FGameplayTag Parts;
};


USTRUCT(BlueprintType)
struct FDTNPCCharacterInfoRow
	: public FDTCharacterInfoBaseRow
{
	GENERATED_BODY()

public:

	// 体力值
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 Strength = 0;


	// 体力恢复的速度   点/秒
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 StrengthRecoverSpeed = 0;

	// 体力消耗的速度   点/秒
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 StrengthCostSpeed = 0;

	// 玩家的可锁定距离
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 10000))
		int32 PlayerCanLockDistance = 0;

	// 玩家的可锁定最大距离
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 10000))
		int32 PlayerCanLockMaxDistance = 0;

	// 被锁定的优先级,  值越大 越优先被锁定
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 BeLockedPriority = 0;

	// 玩家的自由锁定最大距离
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 PlayerCanFreeLockMaxDistance = 0;

	// 玩家的固定锁定的点
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 FixedLockPointIndex = 0;

	// 视野等级
	UPROPERTY(EditAnywhere, Category = FieldOfView, BlueprintReadOnly)
		int32 FOVLevel = 0;

	// 视野距离
	UPROPERTY(EditAnywhere, Category = FieldOfView, BlueprintReadOnly)
		int32 FOVAngle = 0;


	// 脱离战斗 距离 出生点的距离
	UPROPERTY(EditAnywhere, Category = OutOfBattle, BlueprintReadOnly)
		int32 OutOfBattleDistanceFormStartPoint = 0;

	// 脱离战斗 距离 玩家的距离
	UPROPERTY(EditAnywhere, Category = OutOfBattle, BlueprintReadOnly)
		int32 OutOfBattleDistanceFormPlayer = 0;

	// 脱离战斗的未攻击时间 (秒)
	UPROPERTY(EditAnywhere, Category = OutOfBattle, BlueprintReadOnly)
		float OutOfBattleNoAttackTime = 0.f;

	// 脱离战斗的未受到攻击的时间 (秒)
	UPROPERTY(EditAnywhere, Category = OutOfBattle, BlueprintReadOnly)
		float OutOfBattleNoTimeOfAttack = 0.f;

	// 部位的属性
	UPROPERTY(EditAnywhere, Category = PartsData, BlueprintReadOnly)
		TArray<FDLNPCPartsAttribute> PartsAttributeArray;

	// 默认的部位
	UPROPERTY(EditAnywhere, Category = PartsData, BlueprintReadOnly)
		int32 DefaultPartIndex = 0;

#if WITH_EDITORONLY_DATA
	// 目前仅仅需要 BOSS 填写这个字段，物理Body与 身体部位的映射
	UPROPERTY(EditAnywhere, Category = PartsData)
		TArray<FCharacterBodyCollection> CharacterPartsMap;
#endif

	UPROPERTY(VisibleAnywhere, Category = PartsData, BlueprintReadOnly)
		TMap<FName, FGameplayTag> BodyNameMapParts;

private:

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{

		Super::OnDataTableChanged(InDataTable, InRowName);

#if WITH_EDITOR

		BodyNameMapParts.Empty();

		for (const auto& Data : CharacterPartsMap)
		{
			for (const auto& V : Data.BodyName)
			{
				BodyNameMapParts.FindOrAdd(V) = Data.Parts;
			}
		}

#endif

	}
};


USTRUCT(BlueprintType)
struct FDTPlayerCharacterInfoRow
	: public FDTCharacterInfoBaseRow
{
	GENERATED_BODY()

public:

	// 精力值
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 MaxStamina = 0;

	// 快 恢复精力 速度  点/0.1s
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 FastRecoveryStaminaSpeed = 0;

	// 慢 恢复精力 速度  点/0.1s
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 LowRecoveryStaminaSpeed = 0;

	// 冲刺的时候 精力消耗 速度 
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 ConsumeStaminaWhenSprint = 0;

	// 怒气上限
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 MaxRage = 0;

	// 魂力
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 SoulPower = 0;

	// 防御属性
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DefenseAttr)
		FDLCharacterDefenseAttr DefenseAttr;

	// 部位破坏属性 相当于针对各个部位的攻击力
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Destroy)
		TArray<FDLDestroyPartsAttr> DestroyPartsAttrArray;

	// 锁定状态下  角色转向的 阈值 度数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseSpeed)
		float LockTurnMaxThreshold = 30.f;
};




namespace DRCharacter
{
	static FDataRegistryType DRTypeForPlayer(TEXT("CharacterPlayerInfo"));
	static FDataRegistryType DRTypeForNPC(TEXT("CharacterNPCInfo"));

	inline FDataRegistryId CreatePlayerDataRegistryId(const FName CharacterId)
	{
		FDataRegistryId ID;
		ID.RegistryType = DRTypeForPlayer;
		ID.ItemName = CharacterId;
		return ID;
	}

	inline FDataRegistryId CreateNPCDataRegistryId(const FName CharacterId)
	{
		FDataRegistryId ID;
		ID.RegistryType = DRTypeForNPC;
		ID.ItemName = CharacterId;
		return ID;
	}
}

