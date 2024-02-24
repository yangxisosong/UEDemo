// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLEditorTool : ModuleRules
{
    public DLEditorTool(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;
        
        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Blutility",
            "EditorScriptingUtilities",
            "UMG",
            "UMGEditor",
            "UnrealEd",
            "PlacementMode"
        });

        // Project Module 
        PrivateDependencyModuleNames.AddRange(new string[] {
           "DLGameplayCore",
           "GameplayScene"
        });
    }
}
