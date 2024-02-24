#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "DLGameplayCoreLib.generated.h"


UCLASS()
class DLGAMEPLAYCORE_API UDLGameplayCoreLib
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = UnitTags, meta=(AdvancedDisplay = DebugEnsure))
		static bool CheckUnitCampRelationship(AActor* UnitA, AActor* UnitB, FGameplayTag& OutRelationship, bool DebugEnsure = false);


	UFUNCTION(BlueprintCallable, Category= UintTags, meta=(AdvancedDisplay = DebugEnsure))
		static FGameplayTag GetUnitRoleTag(AActor* Unit, bool DebugEnsure = false);
};
