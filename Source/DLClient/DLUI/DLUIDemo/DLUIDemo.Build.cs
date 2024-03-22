// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLUIDemo : ModuleRules
{
    public DLUIDemo(ReadOnlyTargetRules Target) : base(Target)
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
            "UMG",
            "GameplayAbilities",
            "InputCore",
            "GameplayTasks"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
           
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "DLUICore",
            "DLAbilitySystem",
            "DLGameplayTypes",
            "DLCFrontEndSubsystem",
            "DLCDemoGameplaySubsys",
            "GameplayScene"
        });
    }
}
