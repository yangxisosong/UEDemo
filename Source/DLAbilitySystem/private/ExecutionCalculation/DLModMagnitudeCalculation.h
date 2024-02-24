#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DLModMagnitudeCalculation.generated.h"


USTRUCT(BlueprintType)
struct FDLMagnitudeCalculationContext
{
	GENERATED_BODY()
public:

	FGameplayEffectSpec GESpec;
	FAggregatorEvaluateParameters EvaluateParameters;
};



UCLASS(Meta = (KismetHideOverrides = CalculateBaseMagnitude, hideCategories = Attributes))
class UDLModMagnitudeCalculation
	: public UGameplayModMagnitudeCalculation
{
private:
	GENERATED_BODY()

public:

	UDLModMagnitudeCalculation();

public:

	UPROPERTY(EditDefaultsOnly, Category = CalculaMangnitude)
		FGameplayTagContainer SourceTagFilter;

	UPROPERTY(EditDefaultsOnly, Category = CalculaMangnitude)
		FGameplayTagContainer TargetTagFilter;

	UPROPERTY(EditDefaultsOnly, Category= CalculaMangnitude)
		TMap<FName, FGameplayEffectAttributeCaptureDefinition> RelevantAttributesToCaptureMap;

protected:

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:

	UFUNCTION(BlueprintImplementableEvent)
		float CalculateMagnitude(const FDLMagnitudeCalculationContext& Context) const;


	UFUNCTION(BlueprintCallable, Category = CalculaMangnitude)
		float GetCapturedAttribute(const FDLMagnitudeCalculationContext& Context, FName DefName) const;


	UFUNCTION(BlueprintCallable, Category = CalculaMangnitude)
		float GetSetByCallerMagnitude(const FDLMagnitudeCalculationContext& Context, FGameplayTag Tag, bool WarnIfNotFound = true, float DefaultIfNotFound = 0.f) const;
};
