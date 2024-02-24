#pragma once

#include "CoreMinimal.h"

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

#include "DLUnitAttributeBaseSet.generated.h"



#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class DLABILITYSYSTEM_API UDLUnitAttributeBaseSet
	: public UAttributeSet
{

private:
	GENERATED_BODY()
public:

	UDLUnitAttributeBaseSet() {}

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepHealth)
		FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMaxHealth)
		FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, MaxHealth);


	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepAttack)
		FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, Attack);


	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMaxSpeed)
		FGameplayAttributeData SpeedBase;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, SpeedBase);


	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMaxSpeedTotalScale)
		FGameplayAttributeData MaxSpeedTotalScale;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, MaxSpeedTotalScale);


	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepAbilityCDCoefficient)
		FGameplayAttributeData AbilityCDCoefficient;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, AbilityCDCoefficient);


public:

	// 物理攻击值伤害值
	UPROPERTY(BlueprintReadOnly)
		FGameplayAttributeData PhysicalAttackDamage;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, PhysicalAttackDamage);

	// 闪电攻击值伤害值
	UPROPERTY(BlueprintReadOnly)
		FGameplayAttributeData FlashAttackDamage;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, FlashAttackDamage);


	// 冰霜攻击值伤害值
	UPROPERTY(BlueprintReadOnly)
		FGameplayAttributeData IceAttackDamage;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, IceAttackDamage);


	// 火焰攻击值伤害值
	UPROPERTY(BlueprintReadOnly)
		FGameplayAttributeData FlameAttackDamage;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, FlameAttackDamage);


	UPROPERTY(BlueprintReadOnly)
		FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, Damage);


	// 最大的韧性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepMaxTenacity)
		FGameplayAttributeData MaxTenacity;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, MaxTenacity);

	// 韧性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepTenacity)
		FGameplayAttributeData Tenacity;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, Tenacity);


	// 出手韧性 （这个就是一个 韧性的护盾）
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepTenacityDynamic)
		FGameplayAttributeData TenacityDynamic;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, TenacityDynamic);

	// 出手韧性的内部实现
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepTenacityDynamicInternal)
	FGameplayAttributeData TenacityDynamicInternal;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, TenacityDynamicInternal);

	// 削韧减免百分比
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepCutTenacityImmuneProportion)
		FGameplayAttributeData CutTenacityImmuneProportion;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, CutTenacityImmuneProportion);

	// 韧性扣除数
	UPROPERTY(BlueprintReadOnly)
		FGameplayAttributeData TenacityDamage;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, TenacityDamage);

	// 快速恢复韧性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepFastRecoveryTenacitySpeed)
		FGameplayAttributeData FastRecoveryTenacitySpeed;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, FastRecoveryTenacitySpeed);

	// 慢速恢复韧性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepLowRecoveryTenacitySpeed)
		FGameplayAttributeData LowRecoveryTenacitySpeed;
	ATTRIBUTE_ACCESSORS(UDLUnitAttributeBaseSet, LowRecoveryTenacitySpeed);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME_CONDITION_NOTIFY(UDLUnitAttributeBaseSet, Health, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLUnitAttributeBaseSet, MaxHealth, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLUnitAttributeBaseSet, Attack, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLUnitAttributeBaseSet, SpeedBase, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLUnitAttributeBaseSet, MaxSpeedTotalScale, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLUnitAttributeBaseSet, AbilityCDCoefficient, COND_None, REPNOTIFY_Always);


		DOREPLIFETIME_CONDITION_NOTIFY(UDLUnitAttributeBaseSet, MaxTenacity, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLUnitAttributeBaseSet, Tenacity, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLUnitAttributeBaseSet, TenacityDynamic, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLUnitAttributeBaseSet, CutTenacityImmuneProportion, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLUnitAttributeBaseSet, FastRecoveryTenacitySpeed, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLUnitAttributeBaseSet, LowRecoveryTenacitySpeed, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UDLUnitAttributeBaseSet, TenacityDynamicInternal, COND_None, REPNOTIFY_Always);
	}


	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override{ return  Super::PreGameplayEffectExecute(Data);}

	// GE 执行后
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:

	UFUNCTION()
		void OnRepHealth(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLUnitAttributeBaseSet, Health, Data); };

	UFUNCTION()
		void OnRepMaxHealth(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLUnitAttributeBaseSet, MaxHealth, Data); };

	UFUNCTION()
		void OnRepMaxSpeed(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLUnitAttributeBaseSet, SpeedBase, Data); };

	UFUNCTION()
		void OnRepAttack(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLUnitAttributeBaseSet, Attack, Data); };

	UFUNCTION()
		void OnRepAbilityCDCoefficient(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLUnitAttributeBaseSet, AbilityCDCoefficient, Data); };

	UFUNCTION()
		void OnRepMaxSpeedTotalScale(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLUnitAttributeBaseSet, MaxSpeedTotalScale, Data); };

	UFUNCTION()
		void OnRepTenacity(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLUnitAttributeBaseSet, Tenacity, Data); };

	UFUNCTION()
		void OnRepTenacityDynamic(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLUnitAttributeBaseSet, TenacityDynamic, Data); };

	UFUNCTION()
		void OnRepTenacityDynamicInternal(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLUnitAttributeBaseSet, TenacityDynamicInternal, Data); };

	UFUNCTION()
		void OnRepCutTenacityImmuneProportion(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLUnitAttributeBaseSet, CutTenacityImmuneProportion, Data); };

	UFUNCTION()
		void OnRepMaxTenacity(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLUnitAttributeBaseSet, MaxTenacity, Data); };

	UFUNCTION()
		void OnRepFastRecoveryTenacitySpeed(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLUnitAttributeBaseSet, FastRecoveryTenacitySpeed, Data); };

	UFUNCTION()
		void OnRepLowRecoveryTenacitySpeed(const FGameplayAttributeData& Data) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDLUnitAttributeBaseSet, LowRecoveryTenacitySpeed, Data); };

};


inline void InitAttribute(UDLUnitAttributeBaseSet* Attribute, const FName Name, const float Value)
{
	const FStructProperty* StructProperty = FindFieldChecked<FStructProperty>(Attribute->GetClass(), Name);
	if (StructProperty)
	{
		FGameplayAttributeData* DataPtr = StructProperty->ContainerPtrToValuePtr<FGameplayAttributeData>(Attribute);
		if (ensure(DataPtr))
		{
			DataPtr->SetBaseValue(Value);
			DataPtr->SetCurrentValue(Value);
		}
	}
};