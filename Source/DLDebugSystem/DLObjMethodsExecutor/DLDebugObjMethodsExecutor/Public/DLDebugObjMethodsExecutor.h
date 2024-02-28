#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "DLDebugObjMethodsExecutor.generated.h"


USTRUCT(BlueprintType)
struct FDLDebugObjectFilter
{
	GENERATED_BODY()
public:

	UPROPERTY()
		TSubclassOf<UObject> ClassFilter;

	UPROPERTY()
		TEnumAsByte<ENetRole> NetRole;
};


USTRUCT()
struct FDLDebugFunctionListFilter
{
	GENERATED_BODY()
public:

	TArray<EFunctionFlags> Flags;

	EFunctionFlags GetFlags()
	{
		EFunctionFlags Ret = EFunctionFlags::FUNC_None;
		for (const auto Flag : Flags)
		{
			Ret |= Flag;
		}
		return Ret;
	}

};


UCLASS()
class DLDEBUGOBJMETHODSEXECUTOR_API UDLDebugObjExecutorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	void GetObjectListByFilter(const FDLDebugObjectFilter Filter, TArray<TWeakObjectPtr<UObject>>& OutObjectList);

	void GetFunctionListByFilter(const TWeakObjectPtr<UObject> Target, FDLDebugFunctionListFilter Filter, TArray<UFunction*>& FuncList);


protected:

	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override
	{
		return WorldType == EWorldType::PIE;
	}

};


DECLARE_DELEGATE_OneParam(FOnDebugInstanceEvent, int32 /*PIEInstanceIndex*/);


struct DLDEBUGOBJMETHODSEXECUTOR_API FDLDebugObjExecutorDelegates
{
	// 创建一个 PIE 实例
	static FOnDebugInstanceEvent OnDebugNewInstanceWorld;

	// PIE 实例销毁
	static FOnDebugInstanceEvent OnDebugRemoveInstanceWorld;
};


