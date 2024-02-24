#include "DLModMagnitudeCalculation.h"

UDLModMagnitudeCalculation::UDLModMagnitudeCalculation()
{
}

float UDLModMagnitudeCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	EvaluateParameters.AppliedSourceTagFilter = SourceTagFilter;
	EvaluateParameters.AppliedTargetTagFilter = TargetTagFilter;

	const FDLMagnitudeCalculationContext Context{Spec, EvaluateParameters};
	return this->CalculateMagnitude(Context);
}
#if WITH_EDITOR

void UDLModMagnitudeCalculation::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	RelevantAttributesToCaptureMap.GenerateValueArray(RelevantAttributesToCapture);
}

#endif

float UDLModMagnitudeCalculation::GetCapturedAttribute(const FDLMagnitudeCalculationContext& Context,
	FName DefName) const
{
	if (RelevantAttributesToCaptureMap.Contains(DefName))
	{
		float Value = 0.f;
		if (this->GetCapturedAttributeMagnitude(RelevantAttributesToCaptureMap[DefName], Context.GESpec, Context.EvaluateParameters, Value))
		{
			return Value;
		}
	}

	return 0.f;
}

float UDLModMagnitudeCalculation::GetSetByCallerMagnitude(const FDLMagnitudeCalculationContext& Context,
                                                          const FGameplayTag Tag, const bool WarnIfNotFound, const float DefaultIfNotFound) const
{
	return Context.GESpec.GetSetByCallerMagnitude(Tag, WarnIfNotFound, DefaultIfNotFound);
}
