#include "DLCharacterAttributeSet.h"
#include "DLUnitAttributeBaseSet.h"

#include "DLUnitDamageTypesDef.h"
#include "GameplayEffectExtension.h"
#include "Kismet/GameplayStatics.h"

void UDLUnitAttributeBaseSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if(Attribute == UDLUnitAttributeBaseSet::GetTenacityDynamicAttribute())
	{
		SetTenacityDynamicInternal(FMath::Max(0.f, NewValue));
	}
}

void UDLUnitAttributeBaseSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	
}

void UDLUnitAttributeBaseSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == UDLUnitAttributeBaseSet::GetDamageAttribute())
	{
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if (LocalDamageDone > 0.f)
		{
			const float NewHeath = GetHealth() - LocalDamageDone;
			SetHealth(FMath::Clamp(NewHeath, 0.f, GetMaxHealth()));

			AActor* TargetActor = Data.Target.GetOwnerActor();
			if (TargetActor)
			{
				AActor* SrcActor = Data.EffectSpec.GetEffectContext().GetInstigator();
				UGameplayStatics::ApplyDamage(TargetActor, LocalDamageDone, nullptr, SrcActor, UDLDamage::StaticClass());
				UGameplayStatics::ApplyDamage(TargetActor, LocalDamageDone, nullptr, SrcActor, UDLRealDamage::StaticClass());
			}
		}
	}
	else if(Data.EvaluatedData.Attribute == UDLUnitAttributeBaseSet::GetTenacityDynamicAttribute())
	{
		SetTenacityDynamicInternal(FMath::Max(0.f, GetTenacityDynamic()));
	}
	else if(Data.EvaluatedData.Attribute == UDLUnitAttributeBaseSet::GetTenacityDamageAttribute())
	{
		const float LocalTenacityDamage = GetTenacityDamage();
		SetTenacityDamage(0.f);

		if (LocalTenacityDamage > 0.f)
		{
			// 先用 Dy 值 抵扣

			const float NewTenacityDynamic = GetTenacityDynamicInternal() - LocalTenacityDamage;
			SetTenacityDynamicInternal(FMath::Max(0.f, NewTenacityDynamic));

			// 没抵扣完
			if (NewTenacityDynamic <.0f)
			{
				const float NewTenacity = GetTenacity() + NewTenacityDynamic;
				SetTenacity(FMath::Clamp(NewTenacity, 0.f, GetMaxTenacity()));
			}
		}
	}
	else if(Data.EvaluatedData.Attribute == UDLUnitAttributeBaseSet::GetTenacityAttribute())
	{
		// 避免恢复的时候 直接回多了
		SetTenacity(FMath::Min(GetTenacity(), GetMaxTenacity()));
	}
}
