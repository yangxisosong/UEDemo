// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLGameplayCore : ModuleRules
{
    public DLGameplayCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Slate",
            "UMG",
            "Json",
            "AIModule",
            "GameplayAbilities",
            "GameplayTags",
            "DataRegistry",
            "JsonUtilities",
            "DeveloperSettings",
            "AssetRegistry",
            "NetCore"
        });

        // Plugins
        PublicDependencyModuleNames.AddRange(new string[] {
            "EnhancedInput",
             "GameplayMessageRuntime"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
            "DLAbilitySystem",
            "DLWeaponSystem",
            "DLAssetManager",
            "DLGameplayTypes",
            "DLGameSavingSystem",
            "DLGameplayPredictionTask",
            "DLGameplayInputCmdSystem",
            "DLAppFramework"
        });
    }
}
