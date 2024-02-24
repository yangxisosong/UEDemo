// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLAbstractLogicServer : ModuleRules
{
    public DLAbstractLogicServer(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Json"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
        });
    }
}
