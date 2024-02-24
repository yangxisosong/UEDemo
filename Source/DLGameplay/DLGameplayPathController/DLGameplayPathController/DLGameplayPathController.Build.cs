// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLGameplayPathController : ModuleRules
{
    public DLGameplayPathController(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
            "DLKit",
            "DLDrawDebugSystem"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
        });
    }
}
