// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppMemberFunctionMayBeConst
#pragma once

#include "CoreMinimal.h"
#include "DLUnitAttributeBaseSet.h"
#include "GameplayEffectExtension.h"
#include "DLCharacterAttributeSet.generated.h"


UCLASS()
class DLABILITYSYSTEM_API UDLPlayerAttributeSet
	: public UDLUnitAttributeBaseSet
{
	GENERATED_BODY()

public:

	// 精力值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepStamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, Stamina);

	UFUNCTION()
	void OnRepStamina(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, Stamina, Data); };

	// MAX 精力值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, MaxStamina);

	UFUNCTION()
	void OnRepMaxStamina(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, MaxStamina, Data); };


	// 快速恢复 精力值   0.1s
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFastRecoveryStaminaSpeed)
		FGameplayAttributeData FastRecoveryStaminaSpeed;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, FastRecoveryStaminaSpeed);

	UFUNCTION()
		void OnRepFastRecoveryStaminaSpeed(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, FastRecoveryStaminaSpeed, Data); };


	// 慢速恢复 精力值    0.1s
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnLowRecoveryStaminaSpeed)
		FGameplayAttributeData LowRecoveryStaminaSpeed;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, LowRecoveryStaminaSpeed);

	UFUNCTION()
		void OnLowRecoveryStaminaSpeed(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, LowRecoveryStaminaSpeed, Data); };

	// 冲刺的时候，扣除精力 0.1s, Local Only
	UPROPERTY(BlueprintReadOnly)
		FGameplayAttributeData ConsumeStaminaWhenSprint;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, ConsumeStaminaWhenSprint);



	// 怒气上限
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMaxRage)
	FGameplayAttributeData MaxRage;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, MaxRage);

	UFUNCTION()
	void OnRepMaxRage(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, MaxRage, Data); };

	// 魂力
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepSoulPower)
	FGameplayAttributeData SoulPower;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, SoulPower);

	UFUNCTION()
	void OnRepSoulPower(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, SoulPower, Data); };

	// MAX 魂力
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMaxSoulPower)
	FGameplayAttributeData MaxSoulPower;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, MaxSoulPower);

	UFUNCTION()
	void OnRepMaxSoulPower(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, MaxSoulPower, Data); };


	void GetLifetimeReplicatedPropsNormal(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, Tenacity, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, MaxRage, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, SoulPower, COND_None, REPNOTIFY_Always);

		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, MaxSoulPower, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);

		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, FastRecoveryStaminaSpeed, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, LowRecoveryStaminaSpeed, COND_None, REPNOTIFY_Always);
	}

	// 物理抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPhysicsDefense)
	FGameplayAttributeData PhysicsDefense;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, PhysicsDefense);

	UFUNCTION()
	void OnRepPhysicsDefense(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, PhysicsDefense, Data); };


	// 闪电抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlashDefense)
	FGameplayAttributeData FlashDefense;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, FlashDefense);
	UFUNCTION()
	void OnRepFlashDefense(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, FlashDefense, Data); };


	// 冰霜抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepIceDefense)
	FGameplayAttributeData IceDefense;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, IceDefense);

	UFUNCTION()
	void OnRepIceDefense(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, IceDefense, Data); };

	// 火焰抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlameDefense)
	FGameplayAttributeData FlameDefense;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, FlameDefense);

	UFUNCTION()
	void OnRepFlameDefense(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, FlameDefense, Data); };

	// 基础打击抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepStrikeDefense)
	FGameplayAttributeData StrikeDefense;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, StrikeDefense);

	UFUNCTION()
	void OnRepStrikeDefense(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, StrikeDefense, Data); };

	// 基础穿刺抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPiercingDefense)
	FGameplayAttributeData PiercingDefense;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, PiercingDefense);

	UFUNCTION()
	void OnRepPiercingDefense(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, PiercingDefense, Data); };

	// 基础切割抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepCuttingDefense)
	FGameplayAttributeData CuttingDefense;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, CuttingDefense);

	UFUNCTION()
	void OnRepCuttingDefense(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, CuttingDefense, Data); };

	// 基础爆炸抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepBombDefense)
	FGameplayAttributeData BombDefense;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, BombDefense);

	UFUNCTION()
	void OnRepBombDefense(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, BombDefense, Data); };


	void GetLifetimeReplicatedPropsDefense(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, PhysicsDefense, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, FlashDefense, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, IceDefense, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, FlameDefense, COND_None, REPNOTIFY_Always);

		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, StrikeDefense, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, CuttingDefense, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, PiercingDefense, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, BombDefense, COND_None, REPNOTIFY_Always);
	}

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override
	{
		Super::PreAttributeChange(Attribute, NewValue);

		if (Attribute == UDLPlayerAttributeSet::GetStaminaAttribute())
		{
			NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
		}
	}
	
