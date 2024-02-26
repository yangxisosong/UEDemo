// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLGameplayPathControllerEditor : ModuleRules
{
    public DLGameplayPathControllerEditor(ReadOnlyTargetRules Target) : base(Target)
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
            "AdvancedPreviewScene",
            "ComponentVisualizers",
            "Kismet",
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
            "DLGameplayPathController"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
        });
    }
}
