#include "DLAssetManager.h"

UDLAssetManager& UDLAssetManager::Get()
{
	UDLAssetManager* Obj = Cast<UDLAssetManager>(GEngine->AssetManager);
	checkf(Obj, TEXT("这里一定能够取到，取不到可能是配置不对 Or 时机太早"));
	return *Obj;
}
