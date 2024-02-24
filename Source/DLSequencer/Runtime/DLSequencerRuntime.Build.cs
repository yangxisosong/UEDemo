// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLSequencerRuntime : ModuleRules
{
    public DLSequencerRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "LevelSequence",
            "MovieScene"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {

        });

        PrivateDependencyModuleNames.AddRange(new string[] {
        });
    }
}
