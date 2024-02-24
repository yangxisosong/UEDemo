// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLDrawDebugSystem : ModuleRules
{
    public DLDrawDebugSystem(ReadOnlyTargetRules Target) : base(Target)
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
           "DLKit"
        });
    }
}
