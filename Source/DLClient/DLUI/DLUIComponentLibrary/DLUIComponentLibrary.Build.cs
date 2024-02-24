// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLUIComponentLibrary : ModuleRules
{
    public DLUIComponentLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;
        
        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "UMG"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
           
        });
    }
}
