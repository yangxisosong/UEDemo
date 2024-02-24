#include "DLNPCStartPoint.h"
#include "Editor.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "IPlacementModeModule.h"
#include "Modules/ModuleInterface.h"
#include "Trigger/DLEnterSceneTrigger.h"

class FDLEditorTool
	: public IModuleInterface
{
	virtual void StartupModule() override
	{
		//const TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		//const TSharedPtr<FUICommandList> UICommandList = MakeShareable(new FUICommandList);
		//
		//
		//
		//UICommandList->MapAction()
		//ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, EditorCommands,
		//	FToolBarExtensionDelegate::CreateRaw(
		//		this, &FDLDebugObjMethodsExecutorModule::AddToolbarExtension));
		//
		//FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		//LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);


		static bool InitedEditorUnity = false;
		FCoreDelegates::OnEndFrame.AddLambda([]()
		{
			// if (!InitedEditorUnity)
			// {
			// 	UObject* obj = StaticLoadObject(UObject::StaticClass(), NULL,
			// 									 TEXT("/Game/RootMotionSourceDemo/DrawCurve.DrawCurve"), NULL, LOAD_None, NULL);
			// 	if (obj)
			// 	{
			// 		UEditorUtilityWidgetBlueprint* EditorWidget = Cast<UEditorUtilityWidgetBlueprint>(obj);
			// 		UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
			// 		EditorUtilitySubsystem->SpawnAndRegisterTab(EditorWidget);
			// 	}
			// 	InitedEditorUnity = true;
			// }
		});


		//auto& PlacementModeModule = IPlacementModeModule::Get();
		//
		//PlacementModeModule.RegisterPlaceableItem(TEXT("Basic"),
		//			MakeShareable( new FPlaceableItem(nullptr, FAssetData(ADLNPCStartPoint::StaticClass()))));
		//
		//PlacementModeModule.RegisterPlaceableItem(TEXT("Basic"),
		//	MakeShareable( new FPlaceableItem(nullptr, FAssetData(ADLEnterSceneTrigger::StaticClass()))));
		//
	}

	virtual void ShutdownModule() override
	{
	}
};


IMPLEMENT_MODULE(FDLEditorTool, DLEditorTool)