// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLUIManagerSubsystem : ModuleRules
{
    public DLUIManagerSubsystem(ReadOnlyTargetRules Target) : base(Target)
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
            "DLAppFramework",
            "DLKit"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
        });
    }
}
