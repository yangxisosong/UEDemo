#pragma once
#include "CoreMinimal.h"
#include "IDLLaunchProjectile.generated.h"



UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UDLLaunchProjectile : public UInterface
{
	GENERATED_BODY()
};


class IDLLaunchProjectile : public IInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="SubObject")
		void LaunchToTargetUnit(AActor* TargetUnit, bool NeedTrace);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="SubObject")
		void LaunchToLocation(FVector TargetLocation);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="SubObject")
		void LaunchWithDirection(FRotator Rotation);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="SubObject")
		void Launch();
};
