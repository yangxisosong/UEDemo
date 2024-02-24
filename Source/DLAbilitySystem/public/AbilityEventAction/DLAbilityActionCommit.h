#pragma once
#include "ConvertString.h"
#include "DLAbilityAction.h"
#include "DLGameplayAbilityBase.h"
#include "DLAbilityLog.h"
#include "DLAbilityActionCommit.generated.h"



// 提交技能，扣除消耗和CD
UCLASS(DisplayName = CommitAbility)
class UDLAbilityActionCommit
	: public UDLAbilityAction
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
		bool bEnabledCost = true;

	UPROPERTY(EditAnywhere)
		bool bEnabledCD = true;

protected:

	virtual void OnExec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event) override
	{
		if (bEnabledCD && bEnabledCost)
		{
			const bool CommitOk = Context.Ability->K2_CommitAbility();

			UE_LOG(LogDLAbility, Log, TEXT("[%s] CommitAbility %s  %s"),
				NET_ROLE_STR(Context.Ability->GetActorInfo().PlayerController.Get()),
				TO_STR(CommitOk),
				TO_STR(Context.Ability)
			);

			if (!CommitOk)
			{
				Context.Ability->K2_CancelAbility();
			}
		}
		else
		{
			if (bEnabledCost)
			{
				const bool CostOk = Context.Ability->K2_CommitAbilityCost();
				UE_LOG(LogDLAbility, Log, TEXT("[%s] CommitAbilityCost %s  %s"),
					NET_ROLE_STR(Context.Ability->GetActorInfo().PlayerController.Get()),
					TO_STR(CostOk),
					TO_STR(Context.Ability)
				);

				if (!CostOk)
				{
					Context.Ability->K2_CancelAbility();
				}
			}
			else if(bEnabledCD)
			{
				const bool CostCD = Context.Ability->K2_CommitAbilityCooldown();
				UE_LOG(LogDLAbility, Log, TEXT("[%s] CommitAbilityCooldown %s  %s"),
					NET_ROLE_STR(Context.Ability->GetActorInfo().PlayerController.Get()),
					TO_STR(CostCD),
					TO_STR(Context.Ability)
				);

				if (!CostCD)
				{
					Context.Ability->K2_CancelAbility();
				}
			}
		}
	}
};
