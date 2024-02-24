// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLTargetSelectorEditor : ModuleRules
{
    public DLTargetSelectorEditor(ReadOnlyTargetRules Target) : base(Target)
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
            "KismetWidgets",
            "InputCore",
            "AdvancedPreviewScene",
            "RenderCore",
            "RHI",
            "Unrealed",
            "DLGamePlayCharacter",
            "EditorStyle"

        });

        // Project Module 
        PrivateDependencyModuleNames.AddRange(new string[] {
           "DLGameplayTargetSelector"
        });

        PublicDependencyModuleNames.AddRange(new string[] {
           
        });
    }
}
