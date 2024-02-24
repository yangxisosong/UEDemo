// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class GASDemoServerTarget : TargetRules
{
	public GASDemoServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
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
		ExtraModuleNames.Add("DLAppFramework");
		ExtraModuleNames.Add("DLUIDemo");

		ExtraModuleNames.Add("DLDemoNetWork");

		ExtraModuleNames.Add("DLAbstractLogicServer");

	}
}
