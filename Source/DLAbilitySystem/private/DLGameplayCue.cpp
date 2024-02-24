// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameplayCue.h"
#include "DataRegistrySubsystem.h"
#include "DLAbilityLog.h"
#include "DLAbilitySystemGlobal.h"
#include "DLGameplayEffectContext.h"
#include "DLTypeCast.h"
#include "DataTable/DTAbilityAudioRow.h"

AActor* UDLGameplayCueLib::GetEffectCauser(const FGameplayCueParameters& Parameters)
{
	return Parameters.GetEffectCauser();
}

FGameplayTagContainer UDLGameplayCueLib::GetEffectContextTags(const FGameplayCueParameters& Parameters)
{
	if (Parameters.EffectContext.IsValid())
	{
		if (const FDLGameplayEffectContext* Context = StructCast<FDLGameplayEffectContext>(Parameters.EffectContext.Get()))
		{
			return Context->AbilityApplyGETags;
		}
	}
	return {};
}

AActor* UDLGameplayCueLib::GetEffectTrigger(const FGameplayCueParameters& Parameters)
{
	if (Parameters.EffectContext.IsValid())
	{
		if (const FDLGameplayEffectContext* Context = StructCast<FDLGameplayEffectContext>(Parameters.EffectContext.Get()))
		{
			return Context->GameplayEffectTrigger.Get();
		}
	}
	return {};
}


bool UDLGameplayCueCondition::CanPlayCue_Implementation(const FGameplayCueParameters& Parameters)
{
	return false;
}


TArray<USoundBase*> UDLAbilitySoundGetter::GetSound_Implementation(const FGameplayCueParameters& Arg)
{
	return {};
}


USoundBase* UDLAbilitySoundGetter_AttackMaterial::FindSound(FGameplayTag AttackTypeTag, FGameplayTag DefenseMaterialAttTag,
															FGameplayTag AttackMaterialAttrTag, FGameplayTag AttackPowerTag)
{
	const FString DebugArg = FString::Printf(TEXT("Attack %s, Defense %s, Type %s, Power %s"),
											 *AttackMaterialAttrTag.ToString(), *DefenseMaterialAttTag.ToString(),
											 *AttackTypeTag.ToString(), *AttackPowerTag.ToString());

	const auto DT = UDLAbilitySystemGlobal::Get().DTAbilityAudio.LoadSynchronous();
	if (!ensureMsgf(DT, TEXT("查不到数据表，可能音频播放异常 Arg %s"), *DebugArg))
	{
		return nullptr;
	}

	TArray<FDTAttackAudioRow*> AllRowData;
	DT->GetAllRows<FDTAttackAudioRow>(TEXT("加载数据音频的配置表数据"), AllRowData);


	UE_LOG(LogDLAbilityCue, Log, TEXT("FindSound Arg : %s"), *DebugArg);


	int32 AttackTypeIndex = 0;
	int32 AttackPowerIndex = 0;
	int32 AttackMaterialAttrIndex = 0;
	int32 DefenseMaterialAttIndex = 0;
	if (TagMap.Num() == 0)
	{
		for (int i = 0; i < AllRowData.Num(); i++)
		{
			if (TagMap.Contains(AllRowData[i]->AttackType.GetTagName()))
			{
				AttackTypeIndex = TagMap[AllRowData[i]->AttackType.GetTagName()];
			}
			else
			{
				auto AddNum = TagMap.Num();
				TagMap.Add(AllRowData[i]->AttackType.GetTagName(), AddNum);
				AttackTypeIndex = AddNum;
			}

			if (TagMap.Contains(AllRowData[i]->AttackPower.GetTagName()))
			{
				AttackPowerIndex = TagMap[AllRowData[i]->AttackPower.GetTagName()];
			}
			else
			{
				auto AddNum = TagMap.Num();
				TagMap.Add(AllRowData[i]->AttackPower.GetTagName(), AddNum);
				AttackPowerIndex = AddNum;
			}
			if (TagMap.Contains(AllRowData[i]->AttackMaterialAttr.GetTagName()))
			{
				AttackMaterialAttrIndex = TagMap[AllRowData[i]->AttackMaterialAttr.GetTagName()];
			}
			else
			{
				auto AddNum = TagMap.Num();
				TagMap.Add(AllRowData[i]->AttackMaterialAttr.GetTagName(), AddNum);
				AttackMaterialAttrIndex = AddNum;
			}

			if (TagMap.Contains(AllRowData[i]->DefenseMaterialAtt.GetTagName()))
			{
				DefenseMaterialAttIndex = TagMap[AllRowData[i]->DefenseMaterialAtt.GetTagName()];
			}
			else
			{
				auto AddNum = TagMap.Num();
				TagMap.Add(AllRowData[i]->DefenseMaterialAtt.GetTagName(), AddNum);
				DefenseMaterialAttIndex = AddNum;
			}

			int32 ResultKey = AttackTypeIndex + AttackPowerIndex * 100 + AttackMaterialAttrIndex * 10000 +
				DefenseMaterialAttIndex * 1000000;
			RowMap.Add(ResultKey, i);
		}
	}

	const FName AttackTypeName = AttackTypeTag.GetTagName();
	const FName AttackPowerName = AttackPowerTag.GetTagName();
	const FName AttackMaterialAttrName = AttackMaterialAttrTag.GetTagName();
	const FName DefenseMaterialAttName = DefenseMaterialAttTag.GetTagName();

	if (TagMap.Contains(AttackTypeName) &&
		TagMap.Contains(AttackPowerName) &&
		TagMap.Contains(AttackMaterialAttrName) &&
		TagMap.Contains(DefenseMaterialAttName))
	{
		AttackTypeIndex = TagMap[AttackTypeName];
		AttackPowerIndex = TagMap[AttackPowerName];
		AttackMaterialAttrIndex = TagMap[AttackMaterialAttrName];
		DefenseMaterialAttIndex = TagMap[DefenseMaterialAttName];

		const int32 ResultKey = AttackTypeIndex + AttackPowerIndex * 100 + AttackMaterialAttrIndex * 10000 +
			DefenseMaterialAttIndex * 1000000;

		if (RowMap.Contains(ResultKey))
		{
			const auto RowIndex = RowMap[ResultKey];

			const auto ResultRow = DT->FindRow<FDTAttackAudioRow>(FName(FString::FromInt(RowIndex + 1)), "FindSound");
			return ResultRow->Sound.LoadSynchronous();
		}
	}

	UE_LOG(LogDLAbilityCue, Log, TEXT("FindSound Failed "));
	return nullptr;
}

