// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLGameplaySubObject : ModuleRules
{
    public DLGameplaySubObject(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        // Engine Module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks"

        });

        // Project Module 
        PublicDependencyModuleNames.AddRange(new string[] {

        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "DLAbilitySystem",
            "DLGameplayTargetSelector",
            "DLGameplayTypes"
        });
    }
}
