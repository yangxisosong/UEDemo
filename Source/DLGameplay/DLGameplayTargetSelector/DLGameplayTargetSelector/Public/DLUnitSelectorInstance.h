// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLAbilityTargetSelector.h"
#include "DLGameplayTagLibrary.h"
#include "UObject/Object.h"
#include "DLUnitSelectorInstance.generated.h"


UENUM(BlueprintType)
enum class EUnitSelectorDetectionType : uint8
{
	OneFrame,
	Duration,
	Custom,
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectUnit, const TArray<FHitResult>&, HitRets);




USTRUCT(BlueprintType)
struct FDLUnitSelectorInstanceContext
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
		AActor* Instigator = nullptr;
};

USTRUCT()
struct FUnitSelectorInstanceRuntime
{
	GENERATED_BODY()
public:
	bool bIsSelecting = false;
	TWeakObjectPtr<const AActor> RelativeObjActor;
	TOptional<FTransform> RelativeTransform;
	TWeakObjectPtr<UObject> WorldContext;
	float CumulativeTime = 0.f;
	float LastProcessTime = 0.f;
	FGameplayTargetSelectorExtendData ExtendData;
public:
	UPROPERTY()
		FDLUnitSelectorInstanceContext InstanceContext;
};


UCLASS(CollapseCategories, EditInlineNew, BlueprintType)
class DLGAMEPLAYTARGETSELECTOR_API UDLUnitSelectorInstance : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bAttackAnchorLocation = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bAttackAnchorRotation = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = HitDetection)
		EUnitSelectorDetectionType HitDetectionType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HitDetection, meta = (EditCondition = "HitDetectionType==EUnitSelectorDetectionType::Duration"))
		float Duration = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HitDetection, meta = (EditCondition = "HitDetectionType==EUnitSelectorDetectionType::Duration"))
		bool bIsPeriod = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HitDetection, meta = (EditCondition = "HitDetectionType==EUnitSelectorDetectionType::Duration&&bIsPeriod", EditConditionHides))
		float Period = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class UDLGameplayTargetSelectorShapeTraceBase* UnitSelector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Instanced)
		class UDLUnitSelectorFilterBase* Filter;

public:

	UPROPERTY(BlueprintAssignable, Category = Instance, Transient)
		FOnSelectUnit OnSelectUnit;


private:

	UPROPERTY()
		FUnitSelectorInstanceRuntime Runtime;


public:

	UDLUnitSelectorInstance* Clone(UObject* Outer) const;

protected:

	virtual void Tick(const float Dt) override;
	virtual UWorld* GetTickableGameObjectWorld() const override;

	virtual TStatId GetStatId() const override;;

	virtual bool IsAllowedToTick() const override;
public:


	void ProcessUnitSelector() const;

	void StartSelectUnit(const FDLUnitSelectorInstanceContext& Context, const AActor* InRelativeObjActor);
	void StartSelectUnit(UObject* WorldContext, const FDLUnitSelectorInstanceContext& Context, const FTransform& Transform);

	void EndSelectUnit();

public:

	UFUNCTION(BlueprintCallable, DisplayName = StartSelectUnitRelativeActor)
		void K2_StartSelectUnitRelativeActor(const FDLUnitSelectorInstanceContext& Context, const AActor* InRelativeObjActor);

	UFUNCTION(BlueprintCallable, DisplayName = StartSelectUnitRelativeTransform)
		void K2_StartSelectUnitRelativeTransform(UObject* WorldContext, const FDLUnitSelectorInstanceContext& Context, const FTransform& Transform);

	UFUNCTION(BlueprintCallable, DisplayName = EndSelectUnit)
		void K2_EndSelectUnit();
};
