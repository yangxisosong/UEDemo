#pragma once

#include "CoreMinimal.h"
#include "DLUnitSelectorFitler.generated.h"


USTRUCT(BlueprintType)
struct FUnitSelectorFilterContext
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
		AActor* Instigator = nullptr;
};

UCLASS(Abstract, EditInlineNew, CollapseCategories)
class DLGAMEPLAYTARGETSELECTOR_API UDLUnitSelectorFilterBase : public UObject
{
	GENERATED_BODY()
public:

	/**
	 * @brief 是否过滤掉 这个 HitRet
	 * @param Context
	 * @param Result
	 * @return  True 过滤掉，False 不过滤
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool FilterHitUnit(const FUnitSelectorFilterContext& Context, const FHitResult& Result);


	/**
	 * @brief 重置
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void Reset();


	UFUNCTION(BlueprintCallable)
		virtual UDLUnitSelectorFilterBase* CloneInstance(UObject* Outer) { return nullptr; }

protected:

	virtual bool FilterHitUnit_Implementation(const FUnitSelectorFilterContext& Context, const FHitResult& Result)
	{
		return false;
	}


	virtual void Reset_Implementation()
	{

	}
};

