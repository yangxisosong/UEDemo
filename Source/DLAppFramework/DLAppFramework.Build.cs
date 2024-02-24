// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLAppFramework : ModuleRules
{
    public DLAppFramework(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;
        
        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "DeveloperSettings"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
        });
    }
}
