#include "DamageExecCalc.h"

#include "DLAbilitySystemGlobal.h"
#include "AttributeSet/DLCharacterAttributeSet.h"
#include "Interface/IDLCharacterParts.h"
#include "DLAbilityLog.h"

struct FDLDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalAttackDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FlashAttackDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IceAttackDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FlameAttackDamage);

	FDLDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDLUnitAttributeBaseSet, PhysicalAttackDamage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDLUnitAttributeBaseSet, FlashAttackDamage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDLUnitAttributeBaseSet, IceAttackDamage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDLUnitAttributeBaseSet, FlameAttackDamage, Source, true);




	}
};


UDamageExecCalc::UDamageExecCalc()
{
	this->RelevantAttributesToCapture.Add(FDLDamageStatics().PhysicalAttackDamageDef);
	this->RelevantAttributesToCapture.Add(FDLDamageStatics().FlashAttackDamageDef);
}


static const FDLDamageStatics& DamageStatics()
{
	static FDLDamageStatics DStatics;
	return DStatics;
}

struct FCalcDamageContext
{
public:

	FName DamageType;

	FName AttackType;

	float Damage = 0.f;
};

static FGameplayAttribute GetPropertyNameAttribute(UClass* SetClass, FName PropertyName)
{
	FProperty* Prop = FindFProperty<FProperty>(SetClass, PropertyName);
	return Prop;
}


static void CalcCounteract(const UAbilitySystemComponent* TargetASC, FName DamageTypeDefenseAttrName, FName AttackTypeDefenseAttrName, float& OutDamage)
{
	const auto DamageTypeDefenseAttr = GetPropertyNameAttribute(UDLNPCAttributeSet::StaticClass(), DamageTypeDefenseAttrName);
	const auto AttackTypeDefenseAttr = GetPropertyNameAttribute(UDLNPCAttributeSet::StaticClass(), AttackTypeDefenseAttrName);

	float DamageTypeDefenseValue = 0.f;
	float AttackTypeDefenseValue = 0.f;

	if (DamageTypeDefenseAttr.IsValid())
	{
		DamageTypeDefenseValue = TargetASC->GetNumericAttribute(DamageTypeDefenseAttr);
	}

	if (AttackTypeDefenseAttr.IsValid())
	{
		AttackTypeDefenseValue = TargetASC->GetNumericAttribute(AttackTypeDefenseAttr);
	}

	// 随意写的一个计算
	OutDamage -= DamageTypeDefenseValue;
	OutDamage -= AttackTypeDefenseValue;
	OutDamage = FMath::Max(0.f, OutDamage);
}

// 部位的抗性抵消
static void PartCounteract(const UAbilitySystemComponent* TargetASC, FCalcDamageContext& Context, const int32 Index)
{
	if (FMath::IsNearlyZero(Context.Damage))
	{
		return;
	}

	const FString DamageTypeDefenseName = FString::Printf(TEXT("%sDefense_Part%d"), *Context.DamageType.ToString(), Index);
	const FString AttackTypeDefenseName = FString::Printf(TEXT("%sDefense_Part%d"), *Context.AttackType.ToString(), Index);

	CalcCounteract(TargetASC, *DamageTypeDefenseName, *AttackTypeDefenseName, Context.Damage);
}


static void Counteract(const UAbilitySystemComponent* TargetASC, FCalcDamageContext& Context)
{
	if (FMath::IsNearlyZero(Context.Damage))
	{
		return;
	}

	const FString DamageTypeDefenseName = FString::Printf(TEXT("%sDefense"), *Context.DamageType.ToString());
	const FString AttackTypeDefenseName = FString::Printf(TEXT("%sDefense"), *Context.AttackType.ToString());

	CalcCounteract(TargetASC, *DamageTypeDefenseName, *AttackTypeDefenseName, Context.Damage);
}

// 应用部位破坏
static void ApplyPartDestroy(const UAbilitySystemComponent* SrcASC, const UAbilitySystemComponent* TargetASC, int32 Index, FGameplayEffectCustomExecutionOutput& OutExecutionOutput)
{
	const FString DestroyValueName = FString::Printf(TEXT("DestroyValue_Part%d"), Index);
	const auto DestroyValueNameAttr = GetPropertyNameAttribute(UDLNPCAttributeSet::StaticClass(), *DestroyValueName);

	const FString AtkDestroyValueName = FString::Printf(TEXT("AtkDestroyValue_Part%d"), Index);
	const auto AtkDestroyValueNameAttr = GetPropertyNameAttribute(UDLPlayerAttributeSet::StaticClass(), *AtkDestroyValueName);

	float AtkDestroyValue = 0.f;

	if (AtkDestroyValueNameAttr.IsValid() && SrcASC->HasAttributeSetForAttribute(AtkDestroyValueNameAttr))
	{
		AtkDestroyValue = SrcASC->GetNumericAttributeChecked(AtkDestroyValueNameAttr);
	}
	else
	{
		return;
	}

	if ((!DestroyValueNameAttr.IsValid()) || !TargetASC->HasAttributeSetForAttribute(DestroyValueNameAttr))
	{
		return;
	}

	if (AtkDestroyValue > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DestroyValueNameAttr, EGameplayModOp::Additive, -AtkDestroyValue));
	}
}

void UDamageExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	const UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayTagContainer AssetTags;
	Spec.GetAllAssetTags(AssetTags);

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	static const FGameplayTagContainer AttackTypeRootType = FGameplayTagContainer{ UDLAbilitySystemGlobal::Get().AttackTypeTagRoot };
	auto AttackTypeTags = AssetTags.Filter(AttackTypeRootType);
	ensureAlwaysMsgf(AttackTypeTags.Num() == 1, TEXT("一定会有一个对应的攻击类型，是不是忘记配置了, 可能对伤害结算有影响"));
	FGameplayTag AttackTypeTag = AttackTypeTags.First();
	FString AttackTypeName;
	AttackTypeTag.ToString().Split(TEXT("."), nullptr, &AttackTypeName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

	// Step1 先计算 不同属性造成的伤害

	FCalcDamageContext PhysicalAttackDamage;
	FCalcDamageContext FlashAttackDamage;
	FCalcDamageContext IceAttackDamage;
	FCalcDamageContext FlameAttackDamage;

	PhysicalAttackDamage.DamageType = TEXT("Physics");
	FlashAttackDamage.DamageType = TEXT("Flash");
	IceAttackDamage.DamageType = TEXT("Ice");
	FlameAttackDamage.DamageType = TEXT("Flame");

	PhysicalAttackDamage.AttackType = *AttackTypeName;
	FlashAttackDamage.DamageType = *AttackTypeName;
	IceAttackDamage.DamageType = *AttackTypeName;
	FlameAttackDamage.DamageType = *AttackTypeName;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(FDLDamageStatics().PhysicalAttackDamageDef, EvaluationParameters, PhysicalAttackDamage.Damage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(FDLDamageStatics().FlashAttackDamageDef, EvaluationParameters, FlashAttackDamage.Damage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(FDLDamageStatics().IceAttackDamageDef, EvaluationParameters, IceAttackDamage.Damage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(FDLDamageStatics().FlameAttackDamageDef, EvaluationParameters, FlameAttackDamage.Damage);

	// 根据计算部位的伤害
	IDLCharacterParts* PartsInterface = Cast<IDLCharacterParts>(TargetActor);
	if (PartsInterface)
	{
		int32 PartAttrIndex = PartsInterface->GetDefaultPartAttrIndex();

		// 尝试获取攻击的部位
		const FHitResult* HitResult = Spec.GetContext().GetHitResult();
		if (HitResult)
		{
			const int32 CharacterBoneIndex = HitResult->Item;
			FGameplayTag&& PartTag = PartsInterface->GetPartByBodyIndex(CharacterBoneIndex);
			if (PartTag.IsValid())
			{
				const int32 TempPartAttrIndex = PartsInterface->GetPartAttrIndexByTag(PartTag);
				if (TempPartAttrIndex != INDEX_NONE)
				{
					PartAttrIndex = TempPartAttrIndex;
				}
			}
		}

		// 不同攻击方式 and 不同伤害类型 的 抗性结算
		PartCounteract(TargetAbilitySystemComponent, FlashAttackDamage, PartAttrIndex);
		PartCounteract(TargetAbilitySystemComponent, IceAttackDamage, PartAttrIndex);
		PartCounteract(TargetAbilitySystemComponent, FlameAttackDamage, PartAttrIndex);
		PartCounteract(TargetAbilitySystemComponent, PhysicalAttackDamage, PartAttrIndex);

		// 结算部位破坏，因为部位破坏是不和伤害挂钩的，只要涉及某个部位，都会对部位运行一次 部位破坏的结算
		ApplyPartDestroy(SourceAbilitySystemComponent, TargetAbilitySystemComponent, PartAttrIndex, OutExecutionOutput);
	}
	// 没有部位这种说法的角色
	else
	{
		Counteract(TargetAbilitySystemComponent, FlashAttackDamage);
		Counteract(TargetAbilitySystemComponent, IceAttackDamage);
		Counteract(TargetAbilitySystemComponent, FlameAttackDamage);
		Counteract(TargetAbilitySystemComponent, PhysicalAttackDamage);
	}

	// Step2 应用伤害
	float MitigatedDamage = 0.f;

	MitigatedDamage += PhysicalAttackDamage.Damage;
	MitigatedDamage += FlashAttackDamage.Damage;
	MitigatedDamage += IceAttackDamage.Damage;
	MitigatedDamage += FlameAttackDamage.Damage;

	if (MitigatedDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UDLUnitAttributeBaseSet::GetDamageAttribute(), EGameplayModOp::Additive, MitigatedDamage));
	}
}


