#include "AbilityEventAction/DLAbilityAction.h"

#include "DLKit/Public/ConvertString.h"
#include "DLGameplayAbilityBase.h"
#include "DLAbilityLog.h"

UDLAbilityAction::UDLAbilityAction()
{
	const auto ImplementedInBlueprint = [](const UFunction* Func) -> bool
	{
		return Func && ensure(Func->GetOuter())
			&& (Func->GetOuter()->IsA(UBlueprintGeneratedClass::StaticClass()) || Func->GetOuter()->IsA(UDynamicClass::StaticClass()));
	};

	static FName FuncName = FName(TEXT("K2_OnExec"));
	const UFunction* ShouldRespondFunction = GetClass()->FindFunctionByName(FuncName);
	bCallBPExec = ImplementedInBlueprint(ShouldRespondFunction);
}

void UDLAbilityAction::Exec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event)
{
	if (bCallBPExec)
	{
		this->K2_OnExec(Context, Event);
	}
	else
	{
		this->OnExec(Context, Event);
	}
}

const UDLGameplayAbilityBase* UDLAbilityAction::GetOuterAbility() const
{
	UObject* Outer = GetOuter();
	while (Outer)
	{
		if (Cast<UDLGameplayAbilityBase>(Outer))
		{
			break;
		}

		Outer = Outer->GetOuter();
	}

	return Cast<UDLGameplayAbilityBase>(Outer);
}
