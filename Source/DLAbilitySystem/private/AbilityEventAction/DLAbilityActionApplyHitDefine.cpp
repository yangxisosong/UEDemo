#include "AbilityEventAction/DLAbilityActionApplyHitDefine.h"
#include "DLGameplayAbilityBase.h"
#include "DLGameplayEffectContext.h"
#include "DLAbilityLog.h"
#include "DLKit/Public/DLTypeCast.h"
#include "TargetData/DLAbilityTargetData.h"
#include "DLAbilityTagsDef.h"
#include "Interface/ICharacterStateAccessor.h"

void UDLAbilityActionApplyHitDefine::OnExec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event)
{
	const auto Ability = Context.Ability;

	const auto HitActorInterface = GetActionFeature<IDLAbilityAFHitActor>(Event);

	if (!HitActorInterface)
	{
		return;
	}

	const auto TargetData = HitActorInterface->GetTargetData();
	if (TargetData.Num() == 0)
	{
		return;
	}

	const FDLAbilityHitDefine* OutHitDef = Ability->FindAbilityHitDefine(FGameplayTag::RequestGameplayTag(HitDefineId));
	if (!OutHitDef)
	{
		UE_LOG(LogDLAbility, Warning, TEXT("ApplyHitDefine HitDefineId %s Not Find, 检查配置"), *HitDefineId.ToString());
		return;
	}

	for (auto Data : TargetData.Data)
	{
		// 仅仅正对还活着的对象
		const AActor* Actor = Data.Get()->GetHitResult()->GetActor();
		if (Actor->Implements<UCharacterStateAccessor>())
		{
			if (ICharacterStateAccessor::Execute_K2_IsDied(Actor))
			{
				continue;
			}
		}
		else
		{
			continue;
		}

		const auto MyTargetData = StructCast<FGameplayAbilityTargetData_AttackHitResult>(Data.Get());
		if (MyTargetData)
		{
			MyTargetData->AttackTags.AppendTags(OutHitDef->CollectAttackTags());
		}
	}

	// Apply GE  主控端、权威端都得执行
	const bool MustPredict = Ability->GetAbilitySystemComponentFromActorInfo_Checked()->HasAuthorityOrPredictionKey(&Ability->CurrentActivationInfo);
	if (!MustPredict)
	{
		UE_LOG(LogDLAbility, Warning, TEXT("Exec  DLAbilityActionApplyHitDefine 无法进行 本地预测 ApplyGE"));
	}

	// 先给到 韧性的 GE
	const auto CutTenacityGEHandle = Ability->MakeOutgoingGameplayEffectSpec(Ability->CutTenacityGEClass);
	if (CutTenacityGEHandle.IsValid())
	{
		CutTenacityGEHandle.Data->SetSetByCallerMagnitude(EAbilityTagDef::ByCallerData_CutTenacityValue, OutHitDef->CutTenacityValue);
		Ability->K2_ApplyGameplayEffectSpecToTarget(CutTenacityGEHandle, TargetData);
	}

	// 如果是 Locally Controlled  需要处理攻击的表现
	if (Ability->GetActorInfo().IsLocallyControlled())
	{
		// TODO 触发者可以传 造成 Hit 的武器
		Ability->AttackActor(nullptr, TargetData, OutHitDef->CalculateAttackInfoClass.GetDefaultObject());
	}

	for (const auto& Effect : OutHitDef->OutgoingGEDefineArray)
	{
		const auto Specs = Ability->MakeOutgoingGameplayEffectSpec(Effect.GEClass, Effect.OverrideLevel);
		Ability->K2_ApplyGameplayEffectSpecToTarget(Specs, TargetData);
	}
}

TArray<FName> UDLAbilityActionApplyHitDefine::GetHitDefineIdOptions() const
{
	TArray<FName> Ret;
	if (const UDLGameplayAbilityBase* Ability = GetOuterAbility())
	{
		for (const auto& Info : Ability->GetHitDefines())
		{
			Ret.Add(*Info.DefineId.ToString());
		}
	}
	return Ret;
}
