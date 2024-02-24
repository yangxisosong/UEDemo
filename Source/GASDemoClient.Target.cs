// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GASDemoClientTarget : TargetRules
{
	public GASDemoClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.Add("DLApplication");
		ExtraModuleNames.Add("DLKit");
		ExtraModuleNames.Add("DLAbilitySystem");
		ExtraModuleNames.Add("DLAssetManager");
		ExtraModuleNames.Add("DLWeaponSystem");
		ExtraModuleNames.Add("DLGameplayCharacter");
		ExtraModuleNames.Add("DLGameplayCore");
		ExtraModuleNames.Add("DLGameplaySubObject");
		ExtraModuleNames.Add("DLGameplayTypes");
		ExtraModuleNames.Add("DLUIDemo");
		ExtraModuleNames.Add("DLAnimGraphRuntime");
		ExtraModuleNames.Add("DLGameSavingSystem");
		ExtraModuleNames.Add("DLVirtualCameraSystem");
		ExtraModuleNames.Add("DLGameplayTargetSelector");
		ExtraModuleNames.Add("DLGameplayPathController");
		ExtraModuleNames.Add("DLSequencerRuntime");
		ExtraModuleNames.Add("DLGameplayInputCmdSystem");
		ExtraModuleNames.Add("DLGameplayPredictionTask");
		ExtraModuleNames.Add("DLDrawDebugSystem");
		ExtraModuleNames.Add("DLGameplayCameraShake");
		ExtraModuleNames.Add("DLUICore");
		ExtraModuleNames.Add("DLAppFramework");
		ExtraModuleNames.Add("DLClientSubsystemCore");
		ExtraModuleNames.Add("DLCGeneralLoadingSubsystem");
		ExtraModuleNames.Add("DLUIManagerSubsystem");
		ExtraModuleNames.Add("ApplicationFrontEnd");
		ExtraModuleNames.Add("DLCFrontEndSubsystem");

		ExtraModuleNames.Add("DLCDemoGameplaySubsys");

		ExtraModuleNames.Add("DLDemoNetWork");

		// TODO 临时先都把模块加了  
		ExtraModuleNames.Add("DLAbstractLogicServer");
	}
}