public:


	// 部位破坏值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepAtkDestroyValue_Part1)
	FGameplayAttributeData AtkDestroyValue_Part1;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, AtkDestroyValue_Part1);

	UFUNCTION()
	void OnRepAtkDestroyValue_Part1(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, AtkDestroyValue_Part1, Data); };


	// 部位破坏几率
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepDestroyOdds_Part1)
	FGameplayAttributeData DestroyOdds_Part1;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, DestroyOdds_Part1);

	UFUNCTION()
	void OnRepDestroyOdds_Part1(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, DestroyOdds_Part1, Data); };


	void GetLifetimeReplicatedProps_Part1(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{
		// 部位破坏值
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, AtkDestroyValue_Part1, COND_None, REPNOTIFY_Always);

		// 部位破坏几率
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, DestroyOdds_Part1, COND_None, REPNOTIFY_Always);
	}


	// 部位破坏值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepAtkDestroyValue_Part2)
		FGameplayAttributeData AtkDestroyValue_Part2;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, AtkDestroyValue_Part2);

	UFUNCTION()
		void OnRepAtkDestroyValue_Part2(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, AtkDestroyValue_Part2, Data); };


	// 部位破坏几率
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepDestroyOdds_Part2)
		FGameplayAttributeData DestroyOdds_Part2;
	ATTRIBUTE_ACCESSORS(UDLPlayerAttributeSet, DestroyOdds_Part2);

	UFUNCTION()
		void OnRepDestroyOdds_Part2(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLPlayerAttributeSet, DestroyOdds_Part2, Data); };


	void GetLifetimeReplicatedProps_Part2(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{
		// 部位破坏值
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, AtkDestroyValue_Part2, COND_None, REPNOTIFY_Always);

		// 部位破坏几率
		DOREPLIFETIME_CONDITION_NOTIFY(UDLPlayerAttributeSet, DestroyOdds_Part2, COND_None, REPNOTIFY_Always);
	}

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		this->GetLifetimeReplicatedPropsNormal(OutLifetimeProps);

		this->GetLifetimeReplicatedPropsDefense(OutLifetimeProps);

		this->GetLifetimeReplicatedProps_Part1(OutLifetimeProps);
		this->GetLifetimeReplicatedProps_Part2(OutLifetimeProps);
	}

};

USTRUCT()
struct FCharacterPartAttrInfo
{
	GENERATED_BODY()
public:

	UPROPERTY()
	int32 PartIndex;


};



UCLASS()
class DLABILITYSYSTEM_API UDLNPCAttributeSet
	: public UDLUnitAttributeBaseSet
{
	GENERATED_BODY()

public:

	// 体力值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepStrength)
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, Strength);

	UFUNCTION()
	void OnRepStrength(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, Strength, Data); };

	// Max 体力值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMaxStrength)
	FGameplayAttributeData MaxStrength;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, MaxStrength);

	UFUNCTION()
	void OnRepMaxStrength(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, MaxStrength, Data); };


	// 体力恢复的速度   点/秒
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepStrengthRecoverSpeed)
	FGameplayAttributeData StrengthRecoverSpeed;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, StrengthRecoverSpeed);

	UFUNCTION()
	void OnRepStrengthRecoverSpeed(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, StrengthRecoverSpeed, Data); };


	// 体力消耗的速度   点/秒
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepStrengthCostSpeed)
	FGameplayAttributeData StrengthCostSpeed;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, StrengthCostSpeed);

	UFUNCTION()
	void OnRepStrengthCostSpeed(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, StrengthCostSpeed, Data); };


	void GetLifetimeReplicatedPropsNormal(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, Strength, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, MaxStrength, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, StrengthRecoverSpeed, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, StrengthCostSpeed, COND_None, REPNOTIFY_Always);
	}

public:

