#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "DLSceneTriggerBase.generated.h"


UCLASS()
class ADLSceneTriggerBase
		: public AActor
{
	GENERATED_BODY()
public:

	explicit ADLSceneTriggerBase(const FObjectInitializer& Initializer)
		:Super(Initializer)
	{
		BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigger"));
	}

public:

	UPROPERTY(EditInstanceOnly)
		UBoxComponent* BoxTrigger;


public:

};

