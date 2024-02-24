// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLCDemoGameplaySubsys : ModuleRules
{
    public DLCDemoGameplaySubsys(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "UMG",
            "GameplayAbilities",
            "DataRegistry",
            "InputCore"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
            "DLClientSubsystemCore",
            "DLUIManagerSubsystem",
            "DLAppFramework",
            "DLGameplayTypes",
            "GameplayMessageRuntime",
            "DLGameplayCore",
            "DLAbilitySystem",
            "GameplayScene"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
        });
    }
}
