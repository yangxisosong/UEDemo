// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLWeaponSystem : ModuleRules
{
    public DLWeaponSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;
        
        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "PhysicsCore"
        });

        // Project Module 
        PrivateDependencyModuleNames.AddRange(new string[] {
           "DLAssetManager",
           "DLKit",
        });

        PublicDependencyModuleNames.AddRange(new string[] {
            "DLGameplayTargetSelector"
        });
    }
}
