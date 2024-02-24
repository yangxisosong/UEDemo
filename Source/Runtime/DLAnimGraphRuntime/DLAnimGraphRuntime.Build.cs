// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLAnimGraphRuntime : ModuleRules
{
    public DLAnimGraphRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        PrivateIncludePaths.Add("Runtime/DLAnimGraphRuntime/Private");

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "AnimGraphRuntime",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "TraceLog",
                "GeometryCollectionEngine",
            }
        );

        SetupModulePhysicsSupport(Target);

        // External users of this library do not need to know about Eigen.
        AddEngineThirdPartyPrivateStaticDependencies(Target,
                "Eigen"
                );

        //if (Target.bCompileChaos || Target.bUseChaos)
        //{
        //    PublicDependencyModuleNames.AddRange(
        //        new string[] {
        //            "GeometryCollectionEngine",
        //        }
        //    );
        //}
    }
}
