// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLAnimationEditorTool : ModuleRules
{
    public DLAnimationEditorTool(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;
        
        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "AnimationModifiers",
            "AnimationBlueprintLibrary"
        });

        // Project Module 
        PrivateDependencyModuleNames.AddRange(new string[] {
           
        });
    }
}
