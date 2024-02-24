// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLClientSubsystemCore : ModuleRules
{
    public DLClientSubsystemCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "GameplayTags",
            "Engine",
            "UMG"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
            "DLUIManagerSubsystem",
            "GameplayMessageRuntime",
            "DLAppFramework",
            "DLKit"

        });

        PrivateDependencyModuleNames.AddRange(new string[] {
        });
    }
}
