// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLGameplayCharacter : ModuleRules
{
    public DLGameplayCharacter(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "UMG",
            "GameplayAbilities",
            "AIModule",
            "GameplayTasks",
            "JsonUtilities",
            "Json",
            "DataRegistry",
            "PhysicsCore",
            "Niagara",
            "InputCore"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
           "DLGameplayCore",
           "DLAbilitySystem",
           "DLAssetManager",
           "DLGameplayTypes",
           "DLVirtualCameraSystem",
           "DLGameplayInputCmdSystem",
           "DLGameplayPredictionTask",
           "DLAppFramework",
           "DLKit"
        });

        PublicDependencyModuleNames.AddRange(new string[] {
           "SMSystem",
           "GameplayMessageRuntime"
        });
    }
}
