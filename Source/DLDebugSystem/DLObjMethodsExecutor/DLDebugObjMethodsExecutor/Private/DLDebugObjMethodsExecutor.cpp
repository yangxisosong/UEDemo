#include "DLDebugObjMethodsExecutor.h"

// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppMemberFunctionMayBeConst

FOnDebugInstanceEvent FDLDebugObjExecutorDelegates::OnDebugRemoveInstanceWorld;
FOnDebugInstanceEvent FDLDebugObjExecutorDelegates::OnDebugNewInstanceWorld;


void UDLDebugObjExecutorSubsystem::GetObjectListByFilter(const FDLDebugObjectFilter Filter,
	TArray<TWeakObjectPtr<UObject>>& OutObjectList)
{
	if (!GetWorld())
	{
		return;
	}

	if (!Filter.ClassFilter)
	{
		return;
	}

	if (Filter.ClassFilter->IsChildOf(AActor::StaticClass()))
	{
		for (TActorIterator<AActor> It(GetWorld(), Filter.ClassFilter.Get()); It; ++It)
		{
			AActor* Actor = *It;
			if (IsValid(Actor))
			{
				if (Actor->GetLocalRole() == Filter.NetRole)
				{
					OutObjectList.Add(Actor);
				}
			}
		}
	}
	else
	{
		// TODO 可能需要 收到收集的方式
		//for (TObjectIterator<UObject> Iterator(Filter.ClassFilter.Get()); Iterator; ++Iterator)
		//{
		//	
		//}

	}
}

void UDLDebugObjExecutorSubsystem::GetFunctionListByFilter(const TWeakObjectPtr<UObject> Target,
                                                           FDLDebugFunctionListFilter Filter, TArray<UFunction*>& FuncList)
{
	if (Target.IsValid())
	{
		const auto Flags = Filter.GetFlags();
		for (TFieldIterator<UFunction> It(Target->GetClass()); ;++It)
		{
			UFunction* Func = *It;
			if (Func && Func->FunctionFlags & (Flags))
			{
				FuncList.Add(Func);
			}
		}
	}
}
