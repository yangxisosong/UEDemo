#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DLPlayerAbilityDataTable.generated.h"


USTRUCT(BlueprintType)
struct FDLPlayerAbilityConfig : public FTableRowBase
{
	GENERATED_BODY()

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		AbilityId = InRowName;
	}
public:
	//技能id
	UPROPERTY(EditAnywhere)
		FName AbilityId;

	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UTexture>  AbilityIcon;

	UPROPERTY(EditAnywhere)
		FText AbilityKey;
};