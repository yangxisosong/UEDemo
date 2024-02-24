#pragma once

#include "CoreMinimal.h"

class IDLWeapon;

class DLWEAPONSYSTEM_API IDLWeaponSystem
{
public:
	virtual ~IDLWeaponSystem() = default;

	static IDLWeaponSystem& Get();


	virtual TArray<TScriptInterface<IDLWeapon>> CreateDLWeaponsByAssetId(UWorld* World, FPrimaryAssetId AssetId) =0;
};
