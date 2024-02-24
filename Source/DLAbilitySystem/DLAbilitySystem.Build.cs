// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLAbilitySystem : ModuleRules
{
    public DLAbilitySystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module 
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayAbilities",
            "GameplayTasks",
            "DataRegistry",
            "GameplayTags",
            "Json",
            "Settings",
            "AIModule",
            "Niagara",
            "PhysicsCore"
        });

        // Plugins
        PublicDependencyModuleNames.AddRange(new string[] {
            "EnhancedInput",
            "SMSystem",
            "GameplayMessageRuntime"
        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {
            "DLKit",
            "DLWeaponSystem",
            "DLGameplayTypes",
            "DLVirtualCameraSystem",
            "DLGameplayTargetSelector",
            "DLGameplayInputCmdSystem"
        });
    }
}