#pragma region Part1

	// 物理抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPhysicsDefense_Part1)
		FGameplayAttributeData PhysicsDefense_Part1;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PhysicsDefense_Part1);

	UFUNCTION()
		void OnRepPhysicsDefense_Part1(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part1, Data); };


	// 闪电抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlashDefense_Part1)
		FGameplayAttributeData FlashDefense_Part1;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlashDefense_Part1);

	UFUNCTION()
		void OnRepFlashDefense_Part1(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlashDefense_Part1, Data); };    


	// 冰霜抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepIceDefense_Part1)
		FGameplayAttributeData IceDefense_Part1;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, IceDefense_Part1);

	UFUNCTION()
		void OnRepIceDefense_Part1(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, IceDefense_Part1, Data); };


	// 火焰抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlameDefense_Part1)
		FGameplayAttributeData FlameDefense_Part1;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlameDefense_Part1);

	UFUNCTION()
		void OnRepFlameDefense_Part1(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlameDefense_Part1, Data); };


	// 基础打击抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepStrikeDefense_Part1)
		FGameplayAttributeData StrikeDefense_Part1;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, StrikeDefense_Part1);

	UFUNCTION()
		void OnRepStrikeDefense_Part1(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, StrikeDefense_Part1, Data); };


	// 基础穿刺抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPiercingDefense_Part1)
		FGameplayAttributeData PiercingDefense_Part1;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PiercingDefense_Part1);

	UFUNCTION()
		void OnRepPiercingDefense_Part1(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PiercingDefense_Part1, Data); };


	// 基础切割抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepCuttingDefense_Part1)
		FGameplayAttributeData CuttingDefense_Part1;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, CuttingDefense_Part1);

	UFUNCTION()
		void OnRepCuttingDefense_Part1(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, CuttingDefense_Part1, Data); };


	// 基础爆炸抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepBombDefense_Part1)
		FGameplayAttributeData BombDefense_Part1;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, BombDefense_Part1);

	UFUNCTION()
		void OnRepBombDefense_Part1(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, BombDefense_Part1, Data); };


	// 部位破坏值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepDestroyValue_Part1)
		FGameplayAttributeData DestroyValue_Part1;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, DestroyValue_Part1);

	UFUNCTION()
		void OnRepDestroyValue_Part1(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, DestroyValue_Part1, Data); };


	// 断肢抵抗
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMutilationDefense_Part1)
		FGameplayAttributeData MutilationDefense_Part1;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, MutilationDefense_Part1);

	UFUNCTION()
		void OnRepMutilationDefense_Part1(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, MutilationDefense_Part1, Data); };



	void GetLifetimeReplicatedProps_Part1(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{

		// 物理抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part1, COND_None, REPNOTIFY_Always);

		// 闪电抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlashDefense_Part1, COND_None, REPNOTIFY_Always);

		// 冰霜抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, IceDefense_Part1, COND_None, REPNOTIFY_Always);

		// 火焰抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlameDefense_Part1, COND_None, REPNOTIFY_Always);

		// 基础打击抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, StrikeDefense_Part1, COND_None, REPNOTIFY_Always);

		// 基础穿刺抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PiercingDefense_Part1, COND_None, REPNOTIFY_Always);

		// 基础切割抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, CuttingDefense_Part1, COND_None, REPNOTIFY_Always);

		// 基础爆炸抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, BombDefense_Part1, COND_None, REPNOTIFY_Always);

		// 部位破坏值
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, DestroyValue_Part1, COND_None, REPNOTIFY_Always);

		// 断肢抵抗
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, MutilationDefense_Part1, COND_None, REPNOTIFY_Always);

	}


#pragma endregion

