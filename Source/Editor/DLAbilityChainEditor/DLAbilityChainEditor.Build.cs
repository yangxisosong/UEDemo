// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLAbilityChainEditor : ModuleRules
{
    public DLAbilityChainEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "UnrealEd",
            "Slate",
            "SlateCore",
            "InputCore",
            "EditorStyle",
            "Kismet",
            "AssetManagerEditor",
            "GraphEditor",
            "ToolMenus",
            "ApplicationCore",
            "Json",
            "EditorScriptingUtilities"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
            "DLAbilitySystem"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
        });
    }
}
