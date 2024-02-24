// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLAnimGraph : ModuleRules
{
    public DLAnimGraph(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

		PrivateIncludePaths.AddRange(
			new string[] {
				"Editor/DLAnimGraph/Private",
			}
		);

		OverridePackageType = PackageOverrideType.EngineDeveloper;

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"AnimGraphRuntime",
				"BlueprintGraph",
				"AnimGraph",
				"DLAnimGraphRuntime"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"InputCore",
				"SlateCore",
				"UnrealEd",
				"GraphEditor",
				"PropertyEditor",
				"EditorStyle",
				"ContentBrowser",
				"KismetWidgets",
				"ToolMenus",
				"KismetCompiler",
				"Kismet",
				"EditorWidgets",
			}
		);

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
				"Persona",
				"SkeletonEditor",
				"AdvancedPreviewScene",
				"AnimationBlueprintEditor",
			}
		);
	}
}