#pragma region Part2

	// 物理抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPhysicsDefense_Part2)
		FGameplayAttributeData PhysicsDefense_Part2;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PhysicsDefense_Part2);

	UFUNCTION()
		void OnRepPhysicsDefense_Part2(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part2, Data); };


	// 闪电抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlashDefense_Part2)
		FGameplayAttributeData FlashDefense_Part2;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlashDefense_Part2);

	UFUNCTION()
		void OnRepFlashDefense_Part2(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlashDefense_Part2, Data); };


	// 冰霜抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepIceDefense_Part2)
		FGameplayAttributeData IceDefense_Part2;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, IceDefense_Part2);

	UFUNCTION()
		void OnRepIceDefense_Part2(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, IceDefense_Part2, Data); };


	// 火焰抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlameDefense_Part2)
		FGameplayAttributeData FlameDefense_Part2;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlameDefense_Part2);

	UFUNCTION()
		void OnRepFlameDefense_Part2(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlameDefense_Part2, Data); };


	// 基础打击抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepStrikeDefense_Part2)
		FGameplayAttributeData StrikeDefense_Part2;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, StrikeDefense_Part2);

	UFUNCTION()
		void OnRepStrikeDefense_Part2(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, StrikeDefense_Part2, Data); };


	// 基础穿刺抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPiercingDefense_Part2)
		FGameplayAttributeData PiercingDefense_Part2;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PiercingDefense_Part2);

	UFUNCTION()
		void OnRepPiercingDefense_Part2(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PiercingDefense_Part2, Data); };


	// 基础切割抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepCuttingDefense_Part2)
		FGameplayAttributeData CuttingDefense_Part2;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, CuttingDefense_Part2);

	UFUNCTION()
		void OnRepCuttingDefense_Part2(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, CuttingDefense_Part2, Data); };


	// 基础爆炸抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepBombDefense_Part2)
		FGameplayAttributeData BombDefense_Part2;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, BombDefense_Part2);

	UFUNCTION()
		void OnRepBombDefense_Part2(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, BombDefense_Part2, Data); };


	// 部位破坏值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepDestroyValue_Part2)
		FGameplayAttributeData DestroyValue_Part2;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, DestroyValue_Part2);

	UFUNCTION()
		void OnRepDestroyValue_Part2(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, DestroyValue_Part2, Data); };


	// 断肢抵抗
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMutilationDefense_Part2)
		FGameplayAttributeData MutilationDefense_Part2;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, MutilationDefense_Part2);

	UFUNCTION()
		void OnRepMutilationDefense_Part2(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, MutilationDefense_Part2, Data); };



	void GetLifetimeReplicatedProps_Part2(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{

		// 物理抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part2, COND_None, REPNOTIFY_Always);

		// 闪电抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlashDefense_Part2, COND_None, REPNOTIFY_Always);

		// 冰霜抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, IceDefense_Part2, COND_None, REPNOTIFY_Always);

		// 火焰抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlameDefense_Part2, COND_None, REPNOTIFY_Always);

		// 基础打击抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, StrikeDefense_Part2, COND_None, REPNOTIFY_Always);

		// 基础穿刺抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PiercingDefense_Part2, COND_None, REPNOTIFY_Always);

		// 基础切割抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, CuttingDefense_Part2, COND_None, REPNOTIFY_Always);

		// 基础爆炸抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, BombDefense_Part2, COND_None, REPNOTIFY_Always);

		// 部位破坏值
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, DestroyValue_Part2, COND_None, REPNOTIFY_Always);

		// 断肢抵抗
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, MutilationDefense_Part2, COND_None, REPNOTIFY_Always);

	}


#pragma endregion

