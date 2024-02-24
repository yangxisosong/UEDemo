// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "VCDataAssetDef.h"
#include "Interface/IDLVirtualCameraController.h"
#include "DLGameplayCue.generated.h"


struct FDLGameplayEffectContext;


UCLASS()
class UDLGameplayCueLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category = "Ability|GameplayCue")
		static AActor* GetEffectCauser(const FGameplayCueParameters& Parameters);


	UFUNCTION(BlueprintPure, Category = "Ability|GameplayCue")
		static FGameplayTagContainer GetEffectContextTags(const FGameplayCueParameters& Parameters);


	UFUNCTION(BlueprintPure, Category = "Ability|GameplayCue")
		static AActor* GetEffectTrigger(const FGameplayCueParameters& Parameters);

};



UENUM()
enum class EDLAbilityGameplayCueTargetType : uint8
{
	/**
	 * @brief 仅仅对施法者有效，其他人无法看到这个效果 （本地释放的）
	 */
	OnlyEffectCauser,

	/**
	 * @brief 仅仅对释法的目标有效，其他人无法看到这个效果 （释放者也看不到）
	 */
	 OnlyTarget,

	 /**
	  * @brief 所有人后可以看到这个效果 （包括旁观者）
	  */
	  Both,
};


UCLASS(Abstract, NotBlueprintType, Blueprintable, CollapseCategories, EditInlineNew)
class UDLGameplayCueCondition : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool CanPlayCue(const FGameplayCueParameters& Parameters);

};


UCLASS(NotBlueprintable, Meta = (Displayname = And))
class UDLGameplayCueCondition_And
	: public UDLGameplayCueCondition
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Instanced)
		TArray<UDLGameplayCueCondition*> OperateCondition;


protected:

	virtual bool CanPlayCue_Implementation(const FGameplayCueParameters& Parameters) override
	{
		for (const auto Condition : OperateCondition)
		{
			if (!Condition)
			{
				continue;
			}

			if (!Condition->CanPlayCue(Parameters))
			{
				return false;
			}
		}

		return true;
	}
};


UCLASS(NotBlueprintable, Meta = (Displayname = Or))
class UDLGameplayCueCondition_Or
	: public UDLGameplayCueCondition
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Instanced)
		TArray<UDLGameplayCueCondition*> OperateCondition;

protected:

	virtual bool CanPlayCue_Implementation(const FGameplayCueParameters& Parameters) override
	{
		for (const auto Condition : OperateCondition)
		{
			if (!Condition)
			{
				continue;
			}

			if (Condition->CanPlayCue(Parameters))
			{
				return true;
			}
		}

		return false;
	}
};




USTRUCT(BlueprintType)
struct FDLAbilitySpawnParticleEffect
{
	GENERATED_BODY()
public:

