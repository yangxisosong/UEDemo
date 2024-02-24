// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLGameplayTypes : ModuleRules
{
    public DLGameplayTypes(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;
        
        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "GameplayAbilities",
             "DataRegistry",

        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
          
        });
    }
}
