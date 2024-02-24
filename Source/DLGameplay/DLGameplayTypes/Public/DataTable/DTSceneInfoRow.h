#pragma once
#include "CoreMinimal.h"
#include "DataRegistryId.h"
#include "Engine/DataTable.h"
#include "DTSceneInfoRow.generated.h"


USTRUCT(BlueprintType)
struct FDTSceneInfoRow
	: public FTableRowBase
{
	GENERATED_BODY()
public:

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		SceneId = InRowName;
	}

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FName SceneId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FText WelcomeSceneText;

};


namespace DRSceneInfo
{
	static FDataRegistryType DRTypeForSceneInfo(TEXT("SceneInfo"));

	inline FDataRegistryId CreateSceneInfoDataRegistryId(const FName& Id)
	{
		FDataRegistryId ID;
		ID.RegistryType = DRTypeForSceneInfo;
		ID.ItemName = Id;
		return ID;
	}
}