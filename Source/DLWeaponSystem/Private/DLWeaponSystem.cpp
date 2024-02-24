
#include "CoreMinimal.h"
#include "DLAssetManager.h"
#include "DLWeaponBase.h"
#include "IDLWeaponSystem.h"
#include "WeaponAssetDef.h"
#include "Engine/AssetManager.h"
#include "Modules/ModuleInterface.h"

struct FDLWeaponSystem
	: public IModuleInterface
	, public IDLWeaponSystem
{
	virtual void StartupModule() override
	{

	};
	virtual void ShutdownModule() override {};
	virtual bool IsGameModule() const override { return true; }

	virtual TArray<TScriptInterface<IDLWeapon>> CreateDLWeaponsByAssetId(UWorld* World, FPrimaryAssetId AssetId) override
	{
		TArray<TScriptInterface<IDLWeapon>> Ret;

		if (!World)
		{
			return Ret;
		}

		// TODO 临时直接加载武器资产
		const auto AssetPath = UAssetManager::Get().GetPrimaryAssetPath(AssetId);
		UDLWeaponAsset* WeaponAsset = Cast<UDLWeaponAsset>(AssetPath.TryLoad());
		if (!WeaponAsset)
		{
			return Ret;
		}

		const FAttachmentTransformRules Rules{
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::KeepWorld,
				true
		};
		
	
		for (const auto& Class : WeaponAsset->WeaponActorClassArr)
		{
			const auto ClassRef = Class.LoadSynchronous();
			if (!ClassRef)
			{
				continue;
			}

			const auto Weapon = World->SpawnActor<ADLWeaponBase>(ClassRef);

			if (!Weapon)
			{
				continue;
			}

			Weapon->AppendBaseWeaponTags(WeaponAsset->BaseWeaponTag);
			
			Ret.Add(Weapon);						
		}

		return Ret;
	}
};


IDLWeaponSystem& IDLWeaponSystem::Get()
{
	IModuleInterface& Interface = FModuleManager::Get().LoadModuleChecked("DLWeaponSystem");
	FDLWeaponSystem& WeaponSystem = static_cast<FDLWeaponSystem&>(Interface);
	return WeaponSystem;
}



IMPLEMENT_MODULE(FDLWeaponSystem, DLWeaponSystem);
