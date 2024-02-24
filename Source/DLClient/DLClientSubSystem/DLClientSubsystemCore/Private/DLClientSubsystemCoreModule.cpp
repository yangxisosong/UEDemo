#include "DLClientSubsystemCoreModule.h"
#include "DLClient/DLClientSubSystem/DLClientSubsystemCore/Public/DLClientSubsysManager.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#endif // WITH_EDITOR

#include "DLClientSubsystemLog.h"

DEFINE_LOG_CATEGORY(LogDLCSubsystem);


void FDLClientSubsystemCore::StartupModule()
{
#if WITH_EDITOR
	// register settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Game", "DLClientSubsystemManager",
			FText::FromString(TEXT("DLClientSubsystemManager")), FText::FromString(TEXT("DLClientSubsystemManager")),
			GetMutableDefault<UDLClientSubsysManager>()
		);
	}

#endif // WITH_EDITOR
}

void FDLClientSubsystemCore::ShutdownModule()
{
#if WITH_EDITOR

	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Project", "Game", "DLClientSubsystemManager");
	}
#endif
}

IMPLEMENT_MODULE(FDLClientSubsystemCore, DLClientSubsystemCore)