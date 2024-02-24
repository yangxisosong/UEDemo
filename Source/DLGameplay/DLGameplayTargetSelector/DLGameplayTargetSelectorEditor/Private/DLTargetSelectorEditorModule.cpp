#include "DLTargetSelectorEditorModule.h"

#include "AssetToolsModule.h"
#include "TargetSelectorAction.h"
#include "TargetSelectorAssetFactory.h"
#include "TargetSelectorEditor.h"
#include "TargetSelectStyle.h"
#include "Logging/LogMacros.h"
#include "Modules/ModuleInterface.h"

DEFINE_LOG_CATEGORY(DLTargetSelectorEditor);

class FDLTargetSelectorEditor
	: public IModuleInterface
	  , public ITargetSelectorModule
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;


	virtual TSharedRef<ITargetSelectorEditor> CreateTargetSelectorEditor(
		const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Object) override;


private:
	TSharedPtr<FTargetSelectorAssetTypeActions> ActionsSelector;
};


void FDLTargetSelectorEditor::StartupModule()
{
	IAssetTools& AssetTool = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	auto AssetCategoryBit = AssetTool.RegisterAdvancedAssetCategory(FName(TEXT("DLAsset")),
	                                                                FText::FromString("DLAsset"));
	FTargetSelectStyle::Initialize();

	ActionsSelector = MakeShared<FTargetSelectorAssetTypeActions>(AssetCategoryBit);
	AssetTool.RegisterAssetTypeActions(ActionsSelector.ToSharedRef());
}


void FDLTargetSelectorEditor::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTool = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		AssetTool.UnregisterAssetTypeActions(ActionsSelector.ToSharedRef());
	}

	FTargetSelectStyle::Shutdown();

}

TSharedRef<ITargetSelectorEditor> FDLTargetSelectorEditor::CreateTargetSelectorEditor(
	const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Object)
{
	TSharedRef<FTargetSelectorEditor> Editor(new FTargetSelectorEditor());

	FTargetSelectorEditor::FInitArg Arg;
	Arg.Object = Object;
	Arg.InitToolkitHost = InitToolkitHost;

	Editor->InitTargetSelectorEditor(Arg);

	return Editor;
}

IMPLEMENT_MODULE(FDLTargetSelectorEditor, DLTargetSelectorEditor)

ITargetSelectorModule& ITargetSelectorModule::Get()
{
	return FModuleManager::LoadModuleChecked<FDLTargetSelectorEditor>("DLTargetSelectorEditor");
}