#pragma region Part3

	// 物理抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPhysicsDefense_Part3)
		FGameplayAttributeData PhysicsDefense_Part3;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PhysicsDefense_Part3);

	UFUNCTION()
		void OnRepPhysicsDefense_Part3(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part3, Data); };


	// 闪电抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlashDefense_Part3)
		FGameplayAttributeData FlashDefense_Part3;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlashDefense_Part3);

	UFUNCTION()
		void OnRepFlashDefense_Part3(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlashDefense_Part3, Data); };


	// 冰霜抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepIceDefense_Part3)
		FGameplayAttributeData IceDefense_Part3;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, IceDefense_Part3);

	UFUNCTION()
		void OnRepIceDefense_Part3(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, IceDefense_Part3, Data); };


	// 火焰抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlameDefense_Part3)
		FGameplayAttributeData FlameDefense_Part3;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlameDefense_Part3);

	UFUNCTION()
		void OnRepFlameDefense_Part3(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlameDefense_Part3, Data); };


	// 基础打击抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepStrikeDefense_Part3)
		FGameplayAttributeData StrikeDefense_Part3;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, StrikeDefense_Part3);

	UFUNCTION()
		void OnRepStrikeDefense_Part3(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, StrikeDefense_Part3, Data); };


	// 基础穿刺抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPiercingDefense_Part3)
		FGameplayAttributeData PiercingDefense_Part3;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PiercingDefense_Part3);

	UFUNCTION()
		void OnRepPiercingDefense_Part3(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PiercingDefense_Part3, Data); };


	// 基础切割抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepCuttingDefense_Part3)
		FGameplayAttributeData CuttingDefense_Part3;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, CuttingDefense_Part3);

	UFUNCTION()
		void OnRepCuttingDefense_Part3(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, CuttingDefense_Part3, Data); };


	// 基础爆炸抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepBombDefense_Part3)
		FGameplayAttributeData BombDefense_Part3;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, BombDefense_Part3);

	UFUNCTION()
		void OnRepBombDefense_Part3(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, BombDefense_Part3, Data); };


	// 部位破坏值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepDestroyValue_Part3)
		FGameplayAttributeData DestroyValue_Part3;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, DestroyValue_Part3);

	UFUNCTION()
		void OnRepDestroyValue_Part3(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, DestroyValue_Part3, Data); };


	// 断肢抵抗
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMutilationDefense_Part3)
		FGameplayAttributeData MutilationDefense_Part3;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, MutilationDefense_Part3);

	UFUNCTION()
		void OnRepMutilationDefense_Part3(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, MutilationDefense_Part3, Data); };



	void GetLifetimeReplicatedProps_Part3(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{

		// 物理抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part3, COND_None, REPNOTIFY_Always);

		// 闪电抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlashDefense_Part3, COND_None, REPNOTIFY_Always);

		// 冰霜抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, IceDefense_Part3, COND_None, REPNOTIFY_Always);

		// 火焰抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlameDefense_Part3, COND_None, REPNOTIFY_Always);

		// 基础打击抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, StrikeDefense_Part3, COND_None, REPNOTIFY_Always);

		// 基础穿刺抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PiercingDefense_Part3, COND_None, REPNOTIFY_Always);

		// 基础切割抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, CuttingDefense_Part3, COND_None, REPNOTIFY_Always);

		// 基础爆炸抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, BombDefense_Part3, COND_None, REPNOTIFY_Always);

		// 部位破坏值
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, DestroyValue_Part3, COND_None, REPNOTIFY_Always);

		// 断肢抵抗
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, MutilationDefense_Part3, COND_None, REPNOTIFY_Always);

	}


#pragma endregion

#pragma region Part4

	// 物理抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPhysicsDefense_Part4)
		FGameplayAttributeData PhysicsDefense_Part4;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PhysicsDefense_Part4);

	UFUNCTION()
		void OnRepPhysicsDefense_Part4(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part4, Data); };


	// 闪电抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlashDefense_Part4)
		FGameplayAttributeData FlashDefense_Part4;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlashDefense_Part4);

	UFUNCTION()
		void OnRepFlashDefense_Part4(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlashDefense_Part4, Data); };


	// 冰霜抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepIceDefense_Part4)
		FGameplayAttributeData IceDefense_Part4;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, IceDefense_Part4);

	UFUNCTION()
		void OnRepIceDefense_Part4(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, IceDefense_Part4, Data); };


	// 火焰抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlameDefense_Part4)
		FGameplayAttributeData FlameDefense_Part4;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlameDefense_Part4);

	UFUNCTION()
		void OnRepFlameDefense_Part4(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlameDefense_Part4, Data); };


	// 基础打击抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepStrikeDefense_Part4)
		FGameplayAttributeData StrikeDefense_Part4;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, StrikeDefense_Part4);

	UFUNCTION()
		void OnRepStrikeDefense_Part4(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, StrikeDefense_Part4, Data); };


	// 基础穿刺抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPiercingDefense_Part4)
		FGameplayAttributeData PiercingDefense_Part4;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PiercingDefense_Part4);

	UFUNCTION()
		void OnRepPiercingDefense_Part4(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PiercingDefense_Part4, Data); };


	// 基础切割抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepCuttingDefense_Part4)
		FGameplayAttributeData CuttingDefense_Part4;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, CuttingDefense_Part4);

	UFUNCTION()
		void OnRepCuttingDefense_Part4(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, CuttingDefense_Part4, Data); };


	// 基础爆炸抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepBombDefense_Part4)
		FGameplayAttributeData BombDefense_Part4;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, BombDefense_Part4);

	UFUNCTION()
		void OnRepBombDefense_Part4(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, BombDefense_Part4, Data); };


	// 部位破坏值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepDestroyValue_Part4)
		FGameplayAttributeData DestroyValue_Part4;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, DestroyValue_Part4);

	UFUNCTION()
		void OnRepDestroyValue_Part4(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, DestroyValue_Part4, Data); };


	// 断肢抵抗
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMutilationDefense_Part4)
		FGameplayAttributeData MutilationDefense_Part4;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, MutilationDefense_Part4);

	UFUNCTION()
		void OnRepMutilationDefense_Part4(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, MutilationDefense_Part4, Data); };



	void GetLifetimeReplicatedProps_Part4(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{

		// 物理抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part4, COND_None, REPNOTIFY_Always);

		// 闪电抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlashDefense_Part4, COND_None, REPNOTIFY_Always);

		// 冰霜抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, IceDefense_Part4, COND_None, REPNOTIFY_Always);

		// 火焰抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlameDefense_Part4, COND_None, REPNOTIFY_Always);

		// 基础打击抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, StrikeDefense_Part4, COND_None, REPNOTIFY_Always);

		// 基础穿刺抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PiercingDefense_Part4, COND_None, REPNOTIFY_Always);

		// 基础切割抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, CuttingDefense_Part4, COND_None, REPNOTIFY_Always);

		// 基础爆炸抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, BombDefense_Part4, COND_None, REPNOTIFY_Always);

		// 部位破坏值
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, DestroyValue_Part4, COND_None, REPNOTIFY_Always);

		// 断肢抵抗
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, MutilationDefense_Part4, COND_None, REPNOTIFY_Always);

	}


