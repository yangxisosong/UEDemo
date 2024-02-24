// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GASDemoTarget : TargetRules
{
	public GASDemoTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
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
	}
}
