#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DamageExecCalc.generated.h"


UCLASS()
class DLABILITYSYSTEM_API UDamageExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UDamageExecCalc();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};


