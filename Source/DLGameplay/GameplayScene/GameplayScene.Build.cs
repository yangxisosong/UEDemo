// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameplayScene : ModuleRules
{
    public GameplayScene(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "EnhancedInput",
            "InputCore"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
            "DLAssetManager",
            "DLGameplayCore",
            "DLGameplayTypes",
            "DLAppFramework",
            "GameplayMessageRuntime"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "GameplayMessageRuntime"
        });
    }
}