FGameplayTag UDLAbilitySoundGetter_AttackMaterial::FindAttackTargetDefenseMaterial(const FGameplayCueParameters& Arg)
{
	const auto& MaterialMap = UDLAbilitySystemGlobal::Get().UnitMaterialMapPhysics;

	FGameplayTag Ret;

	if (const auto HitRet = Arg.EffectContext.GetHitResult())
	{
		if (HitRet->PhysMaterial.IsValid())
		{
			const auto Type = HitRet->PhysMaterial->SurfaceType;
			for (const auto& MaterialMapPhysics : MaterialMap)
			{
				if (MaterialMapPhysics.PhysicsSurfaceType.Contains(Type))
				{
					Ret = MaterialMapPhysics.AbilityUnitMaterial;
					break;
				}
			}
		}
	}

	if (!Ret.IsValid())
	{
		// TODO 这里可以在  actor 获取接口  
	}

	return Ret;
}

void UDLGameplayCueHitImpact::FindBastFrameFrozen(AActor* TargetApplyGC, const FGameplayCueParameters& Arg, bool& IsSuc,
	TArray<FDLAbilityFrameFrozen>& Ret) const
{
	IsSuc = false;

	for (const auto& Data : StartFrameFrozenArray)
	{
		if ((!Data.PlayCondition) || Data.PlayCondition->CanPlayCue(Arg))
		{
			if (Data.FrameFrozenTargetType == EAbilityFrameFrozenTargetType::EffectCauser)
			{
				if (Arg.IsInstigatorLocallyControlled())
				{
					IsSuc = true;
					Ret.Add(Data);
				}
			}
			else
			{
				if (const auto Pawn = Cast<APawn>(TargetApplyGC))
				{
					IsSuc = true;
					Ret.Add(Data);
				}
			}

		}
	}
}

bool UDLGameplayCueHitImpact::CheckCanExec(AActor* Target, const FGameplayCueParameters& Parameters,
										   const EDLAbilityGameplayCueTargetType ApplyTargetType)
{
	bool Ok = true;

	const auto EffectCauser = Parameters.GetEffectCauser();

	if (ApplyTargetType == EDLAbilityGameplayCueTargetType::OnlyEffectCauser)
	{
		if ((!EffectCauser) || EffectCauser->GetLocalRole() != ROLE_AutonomousProxy)
		{
			Ok = false;
		}
	}

	if (ApplyTargetType == EDLAbilityGameplayCueTargetType::OnlyTarget)
	{
		if ((!Target) || Target->GetLocalRole() != ROLE_AutonomousProxy)
		{
			Ok = false;
		}
	}

	return Ok;
}
