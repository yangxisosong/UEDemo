// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLGameSavingSystem : ModuleRules
{
    public DLGameSavingSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayAbilities",
            "GameplayTags",
             "DataRegistry"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {

        });
    }
}
