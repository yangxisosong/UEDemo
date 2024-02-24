// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLCFrontEndSubsystem : ModuleRules
{
    public DLCFrontEndSubsystem(ReadOnlyTargetRules Target) : base(Target)
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
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
            "DLClientSubsystemCore",
            "DLUIManagerSubsystem",
            "GameplayMessageRuntime",
            "DLAppFramework"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
        });
    }
}
