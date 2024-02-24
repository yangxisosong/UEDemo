#include "DLUnitSelectorInstance.h"
#include "DLUnitSelectorFitler.h"

UDLUnitSelectorInstance* UDLUnitSelectorInstance::Clone(UObject* Outer) const
{
	UDLUnitSelectorInstance* Ins = NewObject<UDLUnitSelectorInstance>(Outer);
	Ins->bAttackAnchorLocation = this->bAttackAnchorLocation;
	Ins->bAttackAnchorRotation = this->bAttackAnchorRotation;
	Ins->HitDetectionType = this->HitDetectionType;
	Ins->Duration = this->Duration;
	Ins->bIsPeriod = false;
	Ins->Period = this->Period;
	Ins->UnitSelector = this->UnitSelector;
	Ins->Filter = this->Filter;
	return Ins;
}

void UDLUnitSelectorInstance::Tick(const float Dt)
{
	if (!Runtime.bIsSelecting)
	{
		return;
	}

	bool Process = true;

	if (HitDetectionType == EUnitSelectorDetectionType::Duration)
	{
		if (!FMath::IsNearlyZero(Duration))
		{
			Runtime.CumulativeTime += Dt;
			if (Duration < Runtime.CumulativeTime)
			{
				this->EndSelectUnit();
				return;
			}
		}

		if (bIsPeriod)
		{
			if ((Runtime.LastProcessTime + Period) > Runtime.CumulativeTime)
			{
				Process = false;
			}
			else
			{
				Runtime.LastProcessTime = Runtime.CumulativeTime;
			}
		}
	}
	else if (HitDetectionType == EUnitSelectorDetectionType::Custom)
	{
		// TODO  Custom 的情况
	}

	if (Process)
	{
		this->ProcessUnitSelector();
	}

}

UWorld* UDLUnitSelectorInstance::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

TStatId UDLUnitSelectorInstance::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(DLUnitSelectorInstance, STATGROUP_Tickables);
}

bool UDLUnitSelectorInstance::IsAllowedToTick() const
{
	return (!IsPendingKill()) && Runtime.bIsSelecting;
}

void UDLUnitSelectorInstance::ProcessUnitSelector() const
{
	if (!Runtime.RelativeObjActor.IsValid())
	{
		return;
	}


	bool Suc = false;
	TArray<FHitResult> Rets;

	if (Runtime.RelativeTransform.IsSet() && Runtime.WorldContext.IsValid())
	{
		UnitSelector->CalculateTargetUnitsWithTransform(Runtime.WorldContext.Get(),
								Runtime.RelativeTransform.GetValue(),
								TEXT("UDLUnitSelectorInstance::StartSelectUnit"),
								Suc,
								Rets);
	}
	else if (Runtime.RelativeObjActor.IsValid())
	{
		UnitSelector->CalculateTargetUnits(Runtime.RelativeObjActor.Get(), TEXT("UDLUnitSelectorInstance::StartSelectUnit"), Suc, Rets, Runtime.ExtendData);
	}


	if (Suc)
	{
		if (Filter)
		{
			static TArray<FHitResult> Temp;
			Temp.Empty();

			FUnitSelectorFilterContext Context;
			Context.Instigator = Runtime.InstanceContext.Instigator;

			for (const FHitResult& Ret : Rets)
			{
				if (!Filter->FilterHitUnit(Context, Ret))
				{
					Temp.Add(Ret);
				}
			}

			if (Temp.Num() > 0)
			{
				OnSelectUnit.Broadcast(Temp);
			}

		}
		else
		{
			OnSelectUnit.Broadcast(Rets);
		}
	}

}

void UDLUnitSelectorInstance::StartSelectUnit(const FDLUnitSelectorInstanceContext& Context, const AActor* InRelativeObjActor)
{
	if (!ensureAlwaysMsgf(UnitSelector, TEXT("StartSelectUnit  UnitSelector 一定存在")))
	{
		return;
	}


	if (!ensureAlwaysMsgf(InRelativeObjActor, TEXT("StartSelectUnit  RelativeObjActor 不为空")))
	{
		return;
	}

	Runtime = {};
	Runtime.RelativeObjActor = InRelativeObjActor;
	Runtime.InstanceContext = Context;

	if (HitDetectionType == EUnitSelectorDetectionType::Duration && (!bAttackAnchorLocation || !bAttackAnchorRotation))
	{
		FGameplayTargetSelectorExtendData ExtendData;

		FTransform Out;
		const bool Ok = UnitSelector->CalculateAnchorTransform(InRelativeObjActor, TEXT("UDLUnitSelectorInstance::StartSelectUnit"), Out);
		if (!bAttackAnchorLocation)
		{
			ExtendData.bOverrideLocation = true;
			ExtendData.OverrideLocation = Out.GetLocation();
		}

		if (!bAttackAnchorRotation)
		{
			ExtendData.bOverrideRotation = true;
			ExtendData.OverrideRotation = Out.GetRotation().Rotator();
		}

		Runtime.ExtendData = ExtendData;
	}

	this->ProcessUnitSelector();

	if (HitDetectionType != EUnitSelectorDetectionType::OneFrame)
	{
		Runtime.bIsSelecting = true;
	}
	else
	{
		// 一帧的策略 不需要一直 Check 
		this->EndSelectUnit();
	}
}

void UDLUnitSelectorInstance::StartSelectUnit(UObject* WorldContext, const FDLUnitSelectorInstanceContext& Context, const FTransform& Transform)
{
	if (!ensureAlwaysMsgf(UnitSelector, TEXT("StartSelectUnit  UnitSelector 一定存在")))
	{
		return;
	}

	Runtime = {};
	Runtime.RelativeTransform = Transform;
	Runtime.WorldContext = WorldContext;
	Runtime.InstanceContext = Context;

	this->ProcessUnitSelector();

	if (HitDetectionType != EUnitSelectorDetectionType::OneFrame)
	{
		Runtime.bIsSelecting = true;
	}
	else
	{
		// 一帧的策略 不需要一直 Check 
		this->EndSelectUnit();
	}
}

void UDLUnitSelectorInstance::EndSelectUnit()
{
	Runtime = {};
	if (Filter)
	{
		Filter->Reset();
	}
}

void UDLUnitSelectorInstance::K2_StartSelectUnitRelativeActor(const FDLUnitSelectorInstanceContext& Context, const AActor* InRelativeObjActor)
{
	return StartSelectUnit(Context, InRelativeObjActor);
}

void UDLUnitSelectorInstance::K2_StartSelectUnitRelativeTransform(UObject* WorldContext, const FDLUnitSelectorInstanceContext& Context, const FTransform& Transform)
{
	return StartSelectUnit(WorldContext, Context, Transform);
}

void UDLUnitSelectorInstance::K2_EndSelectUnit()
{
	return EndSelectUnit();
}
