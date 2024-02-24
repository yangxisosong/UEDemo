// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ApplicationFrontEnd : ModuleRules
{
    public ApplicationFrontEnd(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",

            "GameplayMessageRuntime"

        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
            "DLAppFramework"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
        });
    }
}
