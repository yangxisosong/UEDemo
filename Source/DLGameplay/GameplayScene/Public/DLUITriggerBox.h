#pragma once

#include "CoreMinimal.h"
#include "DLUITriggerEventDef.h"
#include "GameplayTagContainer.h"
#include "InputMappingContext.h"
#include "Engine/TriggerBox.h"
#include "DLUITriggerBox.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
UCLASS()
class GAMEPLAYSCENE_API ADLUITriggerBox :public ATriggerBox
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
		FGameplayTag TriggerShowUITag;

	UPROPERTY(EditAnywhere)
		FGameplayTag TriggerHideUITag;

	UPROPERTY(EditAnywhere)
		FGameplayTag ButtonTrigger;

	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(EditAnywhere)
		FDLUIShowButtonTipsMsgBody ButtonTipsMsgBody;

	UPROPERTY(EditAnywhere)
		int32 InputContextPriority;

	UPROPERTY(BlueprintReadOnly)
		bool IsInTriggerBox = false;
public:
	ADLUITriggerBox();
	// declare overlap begin function
	UFUNCTION()
		void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	// declare overlap end function
	UFUNCTION()
		void OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
		void OnTriggerButton(FKey Key);
};