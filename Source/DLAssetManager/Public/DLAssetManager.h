#pragma once

#include "Engine/AssetManager.h"
#include "DLAssetManager.generated.h"


/**
 * This is used by setting AssetManagerClassName in DefaultEngine.ini
 */
UCLASS()
class DLASSETMANAGER_API UDLAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	static UDLAssetManager& Get();
};


UCLASS()
class UDLAssetLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	static UClass* FetchSoftClassPtr(TSoftClassPtr<UObject> Ptr)
	{
		// TODO  做一些资产的 是否被 提前加载的检查

		return Ptr.LoadSynchronous();
	}

};

