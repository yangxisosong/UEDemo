// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLCGeneralLoadingSubsystem : ModuleRules
{
    public DLCGeneralLoadingSubsystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "UMG",
            "SlateCore",
            "Slate",
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
             "DLClientSubsystemCore",
             "DLUIManagerSubsystem",
              "DLGameplayCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
        });
    }
}
