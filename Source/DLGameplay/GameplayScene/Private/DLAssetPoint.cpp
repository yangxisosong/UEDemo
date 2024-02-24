#include "GameplayScene/Public/DLAssetPoint.h"

#include "DLAssetManager.h"


void ADLAssetPoint::StartLoadAsset()
{
	UDLAssetManager& AssetManager = UDLAssetManager::Get();

	if (PrimaryAssetIds.Num() > 0)
	{
		StreamableHandle = AssetManager.LoadPrimaryAssets(PrimaryAssetIds, LoadBundles);
		if (!StreamableHandle.IsValid() || StreamableHandle->HasLoadCompleted())
		{
			this->OnLoadAssetComplate();
		}
		else
		{
			StreamableHandle->BindCompleteDelegate(FStreamableDelegate::CreateUObject(this, &ThisClass::OnLoadAssetComplate));
		}
	}
	else
	{
		this->OnLoadAssetComplate();
	}

}

void ADLAssetPoint::OnLoadAssetComplate()
{


}

void ADLAssetPoint::OnUnloadAsset()
{

}

void ADLAssetPoint::UnLoadAsset()
{
	this->OnUnloadAsset();

	if (StreamableHandle)
	{
		StreamableHandle->ReleaseHandle();
	}

	StreamableHandle = nullptr;
}

void ADLAssetPoint::BeginPlay()
{
	Super::BeginPlay();

	this->StartLoadAsset();
}

void ADLAssetPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::RemovedFromWorld)
	{
		this->UnLoadAsset();
	}
}


