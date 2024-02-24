
#include "CoreMinimal.h"
#include "GameSavingSubSystem.h"
#include "Modules/ModuleInterface.h"


#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#endif


struct FDLGameSavingSystemModule
	: public IModuleInterface
{
	virtual void StartupModule() override
	{

#if WITH_EDITOR
		// register settings
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		if (SettingsModule != nullptr)
		{
			ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Game", "GameSavingSystem",
				FText::FromString(TEXT("GameSavingSystem")), FText::FromString(TEXT("GameSavingSystem")),
				GetMutableDefault<UGameSavingSubSystem>()
			);
		}
#endif


	}


	virtual void ShutdownModule() override
	{
#if WITH_EDITOR

		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (SettingsModule != nullptr)
		{
			SettingsModule->UnregisterSettings("Project", "Game", "GameSavingSystem");
		}
#endif
	}
};

IMPLEMENT_MODULE(FDLGameSavingSystemModule, DLGameSavingSystem);
