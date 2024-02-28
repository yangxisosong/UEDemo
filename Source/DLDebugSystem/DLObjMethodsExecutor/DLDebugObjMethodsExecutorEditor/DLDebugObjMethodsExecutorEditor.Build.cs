// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLDebugObjMethodsExecutorEditor : ModuleRules
{
    public DLDebugObjMethodsExecutorEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;
        
        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "Slate",
            "SlateCore",
            "UnrealEd",
            "LevelEditor",
            "Projects",
            "EditorStyle",
            "WorldBrowser",
            "InputCore",
            "UMG",
            "PropertyEditor",
            "Kismet",
            "ToolMenus",
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
           "DLDebugObjMethodsExecutor",
           "DLGameplayPathController"
        });
    }
}
