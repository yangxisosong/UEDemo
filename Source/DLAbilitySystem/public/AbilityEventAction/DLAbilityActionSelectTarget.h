#pragma once

#include "CoreMinimal.h"
#include "DLAbilityAction.h"
#include "UObject/Object.h"
#include "DLAbilityActionSelectTarget.generated.h"


UENUM(BlueprintType)
enum class EAbilitySelectUnitsOriginLocation : uint8
{
	// 使用技能的拥有者
	UsedSourceActor,

	// 使用锁定的角色
	UsedLockActor,

	// 使用第一个目标
	UsedFirstHitLocation,

	// 使用最后一个目标
	UsedLastHitLocation
};


UCLASS(DisplayName = SelectUnits)
class UDLAbilityActionSelectUnits
	: public UDLAbilityAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,  meta = (GetOptions = "GetUnitSelectDefIdOptions"))
		FName UnitSelectDefId;

	// 携带一些Tag信息，会被系统一直传递下去
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer PayloadTags;

	// 选择起始点
	UPROPERTY(EditAnywhere)
		EAbilitySelectUnitsOriginLocation SelectUnitsOriginLocation;

	UPROPERTY(EditAnywhere)
		FGameplayTag OnHitEventTag;

private:

	UPROPERTY()
		FDLAbilityActionContext Context;

	UPROPERTY()
		class UDLUnitSelectorInstance* TargetSelector;

	UPROPERTY()
		FGameplayAbilityTargetDataHandle TargetData;

protected:

	UFUNCTION()
		void OnHitRet(const TArray<FHitResult>& Ret);


	UFUNCTION()
		void OnExecPredict();


	UFUNCTION()
		TArray<FName> GetUnitSelectDefIdOptions() const;

protected:

	virtual bool Validation(TArray<FString>& OutErrMsg) override;

	virtual void OnExec(const FDLAbilityActionContext& InContext, UDLAbilityEventBase* Event) override;
};


