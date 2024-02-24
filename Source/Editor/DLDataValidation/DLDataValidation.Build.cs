// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLDataValidation : ModuleRules
{
    public DLDataValidation(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        OverridePackageType = PackageOverrideType.EngineDeveloper;

        PrivateIncludePaths.AddRange(
            new string[] {
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "DataValidation",
                "TargetPlatform",
                "EditorSubsystem"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "DLGameplayCharacter",
                "DLAbilitySystem"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Slate",
                "SlateCore",
                "InputCore",
                "UnrealEd",
                "AssetRegistry",
                "Json",
                "CollectionManager",
                "ContentBrowser",
                "WorkspaceMenuStructure",
                "EditorStyle",
                "AssetTools",
                "PropertyEditor",
                "GraphEditor",
                "BlueprintGraph",
                "KismetCompiler",
                "SandboxFile",
                "Blutility",
                "ToolMenus",
                "GameplayTags",

            }
        );
    }
}
