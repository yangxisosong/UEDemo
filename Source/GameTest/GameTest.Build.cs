// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameTest : ModuleRules
{
    public GameTest(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;
        
        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Blutility",
            "UMG",
            "UnrealEd",
            "InputCore",
            "EnhancedInput"
        });

        // Project Module 
        PrivateDependencyModuleNames.AddRange(new string[] {
        });
    }
}