	/**
	 * @brief 播放的目标类型
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		EDLAbilityGameplayCueTargetType ApplyTargetType = EDLAbilityGameplayCueTargetType::Both;

	/**
	 * @brief 播放条件，None 为无条件播放
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced)
		UDLGameplayCueCondition* PlayCondition = nullptr;

	/**
	 * @brief 特效
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UParticleSystem* EmitterTemplate = nullptr;

	/**
	 * @brief 先对击中点的位置偏移
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector LocationOffset = FVector::ZeroVector;

	/**
	* @brief 音源的旋转，正方向是  被击中的Actor的位置 相对 施法者位置 的向量 [ Dir = Src - Target]
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
		FVector Scale = FVector(1.f);
};



UCLASS(Abstract, Blueprintable, CollapseCategories, EditInlineNew)
class UDLAbilitySoundGetter : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		TArray<USoundBase*> GetSound(const FGameplayCueParameters& Arg);

	virtual TArray<USoundBase*> GetSound_Implementation(const FGameplayCueParameters& Arg);

};

UCLASS(DisplayName = SoundAssetsGetter)
class UDLAbilitySoundGetter_Simple : public UDLAbilitySoundGetter
{
	GENERATED_BODY()
public:


	virtual TArray<USoundBase*> GetSound_Implementation(const FGameplayCueParameters& Arg) override
	{
		return Sounds;
	}


protected:


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<USoundBase*> Sounds;
};

UENUM(BlueprintType)
enum class EDLAttackMaterialAttr : uint8
{
	UsedWeapon,
	UsedCustom,
};


UCLASS(Abstract, Config = Game)
class UDLAbilitySoundGetter_AttackMaterial : public UDLAbilitySoundGetter
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
		USoundBase* FindSound(FGameplayTag AttackTypeTag, FGameplayTag DefenseMaterialAttTag,
							  FGameplayTag AttackMaterialAttrTag, FGameplayTag AttackPowerTag);

	UFUNCTION(BlueprintCallable)
		FGameplayTag FindAttackTargetDefenseMaterial(const FGameplayCueParameters& Arg);

public:

	// 攻击材质的类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		EDLAttackMaterialAttr AttackMaterialAttrSourceType;

	// 自定义攻击材质
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "AttackMaterialAttrSourceType == EDLAttackMaterialAttr::UsedCustom", EditConditionHides))
		FGameplayTag CustomAttackMaterialAttr;

	// 攻击的类型  比如是 挥砍、刺 ...
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FGameplayTag AttackType;

	// 攻击的力量Tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FGameplayTag AttackPower;

private:

	TMap<FName, int32> TagMap;

	TMap<int32, int32> RowMap;
};



USTRUCT(BlueprintType)
struct FDLAbilitySpawnSound
{
	GENERATED_BODY()
public:

	/**
	 * @brief 播放的目标类型
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		EDLAbilityGameplayCueTargetType ApplyTargetType = EDLAbilityGameplayCueTargetType::Both;


	/**
	 * @brief 播放条件 ，None 为无条件播放
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced)
		UDLGameplayCueCondition* PlayCondition = nullptr;

	/**
	 * @brief 获取播放的声音来源
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced)
		UDLAbilitySoundGetter* SoundGetter = nullptr;

	/**
	 * @brief 位置偏移
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FVector LocationOffset = FVector::ZeroVector;

	/**
	 * @brief 音源的旋转，正方向是  被击中的Actor的位置 相对 施法者位置 的向量 [ Dir = Src - Target]
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FRotator Rotation = FRotator::ZeroRotator;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, AdvancedDisplay)
		float VolumeMultiplier = 1.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, AdvancedDisplay)
		float PitchMultiplier = 1.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, AdvancedDisplay)
		float StartTime = 0.f;
};


USTRUCT(BlueprintType)
struct FDLAbilitySpawnVirtualCameraEffect
{
	GENERATED_BODY()
public:

	/**
	* @brief 播放的目标类型
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		EDLAbilityGameplayCueTargetType ApplyTargetType = EDLAbilityGameplayCueTargetType::OnlyEffectCauser;

	/**
	* @brief 播放条件 ，None 为无条件播放
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced)
		UDLGameplayCueCondition* PlayCondition = nullptr;

	/**
	 * @brief 虚拟相机的Tag，标记控制那个虚拟相机
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FGameplayTag VirtualCameraChannelTag;

	/**
	 * @brief 虚拟相机的资产列表
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<UVCDataAssetDef*> VCDataAssetArray;

	/**
	 * @brief 结束后的行为
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		EVCTimeOutAction InTimeOutAction = EVCTimeOutAction::Stop;
};


USTRUCT(BlueprintType)
struct FDLAbilitySpawnCameraShake
{
	GENERATED_BODY()

public:

	/**
	* @brief 播放的目标类型
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		EDLAbilityGameplayCueTargetType ApplyTargetType = EDLAbilityGameplayCueTargetType::OnlyEffectCauser;

	/**
	* @brief 播放条件 ，None 为无条件播放
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced)
		UDLGameplayCueCondition* PlayCondition = nullptr;


	/**
	 * @brief CameraShake 的资产文件
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TSubclassOf<class UCameraShakeBase> Shake;


	/**
	 * @brief 缩放播放的速率
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, AdvancedDisplay)
		float Scale = 1.f;

	/**
	 * @brief 播放的坐标空间
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, AdvancedDisplay)
		ECameraShakePlaySpace PlaySpace = ECameraShakePlaySpace::CameraLocal;

	/**
	 * @brief 播放的空间转向
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, AdvancedDisplay)
		FRotator UserPlaySpaceRot = FRotator::ZeroRotator;
};

UENUM(BlueprintType)
enum class EAbilityFrameFrozenTargetType : uint8
{
	// 施法者定帧
	EffectCauser,
	// 被锤的人定帧
	Target,
};


// TODO  需要定制编辑器, 处理定帧Type的问题
USTRUCT(BlueprintType)
struct FDLAbilityFrameFrozen
{
	GENERATED_BODY()

public:

	/**
	 * @brief 定帧的目标类型
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		EAbilityFrameFrozenTargetType FrameFrozenTargetType = EAbilityFrameFrozenTargetType::EffectCauser;

	/**
	 * @brief 播放的条件
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced)
		UDLGameplayCueCondition* PlayCondition = nullptr;

	/**
	* @brief 定帧的持续时间
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Duration = 0.f;

	/**
	* @brief 定帧时的 时间 动画播放速度的曲线  归一化
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UCurveFloat* AnimSpeedCurve = nullptr;

	/**
	* @brief 定帧的类型
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Gameplay.Character.FrameFrozenType"))
		FGameplayTag FrameFrozenType;

	/**
	* @brief 需要定帧的部位
	* @warning 目前只做了四肢
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Gameplay.Character.Parts"))
		FGameplayTagContainer Parts;
};


UCLASS()
class DLABILITYSYSTEM_API UDLGameplayCueHitImpact : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
public:


	// 生成特效
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<FDLAbilitySpawnParticleEffect> SpawnParticleEffectArray;

	// 生成音效、语音
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<FDLAbilitySpawnSound> SpawnSoundArray;

	// 振动屏幕
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<FDLAbilitySpawnVirtualCameraEffect> SpawnVirtualCameraArray;

	// 定帧
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<FDLAbilityFrameFrozen> StartFrameFrozenArray;


	// 生成 CameShake
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<FDLAbilitySpawnCameraShake> SpawnCameraShakeArray;

	// 是否开启调试框
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool DebugDraw = false;

public:

	UFUNCTION(BlueprintCallable)
		void FindBastFrameFrozen(AActor* TargetApplyGC, const FGameplayCueParameters& Arg, bool& IsSuc, TArray<FDLAbilityFrameFrozen>& Ret) const;

	UFUNCTION(BlueprintCallable, meta = (ExpandBoolAsExecs = ReturnValue))
		static bool CheckCanExec(AActor* Target, const FGameplayCueParameters& Parameters, const EDLAbilityGameplayCueTargetType ApplyTargetType);
};



