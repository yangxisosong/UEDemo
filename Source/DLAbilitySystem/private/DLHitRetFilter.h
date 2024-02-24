#pragma once
#include "DLAbilitySystemGlobal.h"
#include "DLGameplayTagLibrary.h"
#include "DLUnitSelectorFitler.h"
#include "GameplayTagAssetInterface.h"
#include "Interface/IDLCharacterParts.h"
#include "DLAbilityLog.h"
#include "Interface/ICharacterStateAccessor.h"
#include "DLHitRetFilter.generated.h"


namespace DLUnitSelectorFilterPrivate
{
	struct FHitUnitInfo
	{
		TWeakObjectPtr<AActor> OwnerActor;
		int32 PartIndex = INDEX_NONE;
		int32 HitCount = 0;
	};


	struct FRuntime
	{
		int32 CurrentHitCount = 0;
		TArray<FHitUnitInfo> HitUnitInfos;

		FHitUnitInfo& FindOrAdd(const TWeakObjectPtr<AActor> Owner, const int32 PartIndex = INDEX_NONE)
		{
			for (auto& Info : HitUnitInfos)
			{
				if (Info.OwnerActor == Owner && Info.PartIndex == PartIndex)
				{
					return Info;
				}
			}

			auto& Ret = HitUnitInfos.AddZeroed_GetRef();
			Ret.PartIndex = PartIndex;
			Ret.OwnerActor = Owner;
			return Ret;
		}
	};
}


USTRUCT(BlueprintType)
struct FDLUnitSelectorFilterDefaultData
{
	GENERATED_BODY()
public:

	/**
	* @brief 击中的单位数量
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int32 MaxUnitCount = 1;

	/**
	* @brief 是否把 同一个Actor 的单个Part 视为一个 Unit
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool PartAsUnit = false;


	/**
	* @brief 每个单位最大的被碰撞数
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		uint8 PerUnitHitMaxCount = 1;


	/**
	 * @brief 是否忽略教唆者
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bIgnoreInstigator = true;

	/**
	* @brief 需要满足的 Tag ，使用 IGameplayTagAssetInterface 获取
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
		FDLGameplayTagRequirements RequirementTags;
};


UCLASS()
class UDLUnitSelectorFilterDefault : public UDLUnitSelectorFilterBase
{
	GENERATED_BODY()

public:

	UDLUnitSelectorFilterDefault()
	{
		FilterData.RequirementTags = GetMutableDefault<UDLAbilitySystemGlobal>()->DefaultHitUnitRequirementTags;
	}

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FDLUnitSelectorFilterDefaultData FilterData;

private:

	DLUnitSelectorFilterPrivate::FRuntime Runtime;

protected:

	virtual UDLUnitSelectorFilterBase* CloneInstance(UObject* Outer) override
	{
		const auto NewIns = NewObject<UDLUnitSelectorFilterDefault>(Outer);
		NewIns->FilterData = FilterData;
		return NewIns;
	}

	virtual void Reset_Implementation() override
	{
		Runtime = {};
	}

	virtual bool FilterHitUnit_Implementation(const FUnitSelectorFilterContext& Context, const FHitResult& Result) override
	{
		// 检查 数量
		if (Runtime.CurrentHitCount >= FilterData.MaxUnitCount)
		{
			return true;
		}

		if (!Result.GetActor())
		{
			return true;
		}

		AActor* HitActor = Result.GetActor();

		if (FilterData.bIgnoreInstigator)
		{
			if (HitActor == Context.Instigator)
			{
				return true;
			}
		}

		// 是不是死了，死了就不管了
		// TODO 这里其实是要对接单位的 Tag，来判定是否是 可受伤的单位，然后进一步判断
		if (HitActor->Implements<UCharacterStateAccessor>())
		{
			if (ICharacterStateAccessor::Execute_K2_IsDied(HitActor))
			{
				return true;
			}
		}


		// 如果分部位了，按分部位的策略进行
		bool IsProcess = false;

		if (FilterData.PartAsUnit)
		{
			const IDLCharacterParts* CharacterParts = Cast<IDLCharacterParts>(HitActor);
			if (CharacterParts && CharacterParts->IsSplitParts())
			{
				auto& Data = Runtime.FindOrAdd(HitActor, Result.Item);
				if (Data.HitCount >= FilterData.PerUnitHitMaxCount)
				{
					// 超出了 每个 Unit 的最大 Hit 数量，直接过滤
					return true;
				}
				else
				{
					Data.HitCount++;
				}

				IsProcess = true;
			}
		}


		// 剩下的情况按 Actor 处理
		if (!IsProcess)
		{
			auto& Data = Runtime.FindOrAdd(HitActor);
			if (Data.HitCount >= FilterData.PerUnitHitMaxCount)
			{
				// 超出了 每个 Unit 的最大 Hit 数量，直接过滤
				return true;
			}
			else
			{
				Data.HitCount++;
			}

			IsProcess = true;
		}

		Runtime.CurrentHitCount++;


		// 检查 Tag 的阶段
		bool TagCheckPass = false;

		FGameplayTagContainer TargetCurrentStateTags;

		if (Result.GetActor())
		{
			const auto TagInterface = Cast<IGameplayTagAssetInterface>(Result.GetActor());
			if (TagInterface)
			{
				TagInterface->GetOwnedGameplayTags(TargetCurrentStateTags);
				if (FilterData.RequirementTags.IsEmpty() || FilterData.RequirementTags.RequirementsMet(TargetCurrentStateTags))
				{
					TagCheckPass = true;
				}
			}
			else
			{
				if (FilterData.RequirementTags.IsEmpty())
				{
					TagCheckPass = true;
				}
			}
		}

		if (!TagCheckPass)
		{
			if (Result.GetActor())
			{
				UE_LOG(LogDLAbilityTargetSelector, Log, 
										TEXT("UDLUnitSelectorFilterDefault %s 无视 Hit，因为不满足作为目标的Tag要求 目标的状态 %s 要求的状态 %s"),
										*GetNameSafe(Result.GetActor()),
										*TargetCurrentStateTags.ToStringSimple(),
										*FilterData.RequirementTags.ToString()
					);
			}

			return true;
		}


		return false;
	}
};
