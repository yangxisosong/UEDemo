#include "AbilityEventAction/DLAbilityActionSelectTarget.h"

#include "DLGameplayAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "ASC/DLUnitAbilitySystemComponent.h"
#include "DLAbilityTargetSelector.h"
#include "DLUnitSelectorInstance.h"
#include "TargetData/DLAbilityTargetData.h"
#include "DLAbilityLog.h"
#include "DLGameplayTagDef.h"
#include "AbilityEventAction/DLAbilityEventBase.h"


void UDLAbilityActionSelectUnits::OnHitRet(const TArray<FHitResult>& Rets)
{
	for (const auto& Ret : Rets)
	{
		UE_LOG(LogDLAbilityActionEvent, Log, TEXT("UDLAbilityActionSelectUnits::OnHitRet %s"), *Ret.ToString());
	}

	FGameplayAbilityTargetDataHandle Ret;

	for (const auto& HitRet : Rets)
	{
		if (HitRet.GetActor())
		{
			FGameplayAbilityTargetData_AttackHitResult* NewData = new FGameplayAbilityTargetData_AttackHitResult;
			NewData->HitResult = HitRet;
			NewData->AttackTags.AppendTags(PayloadTags);
			Ret.Add(NewData);
		}
	}

	if (Ret.Num() > 0)
	{
		TargetData = Ret;
		FAbilitySimpleDynamicDelegate Delegate;
		Delegate.BindDynamic(this, &UDLAbilityActionSelectUnits::OnExecPredict);
		Context.Ability->SendTargetDataToServer(Ret, OnHitEventTag, Delegate);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UDLAbilityActionSelectUnits::OnExecPredict()
{
	Context.Ability->TriggerEventHitUnit(OnHitEventTag, TargetData);
}

void UDLAbilityActionSelectUnits::OnExec(const FDLAbilityActionContext& InContext, UDLAbilityEventBase* Event)
{

	if (!InContext.Ability->GetActorInfo().IsLocallyControlled())
	{
		return;
	}

	const FDLAbilityUnitSelectDefine* Def = InContext.Ability->FindUnitSelectDefine(FGameplayTag::RequestGameplayTag(this->UnitSelectDefId));

	if (!ensureAlwaysMsgf(Def->SelectorInsPrototype, TEXT("DLAbilityActionSelectUnits 没有设置 TargetSelector")))
	{
		return;
	}

	TargetSelector = Def->SelectorInsPrototype->Clone(InContext.Ability);
	TargetSelector->OnSelectUnit.AddDynamic(this, &UDLAbilityActionSelectUnits::OnHitRet);

	Context = InContext;

	const AActor* SrcActor = InContext.Ability->GetActorInfo().AvatarActor.Get();
	if (SelectUnitsOriginLocation == EAbilitySelectUnitsOriginLocation::UsedLockActor)
	{
		if (const auto Comp = Cast<UDLUnitAbilitySystemComponent>(InContext.Ability->GetAbilitySystemComponentFromActorInfo_Ensured()))
		{
			SrcActor = Comp->GetLockCharacterActor();
		}
	}

	FDLUnitSelectorInstanceContext InstanceContext;
	InstanceContext.Instigator = InContext.Ability->GetAvatarActorFromActorInfo();

	if (SelectUnitsOriginLocation == EAbilitySelectUnitsOriginLocation::UsedSourceActor || SelectUnitsOriginLocation == EAbilitySelectUnitsOriginLocation::UsedLockActor)
	{
		TargetSelector->StartSelectUnit(InstanceContext, SrcActor);
	}
	else
	{
		const IDLAbilityAFHitResults* HitResultsInterface = GetActionFeature<IDLAbilityAFHitResults>(Event);
		if (!HitResultsInterface)
		{
			return;
		}

		const auto& HitResults = HitResultsInterface->GetHitResults();
		if (HitResults.Num() > 0)
		{
			FTransform SrcTransform = FTransform::Identity;

			if (SelectUnitsOriginLocation == EAbilitySelectUnitsOriginLocation::UsedFirstHitLocation)
			{
				SrcTransform.SetLocation(HitResults[0].Location);
			}
			else if (SelectUnitsOriginLocation == EAbilitySelectUnitsOriginLocation::UsedFirstHitLocation)
			{
				SrcTransform.SetLocation(HitResults.Last().Location);
			}

			if (!SrcTransform.GetLocation().IsNearlyZero())
			{
				TargetSelector->StartSelectUnit(const_cast<AActor*>(SrcActor), InstanceContext, SrcTransform);
			}

		}
	}
}

TArray<FName> UDLAbilityActionSelectUnits::GetUnitSelectDefIdOptions() const
{
	TArray<FName> Ret;
	if (const UDLGameplayAbilityBase* Ability = GetOuterAbility())
	{
		for (const auto& Info : Ability->GetUnitSelectDefines())
		{
			if (Info.UnitSelectType == EDLAbilityUnitSelectType::UnitSelectorInstance)
			{
				Ret.Add(*Info.DefineId.ToString());
			}
		}
	}
	return Ret;
}

bool UDLAbilityActionSelectUnits::Validation(TArray<FString>& OutErrMsg)
{
	bool Ret = true;

	if (UnitSelectDefId.IsNone())
	{
		Ret = false;
		OutErrMsg.Add((TEXT("UnitSelectDefId Is None")));
	}

	if (!OnHitEventTag.IsValid())
	{
		Ret = false;
		OutErrMsg.Add((TEXT("OnHitEventTag Is Invalid")));
	}

	if (GetOuterAbility())
	{
		FGameplayTagContainer AbilityHitEventTags;
		const auto& HitEventArray = GetOuterAbility()->FindAbilityEvent(UDLAbilityEventWeaponHitActor::StaticClass());
		for (const auto Event : HitEventArray)
		{
			AbilityHitEventTags.AppendTags(Cast<UDLAbilityEventWeaponHitActor>(Event)->HitEventTags);
		}

		const bool Ok = AbilityHitEventTags.HasTag(OnHitEventTag);
		if (!Ok)
		{
			Ret = false;
			OutErrMsg.Add((FString::Printf(TEXT("OnHitEventTag %s Not Match HitUnitEvent->HitEventTags"), *OnHitEventTag.ToString())));
		}
	}

	return Ret;
}
