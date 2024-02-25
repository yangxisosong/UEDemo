// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class CustomWidgetEditorTarget : TargetRules
{
	public CustomWidgetEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "CustomWidget" } );

        ExtraModuleNames.Add("DLApplication");

        ExtraModuleNames.Add("DLKit");
        ExtraModuleNames.Add("DLAbilitySystem");
        ExtraModuleNames.Add("DLAssetManager");
        ExtraModuleNames.Add("DLWeaponSystem");

        ExtraModuleNames.Add("DLGameplayCharacter");
        ExtraModuleNames.Add("DLGameplayCore");
        ExtraModuleNames.Add("DLGameplaySubObject");
        ExtraModuleNames.Add("DLGameplayTypes");
        ExtraModuleNames.Add("DLUIDemo");
        ExtraModuleNames.Add("DLAnimationEditorTool");
        ExtraModuleNames.Add("DLAnimGraphRuntime");
        ExtraModuleNames.Add("DLTargetSelectorEditor");

        ExtraModuleNames.Add("DLAnimGraph");

        ExtraModuleNames.Add("DLGameSavingSystem");


        ExtraModuleNames.Add("DLDataValidation");

        ExtraModuleNames.Add("DLVirtualCameraSystem");
        ExtraModuleNames.Add("DLGameplayTargetSelector");

        ExtraModuleNames.Add("DLGameplayPathController");
        ExtraModuleNames.Add("DLGameplayPathControllerEditor");
        ExtraModuleNames.Add("DLSequencerRuntime");
        ExtraModuleNames.Add("DLGameplayInputCmdSystem");
        ExtraModuleNames.Add("DLGameplayPredictionTask");
        ExtraModuleNames.Add("DLDrawDebugSystem");
        ExtraModuleNames.Add("DLGameplayCameraShake");
        ExtraModuleNames.Add("DLDebugObjMethodsExecutorEditor");

        ExtraModuleNames.Add("DLEditorTool");

        ExtraModuleNames.Add("DLUICore");

        ExtraModuleNames.Add("DLAbilityTest");
        ExtraModuleNames.Add("DLAbilityChainEditor");

        ExtraModuleNames.Add("DLAppFramework");

        ExtraModuleNames.Add("DLClientSubsystemCore");

        ExtraModuleNames.Add("DLCGeneralLoadingSubsystem");

        ExtraModuleNames.Add("DLUIManagerSubsystem");
        ExtraModuleNames.Add("ApplicationFrontEnd");

        ExtraModuleNames.Add("GameplayScene");


        ExtraModuleNames.Add("DLCDemoGameplaySubsys");


        ExtraModuleNames.Add("DLCFrontEndSubsystem");
        ExtraModuleNames.Add("DLUIComponentLibrary");
        ExtraModuleNames.Add("DLCDemoGameplaySubsys");

        ExtraModuleNames.Add("DLDemoNetWork");

        // TODO 临时先都把模块加了  
        ExtraModuleNames.Add("DLAbstractLogicServer");

        //bSupportEditAndContinue = true;
    }
}