#pragma endregion

#pragma region Part5

	// 物理抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPhysicsDefense_Part5)
		FGameplayAttributeData PhysicsDefense_Part5;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PhysicsDefense_Part5);

	UFUNCTION()
		void OnRepPhysicsDefense_Part5(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part5, Data); };


	// 闪电抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlashDefense_Part5)
		FGameplayAttributeData FlashDefense_Part5;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlashDefense_Part5);

	UFUNCTION()
		void OnRepFlashDefense_Part5(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlashDefense_Part5, Data); };


	// 冰霜抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepIceDefense_Part5)
		FGameplayAttributeData IceDefense_Part5;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, IceDefense_Part5);

	UFUNCTION()
		void OnRepIceDefense_Part5(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, IceDefense_Part5, Data); };


	// 火焰抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlameDefense_Part5)
		FGameplayAttributeData FlameDefense_Part5;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlameDefense_Part5);

	UFUNCTION()
		void OnRepFlameDefense_Part5(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlameDefense_Part5, Data); };


	// 基础打击抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepStrikeDefense_Part5)
		FGameplayAttributeData StrikeDefense_Part5;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, StrikeDefense_Part5);

	UFUNCTION()
		void OnRepStrikeDefense_Part5(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, StrikeDefense_Part5, Data); };


	// 基础穿刺抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPiercingDefense_Part5)
		FGameplayAttributeData PiercingDefense_Part5;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PiercingDefense_Part5);

	UFUNCTION()
		void OnRepPiercingDefense_Part5(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PiercingDefense_Part5, Data); };


	// 基础切割抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepCuttingDefense_Part5)
		FGameplayAttributeData CuttingDefense_Part5;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, CuttingDefense_Part5);

	UFUNCTION()
		void OnRepCuttingDefense_Part5(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, CuttingDefense_Part5, Data); };


	// 基础爆炸抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepBombDefense_Part5)
		FGameplayAttributeData BombDefense_Part5;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, BombDefense_Part5);

	UFUNCTION()
		void OnRepBombDefense_Part5(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, BombDefense_Part5, Data); };


	// 部位破坏值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepDestroyValue_Part5)
		FGameplayAttributeData DestroyValue_Part5;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, DestroyValue_Part5);

	UFUNCTION()
		void OnRepDestroyValue_Part5(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, DestroyValue_Part5, Data); };


	// 断肢抵抗
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMutilationDefense_Part5)
		FGameplayAttributeData MutilationDefense_Part5;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, MutilationDefense_Part5);

	UFUNCTION()
		void OnRepMutilationDefense_Part5(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, MutilationDefense_Part5, Data); };



	void GetLifetimeReplicatedProps_Part5(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{

		// 物理抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part5, COND_None, REPNOTIFY_Always);

		// 闪电抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlashDefense_Part5, COND_None, REPNOTIFY_Always);

		// 冰霜抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, IceDefense_Part5, COND_None, REPNOTIFY_Always);

		// 火焰抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlameDefense_Part5, COND_None, REPNOTIFY_Always);

		// 基础打击抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, StrikeDefense_Part5, COND_None, REPNOTIFY_Always);

		// 基础穿刺抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PiercingDefense_Part5, COND_None, REPNOTIFY_Always);

		// 基础切割抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, CuttingDefense_Part5, COND_None, REPNOTIFY_Always);

		// 基础爆炸抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, BombDefense_Part5, COND_None, REPNOTIFY_Always);

		// 部位破坏值
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, DestroyValue_Part5, COND_None, REPNOTIFY_Always);

		// 断肢抵抗
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, MutilationDefense_Part5, COND_None, REPNOTIFY_Always);

	}


