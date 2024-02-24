// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLKit : ModuleRules
{
    public DLKit(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;
        
        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "JsonUtilities",
            "Json"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
           
        });
    }
}
