#include "DLAbilityChainEditorModule.h"
#include "AbilityChainNodeAction.h"
#include "AbilityChainNodeEditor.h"
#include "Modules/ModuleInterface.h"
#include "EdGraphUtilities.h"
#include "AbilityChainNodeEdGraphNode.h"
#include "SGraphNode_AbilityChainNode.h"
#include "AbilityChainNodeEditorCommands.h"
#include "AbilityChainNodeEditorStyle.h"

class FGraphPanelNodeFactory_TreeNode : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override
	{
		if (UAbilityChainNodeEdGraphNode* BTNode = Cast<UAbilityChainNodeEdGraphNode>(Node))
		{
			return SNew(SGraphNode_AbilityChainNode, BTNode);
		}
		return nullptr;
	}
};
class FDLAbilityChainEditor :
		public IModuleInterface,
		public IAbilityChainNodeModule
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;


	virtual TSharedRef<IDLAbilityChainEditor> CreateAbilityChainNodeEditor(
		const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Object) override;

private:
	TSharedPtr<FAbilityChainNodeAssetTypeActions> Actions;
	/** Array of component class names we have registered, so we know what to unregister afterwards */
	TArray<FName> RegisteredComponentClassNames;

	TSharedPtr<FGraphPanelNodeFactory> Factory;
};

void FDLAbilityChainEditor::StartupModule()
{
	IAssetTools& AssetTool = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	auto AssetCategoryBit = AssetTool.RegisterAdvancedAssetCategory(FName(TEXT("DLAsset")),
																	FText::FromString("DLAsset"));
	Actions = MakeShared<FAbilityChainNodeAssetTypeActions>(AssetCategoryBit);
	AssetTool.RegisterAssetTypeActions(Actions.ToSharedRef());

	Factory = MakeShareable(new FGraphPanelNodeFactory_TreeNode());
	FEdGraphUtilities::RegisterVisualNodeFactory(Factory);

	//创建插件风格
	FAbilityChainNodeEditorStyle::Initialize();
	FAbilityChainNodeEditorStyle::ReloadTextures();

	//注册插件命令
	FAbilityChainNodeEditorCommands::Register();
}


void FDLAbilityChainEditor::ShutdownModule()
{
	FAbilityChainNodeEditorStyle::Shutdown();
	FAbilityChainNodeEditorCommands::Unregister();

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTool = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTool.UnregisterAssetTypeActions(Actions.ToSharedRef());
	}

	FEdGraphUtilities::UnregisterVisualNodeFactory(Factory);
}

TSharedRef<IDLAbilityChainEditor> FDLAbilityChainEditor::CreateAbilityChainNodeEditor(
	const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Object)
{
	TSharedRef<FAbilityChainNodeEditor> Editor(new FAbilityChainNodeEditor());

	FAbilityChainNodeEditor::FInitArg Arg;
	Arg.Object = Object;
	Arg.InitToolkitHost = InitToolkitHost;

	Editor->InitTreeNodeEditor(Arg);

	return Editor;
}

IMPLEMENT_MODULE(FDLAbilityChainEditor, DLAbilityChainEditor);

IAbilityChainNodeModule& IAbilityChainNodeModule::Get()
{
	return FModuleManager::LoadModuleChecked<FDLAbilityChainEditor>("DLAbilityChainEditor");
}
