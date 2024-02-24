#include "DLUIManagerSubsystemModule.h"

#include "DLUIManagerSubsystem.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#endif // WITH_EDITOR

#define LOCTEXT_NAMESPACE "DLUIManagerSubsystem"

void FDLUIManagerSubsystem::StartupModule()
{
#if WITH_EDITOR
	// register settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Game", "DLUIManagerSubsystem",
			FText::FromString(TEXT("DLUIManagerSubsystem")), FText::FromString(TEXT("DLUIManagerSubsystem")),
			GetMutableDefault<UDLUIManagerSubsystem>()
		);
	}

#endif // WITH_EDITOR
}

void FDLUIManagerSubsystem::ShutdownModule()
{
#if WITH_EDITOR

	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Project", "Game", "DLUIManagerSubsystem");
	}
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDLUIManagerSubsystem, DLUIManagerSubsystem)