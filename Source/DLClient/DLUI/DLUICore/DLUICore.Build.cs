// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLUICore : ModuleRules
{
    public DLUICore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;
        
        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "UMG",
            "GameplayTags",
            "GameplayAbilities",
            "InputCore"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
           "DLGameplayTypes",
           "GameplayMessageRuntime",
           "DLUIManagerSubsystem",
           "DLAppFramework",
           "DLKit"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
        });
    }
}
