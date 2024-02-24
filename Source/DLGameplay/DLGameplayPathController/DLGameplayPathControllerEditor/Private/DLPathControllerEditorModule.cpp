#include "DLPathControllerEditorModule.h"
#include "PathControllerAction.h"
#include "PathControllerEditor.h"
#include "Modules/ModuleInterface.h"

class FDLGameplayPathControllerEditor : public IModuleInterface,public IPathControllerModule
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;


	virtual TSharedRef<IPathControllerEditor> CreatePathControllerEditor(
		const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Object) override;

private:
	TSharedPtr<FPathControllerAssetTypeActions> Actions;
	/** Array of component class names we have registered, so we know what to unregister afterwards */
	TArray<FName> RegisteredComponentClassNames;
};

void FDLGameplayPathControllerEditor::StartupModule()
{
	IAssetTools& AssetTool = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	auto AssetCategoryBit = AssetTool.RegisterAdvancedAssetCategory(FName(TEXT("DLAsset")),
																	FText::FromString("DLAsset"));
	Actions = MakeShared<FPathControllerAssetTypeActions>(AssetCategoryBit);
	AssetTool.RegisterAssetTypeActions(Actions.ToSharedRef());
}


void FDLGameplayPathControllerEditor::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTool = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTool.UnregisterAssetTypeActions(Actions.ToSharedRef());
	}
}

TSharedRef<IPathControllerEditor> FDLGameplayPathControllerEditor::CreatePathControllerEditor(
	const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Object)
{
	TSharedRef<FPathControllerEditor> Editor(new FPathControllerEditor());

	FPathControllerEditor::FInitArg Arg;
	Arg.Object = Object;
	Arg.InitToolkitHost = InitToolkitHost;

	Editor->InitTargetSelectorEditor(Arg);

	return Editor;
}

IMPLEMENT_MODULE(FDLGameplayPathControllerEditor, DLGameplayPathControllerEditor);

IPathControllerModule& IPathControllerModule::Get()
{
	return FModuleManager::LoadModuleChecked<FDLGameplayPathControllerEditor>("DLGameplayPathControllerEditor");
}