#pragma endregion

#pragma region Part6

	// 物理抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPhysicsDefense_Part6)
		FGameplayAttributeData PhysicsDefense_Part6;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PhysicsDefense_Part6);

	UFUNCTION()
		void OnRepPhysicsDefense_Part6(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part6, Data); };


	// 闪电抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlashDefense_Part6)
		FGameplayAttributeData FlashDefense_Part6;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlashDefense_Part6);

	UFUNCTION()
		void OnRepFlashDefense_Part6(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlashDefense_Part6, Data); };


	// 冰霜抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepIceDefense_Part6)
		FGameplayAttributeData IceDefense_Part6;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, IceDefense_Part6);

	UFUNCTION()
		void OnRepIceDefense_Part6(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, IceDefense_Part6, Data); };


	// 火焰抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlameDefense_Part6)
		FGameplayAttributeData FlameDefense_Part6;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlameDefense_Part6);

	UFUNCTION()
		void OnRepFlameDefense_Part6(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlameDefense_Part6, Data); };


	// 基础打击抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepStrikeDefense_Part6)
		FGameplayAttributeData StrikeDefense_Part6;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, StrikeDefense_Part6);

	UFUNCTION()
		void OnRepStrikeDefense_Part6(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, StrikeDefense_Part6, Data); };


	// 基础穿刺抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPiercingDefense_Part6)
		FGameplayAttributeData PiercingDefense_Part6;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PiercingDefense_Part6);

	UFUNCTION()
		void OnRepPiercingDefense_Part6(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PiercingDefense_Part6, Data); };


	// 基础切割抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepCuttingDefense_Part6)
		FGameplayAttributeData CuttingDefense_Part6;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, CuttingDefense_Part6);

	UFUNCTION()
		void OnRepCuttingDefense_Part6(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, CuttingDefense_Part6, Data); };


	// 基础爆炸抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepBombDefense_Part6)
		FGameplayAttributeData BombDefense_Part6;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, BombDefense_Part6);

	UFUNCTION()
		void OnRepBombDefense_Part6(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, BombDefense_Part6, Data); };


	// 部位破坏值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepDestroyValue_Part6)
		FGameplayAttributeData DestroyValue_Part6;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, DestroyValue_Part6);

	UFUNCTION()
		void OnRepDestroyValue_Part6(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, DestroyValue_Part6, Data); };


	// 断肢抵抗
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMutilationDefense_Part6)
		FGameplayAttributeData MutilationDefense_Part6;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, MutilationDefense_Part6);

	UFUNCTION()
		void OnRepMutilationDefense_Part6(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, MutilationDefense_Part6, Data); };



	void GetLifetimeReplicatedProps_Part6(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{

		// 物理抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part6, COND_None, REPNOTIFY_Always);

		// 闪电抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlashDefense_Part6, COND_None, REPNOTIFY_Always);

		// 冰霜抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, IceDefense_Part6, COND_None, REPNOTIFY_Always);

		// 火焰抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlameDefense_Part6, COND_None, REPNOTIFY_Always);

		// 基础打击抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, StrikeDefense_Part6, COND_None, REPNOTIFY_Always);

		// 基础穿刺抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PiercingDefense_Part6, COND_None, REPNOTIFY_Always);

		// 基础切割抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, CuttingDefense_Part6, COND_None, REPNOTIFY_Always);

		// 基础爆炸抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, BombDefense_Part6, COND_None, REPNOTIFY_Always);

		// 部位破坏值
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, DestroyValue_Part6, COND_None, REPNOTIFY_Always);

		// 断肢抵抗
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, MutilationDefense_Part6, COND_None, REPNOTIFY_Always);

	}


#pragma endregion

