#include "DLCustomizationDetail.h"
#include "DLDebugEditor.h"
#include "DLDebugObjEditorCommands.h"
#include "DLDebugObjEditorStyle.h"
#include "DLDebugObjMethodsExecutorModule.h"
#include "LevelEditor.h"
#include "DLDebugWidget.h"

#define LOCTEXT_NAMESPACE "FDLDebugObjMethodsExecutorModule"

void FDLDebugObjMethodsExecutorModule::StartupModule()
{
	//创建插件风格
	FDLDebugObjEditorStyle::Initialize();
	FDLDebugObjEditorStyle::ReloadTextures();

	//注册插件命令
	FDLDebugObjEditorCommands::Register();

	//创建插件列表
	EditorCommands = MakeShareable(new FUICommandList);

	//添加事件
	EditorCommands->MapAction(
		FDLDebugObjEditorCommands::Get().OpenDebugWindow,
		FExecuteAction::CreateRaw(this, &FDLDebugObjMethodsExecutorModule::OpenDebugTabButtonClicked),
		FCanExecuteAction());


	/*const TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarExtender->AddToolBarExtension("LevelEditor.MainMenu", EExtensionHook::After, EditorCommands,
	                                     FToolBarExtensionDelegate::CreateRaw(
		                                     this, &FDLDebugObjMethodsExecutorModule::AddToolbarExtension));

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);*/

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu");
	UToolMenu* DLToolsMenu = Menu->AddSubMenu(FToolMenuOwner(Menu->GetFName()), FName("DLDebug"), FName("DLDebug"), FText::FromString("DLDebug"));
	FToolMenuSection& Section = DLToolsMenu->FindOrAddSection("DLDebug");
	Section.AddMenuEntryWithCommandList(FDLDebugObjEditorCommands::Get().OpenDebugWindow, EditorCommands);
	//PropertyEditorModule.RegisterCustomPropertyTypeLayout(FName("DetailInfo"),
	//	FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDLCustomizationProperty::MakeInstance));


	if(!DLDebugEditor.IsValid())
	{
		DLDebugEditor = MakeShareable(new FDLDebugEditor);
	}
}

void FDLDebugObjMethodsExecutorModule::ShutdownModule()
{
	FDLDebugObjEditorStyle::Shutdown();

	FDLDebugObjEditorCommands::Unregister();

	//FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//PropertyEditorModule.UnregisterCustomPropertyTypeLayout(FName("DetailInfo"));

	DLDebugEditor.Reset();
}

void FDLDebugObjMethodsExecutorModule::OpenDebugTabButtonClicked() const
{
	UE_LOG(LogTemp, Warning, TEXT("OpenDebugTabButtonClicked"));

	if (DLDebugEditor.IsValid())
	{
		DLDebugEditor.Get()->CreatNewTab();


	}
}

void FDLDebugObjMethodsExecutorModule::AddToolbarExtension(FToolBarBuilder& Builder) const
{
	const FUIAction ComboCammand;
	Builder.AddComboButton(
		ComboCammand,
		FOnGetContent::CreateLambda([](TSharedPtr<class FUICommandList> Commands)
		                            {
			                            FMenuBuilder MenuBuilder(true, Commands);

			                            MenuBuilder.BeginSection(
				                            "Array", TAttribute<FText>(FText::FromString(TEXT("选项"))));
			                            MenuBuilder.AddMenuEntry(FDLDebugObjEditorCommands::Get().OpenDebugWindow);
			                            MenuBuilder.EndSection();

			                            return MenuBuilder.MakeWidget();
		                            },
		                            EditorCommands),
		LOCTEXT("Name", "Debug工具"),
		LOCTEXT("HoverTips", "Debug工具"),
		FSlateIcon(FDLDebugObjEditorStyle::GetStyleSetName(), "DLDebugObjEditorStyle.OpenDebugWindow"),
		false
	);
}

#undef LOCTEXT_NAMESPACE
