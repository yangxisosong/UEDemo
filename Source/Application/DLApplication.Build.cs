// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLApplication : ModuleRules
{
    public DLApplication(ReadOnlyTargetRules Target) : base(Target)
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
            "GameplayDebugger",
            "GameplayAbilities",
            "GameplayTasks",
            "GameplayTags",
            "DataRegistry",
            "DataRegistry"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
            "DLGameplayCore",
            "DLAssetManager",
            "DLGameplayTypes",
            "DLAppFramework",
            "GameplayMessageRuntime",
            "DLGameSavingSystem",
            "DLAbstractLogicServer"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {

        });
    }
}
