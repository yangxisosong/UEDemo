
#include "CoreMinimal.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#endif // WITH_EDITOR

#include "Modules/ModuleInterface.h"
#include "PredictTask/UnderAttack/UnderAttackPredictTask.h"
#include "GameplayCoreLog.h"

struct FDLGameplayCore : public IModuleInterface
{
	virtual void StartupModule() override
	{
#if WITH_EDITOR
		// register settings
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		if (SettingsModule != nullptr)
		{
			ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Game", "UnderAttackLib",
				FText::FromString(TEXT("UnderAttackLib")), FText::FromString(TEXT("UnderAttackLib")),
				GetMutableDefault<UUnderAttackLib>()
			);
		}

#endif // WITH_EDITOR
	}

	virtual void ShutdownModule() override
	{
#if WITH_EDITOR

		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (SettingsModule != nullptr)
		{
			SettingsModule->UnregisterSettings("Project", "Game", "UnderAttackLib");
		}
#endif
	}
};

IMPLEMENT_MODULE(FDLGameplayCore, DLGameplayCore);



DEFINE_LOG_CATEGORY(LogDLGameplayCore);
DEFINE_LOG_CATEGORY(LogDLGameplayCoreAI);