#pragma region Part7

	// 物理抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPhysicsDefense_Part7)
		FGameplayAttributeData PhysicsDefense_Part7;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PhysicsDefense_Part7);

	UFUNCTION()
		void OnRepPhysicsDefense_Part7(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part7, Data); };


	// 闪电抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlashDefense_Part7)
		FGameplayAttributeData FlashDefense_Part7;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlashDefense_Part7);

	UFUNCTION()
		void OnRepFlashDefense_Part7(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlashDefense_Part7, Data); };


	// 冰霜抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepIceDefense_Part7)
		FGameplayAttributeData IceDefense_Part7;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, IceDefense_Part7);

	UFUNCTION()
		void OnRepIceDefense_Part7(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, IceDefense_Part7, Data); };


	// 火焰抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFlameDefense_Part7)
		FGameplayAttributeData FlameDefense_Part7;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, FlameDefense_Part7);

	UFUNCTION()
		void OnRepFlameDefense_Part7(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, FlameDefense_Part7, Data); };


	// 基础打击抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepStrikeDefense_Part7)
		FGameplayAttributeData StrikeDefense_Part7;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, StrikeDefense_Part7);

	UFUNCTION()
		void OnRepStrikeDefense_Part7(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, StrikeDefense_Part7, Data); };


	// 基础穿刺抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepPiercingDefense_Part7)
		FGameplayAttributeData PiercingDefense_Part7;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, PiercingDefense_Part7);

	UFUNCTION()
		void OnRepPiercingDefense_Part7(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, PiercingDefense_Part7, Data); };


	// 基础切割抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepCuttingDefense_Part7)
		FGameplayAttributeData CuttingDefense_Part7;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, CuttingDefense_Part7);

	UFUNCTION()
		void OnRepCuttingDefense_Part7(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, CuttingDefense_Part7, Data); };


	// 基础爆炸抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepBombDefense_Part7)
		FGameplayAttributeData BombDefense_Part7;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, BombDefense_Part7);

	UFUNCTION()
		void OnRepBombDefense_Part7(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, BombDefense_Part7, Data); };


	// 部位破坏值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepDestroyValue_Part7)
		FGameplayAttributeData DestroyValue_Part7;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, DestroyValue_Part7);

	UFUNCTION()
		void OnRepDestroyValue_Part7(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, DestroyValue_Part7, Data); };


	// 断肢抵抗
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMutilationDefense_Part7)
		FGameplayAttributeData MutilationDefense_Part7;
	ATTRIBUTE_ACCESSORS(UDLNPCAttributeSet, MutilationDefense_Part7);

	UFUNCTION()
		void OnRepMutilationDefense_Part7(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLNPCAttributeSet, MutilationDefense_Part7, Data); };



	void GetLifetimeReplicatedProps_Part7(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{

		// 物理抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PhysicsDefense_Part7, COND_None, REPNOTIFY_Always);

		// 闪电抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlashDefense_Part7, COND_None, REPNOTIFY_Always);

		// 冰霜抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, IceDefense_Part7, COND_None, REPNOTIFY_Always);

		// 火焰抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, FlameDefense_Part7, COND_None, REPNOTIFY_Always);

		// 基础打击抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, StrikeDefense_Part7, COND_None, REPNOTIFY_Always);

		// 基础穿刺抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, PiercingDefense_Part7, COND_None, REPNOTIFY_Always);

		// 基础切割抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, CuttingDefense_Part7, COND_None, REPNOTIFY_Always);

		// 基础爆炸抗性
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, BombDefense_Part7, COND_None, REPNOTIFY_Always);

		// 部位破坏值
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, DestroyValue_Part7, COND_None, REPNOTIFY_Always);

		// 断肢抵抗
		DOREPLIFETIME_CONDITION_NOTIFY(UDLNPCAttributeSet, MutilationDefense_Part7, COND_None, REPNOTIFY_Always);

	}


#pragma endregion

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		this->GetLifetimeReplicatedPropsNormal(OutLifetimeProps);

		this->GetLifetimeReplicatedProps_Part1(OutLifetimeProps);
		this->GetLifetimeReplicatedProps_Part2(OutLifetimeProps);
		this->GetLifetimeReplicatedProps_Part3(OutLifetimeProps);
		this->GetLifetimeReplicatedProps_Part4(OutLifetimeProps);
		this->GetLifetimeReplicatedProps_Part5(OutLifetimeProps);
		this->GetLifetimeReplicatedProps_Part6(OutLifetimeProps);
		this->GetLifetimeReplicatedProps_Part7(OutLifetimeProps);

	}

protected:



};