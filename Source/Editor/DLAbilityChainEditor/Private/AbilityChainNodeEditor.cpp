#include "AbilityChainNodeEditor.h"
#include "AbilityChainAsset2.h"
#include "DesktopPlatformModule.h"
#include "EdGraphUtilities.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SSpinBox.h"
#include "SSCSEditor.h"
#include "AbilityChainNodeEdGraph.h"
#include "AbilityChainNodeEdGraphNode.h"
#include "AbilityChainNodeEdGraphSchema.h"
#include "AbilityChainNodeEditorCommands.h"
#include "AbilityChainNodeEditorStyle.h"
#include "Editor/UnrealEdEngine.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Misc/FileHelper.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "Modules/ModuleManager.h"
#include "Slate/Public/Framework/Commands/GenericCommands.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

DEFINE_LOG_CATEGORY(LogAbilityChainEditor);
const FName FAbilityChainNodeEditor::TreeNodeEditorAppIdentifier(TEXT("AbilityChainNodeEditorApp"));

namespace
{
	const FName EditorViewPortTabId(TEXT("EditorViewPortTabId"));
	const FName EditorDetailTabId(TEXT("EditorDetailTabId"));
	const FName EditorNodeDetailTabId(TEXT("EditorNodeDetailTabId"));
}

FAbilityChainNodeEditor::FAbilityChainNodeEditor(): NodeEdGraph(nullptr), AbilityChainNodeAssetObject(nullptr)
{
}

void FAbilityChainNodeEditor::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}

	GraphEditorCommands = MakeShareable(new FUICommandList);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
	                               FExecuteAction::CreateRaw(this, &FAbilityChainNodeEditor::DeleteSelectedNodes),
	                               FCanExecuteAction::CreateRaw(this, &FAbilityChainNodeEditor::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
	                               FExecuteAction::CreateRaw(this, &FAbilityChainNodeEditor::CopySelectedNodes),
	                               FCanExecuteAction::CreateRaw(this, &FAbilityChainNodeEditor::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
	                               FExecuteAction::CreateRaw(this, &FAbilityChainNodeEditor::PasteNodes),
	                               FCanExecuteAction::CreateRaw(this, &FAbilityChainNodeEditor::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
	                               FExecuteAction::CreateRaw(this, &FAbilityChainNodeEditor::CutSelectedNodes),
	                               FCanExecuteAction::CreateRaw(this, &FAbilityChainNodeEditor::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
	                               FExecuteAction::CreateRaw(this, &FAbilityChainNodeEditor::DuplicateNodes),
	                               FCanExecuteAction::CreateRaw(this, &FAbilityChainNodeEditor::CanDuplicateNodes)
	);
}

void FAbilityChainNodeEditor::AddButtonToToolBar()
{
	const TSharedPtr<FUICommandList> EditorCommands = MakeShareable(new FUICommandList);
	const TSharedPtr<FUICommandList> EditorCommandsBuild = MakeShareable(new FUICommandList);

	//添加事件
	EditorCommands->MapAction(
		FAbilityChainNodeEditorCommands::Get().FileToJson,
		FExecuteAction::CreateRaw(this, &FAbilityChainNodeEditor::ClickToJson),
		FCanExecuteAction());

	EditorCommandsBuild->MapAction(
		FAbilityChainNodeEditorCommands::Get().Build,
		FExecuteAction::CreateRaw(this, &FAbilityChainNodeEditor::ClickBuild),
		FCanExecuteAction());

	const TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		EditorCommands,
		FToolBarExtensionDelegate::CreateLambda([](FToolBarBuilder build)
		{
			build.AddToolBarButton(
				FAbilityChainNodeEditorCommands::Get().FileToJson,
				NAME_None,
				FText::FromString("ToJson"),
				FText::FromString("FileToJson"),
				FSlateIcon(FAbilityChainNodeEditorStyle::GetStyleSetName(), "AbilityChainNodeEditorStyle.FileToJson"));
		})
	);
	const TSharedPtr<FExtender> ToolbarExtenderBuild = MakeShareable(new FExtender);
	ToolbarExtenderBuild->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		EditorCommandsBuild,
		FToolBarExtensionDelegate::CreateLambda([](FToolBarBuilder build)
		{
			build.AddToolBarButton(
				FAbilityChainNodeEditorCommands::Get().Build,
				NAME_None,
				FText::FromString("Build"),
				FText::FromString("Build Json To Asset"),
				FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.ViewOptions"));
		})
	);

	AddToolbarExtender(ToolbarExtender);
	AddToolbarExtender(ToolbarExtenderBuild);
}

FGraphPanelSelectionSet FAbilityChainNodeEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	const TSharedPtr<SGraphEditor> FocusedGraphEd = GraphEditor;
	if (FocusedGraphEd)
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}

	return CurrentSelection;
}


void FAbilityChainNodeEditor::InitTreeNodeEditor(const FInitArg& Arg)
{
	//InitBlueprintEditor(EToolkitMode::Standalone, Arg.InitToolkitHost,);
	AbilityChainNodeAssetObject = Cast<UAbilityChainAsset2>(Arg.Object);
	const TArray<UObject*>* EditedObjects = FAssetEditorToolkit::GetObjectsCurrentlyBeingEdited();

	CreateCommandList();

	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(AbilityChainNodeAssetObject);

	if (EditedObjects == nullptr || EditedObjects->Num() == 0)
	{
		constexpr bool bCreateDefaultToolbar = true;
		constexpr bool bCreateDefaultStandaloneMenu = true;
		const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("Standalone_CustomizeAbilityChainNodeEditor")
			->AddArea
			(
				FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
				                             //->Split
				                             //(
					                            // FTabManager::NewStack()
					                            // ->AddTab(GetToolbarTabId(), ETabState::OpenedTab)->SetHideTabWell(false)
				                             //)
				                             ->Split
				                             (
					                             FTabManager::NewSplitter()
					                             ->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(1.0f)
					                             ->Split
					                             (
						                             FTabManager::NewStack()
						                             ->SetSizeCoefficient(0.8f)
						                             ->AddTab(EditorViewPortTabId, ETabState::OpenedTab)->
						                             SetHideTabWell(true)
					                             )
					                             ->Split
					                             (
						                             FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
						                                                       ->SetSizeCoefficient(0.2f)
						                                                       ->Split
						                                                       (
							                                                       FTabManager::NewStack()
							                                                       ->SetSizeCoefficient(0.5f)
							                                                       ->AddTab(EditorDetailTabId,
								                                                       ETabState::OpenedTab)->
							                                                       SetHideTabWell(false)
						                                                       )
						                                                       ->Split
						                                                       (
							                                                       FTabManager::NewStack()
							                                                       ->SetSizeCoefficient(0.5f)
							                                                       ->AddTab(EditorNodeDetailTabId,
								                                                       ETabState::OpenedTab)->
							                                                       SetHideTabWell(false)
						                                                       )

					                             )
				                             )
			);

		InitAssetEditor(EToolkitMode::Standalone,
		                Arg.InitToolkitHost,
		                TreeNodeEditorAppIdentifier,
		                Layout,
		                bCreateDefaultStandaloneMenu,
		                bCreateDefaultToolbar,
		                ObjectsToEdit);
	}
	else
	{
		if (!EditedObjects->Contains(AbilityChainNodeAssetObject))
		{
			AddEditingObject(AbilityChainNodeAssetObject);
		}
	}

	AddButtonToToolBar();

	RegenerateMenusAndToolbars();
}


void FAbilityChainNodeEditor::CreateDetailWidget(const TSharedRef<class FTabManager>& InTabManager)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
		"PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, false);
	DetailsViewArgs.NotifyHook = this;
	DetailsViewArgs.HostTabManager = InTabManager;

	DetailContent = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailContent->ClearSearch();
	DetailContent->SetObject(AbilityChainNodeAssetObject);
}

void FAbilityChainNodeEditor::CreateNodeDetailWidget(const TSharedRef<FTabManager>& InTabManager)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
		"PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, false);
	DetailsViewArgs.NotifyHook = this;
	DetailsViewArgs.HostTabManager = InTabManager;

	NodeDetailContent = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	NodeDetailContent->ClearSearch();
	NodeDetailContent->SetObject(nullptr);
}

void FAbilityChainNodeEditor::SaveAsset_Execute()
{
	NodeJsonData JsonData;

	if (Cast<UAbilityChainNodeEdGraph>(this->AbilityChainNodeAssetObject->EdGraph)->RootNode)
	{
		GetAbilityChainNodeInfo(JsonData, Cast<UAbilityChainNodeEdGraph>(
			                this->AbilityChainNodeAssetObject->EdGraph)->RootNode);
	}

	FString JsonStr = JsonData.ToJson();
	UE_LOG(LogAbilityChainEditor, Log, TEXT("AbilityChainNode.ToJson: %s"), *JsonStr);

	this->AbilityChainNodeAssetObject->AbilityChainData = JsonStr;
	this->AbilityChainNodeAssetObject->EdGraph = Cast<UAbilityChainNodeEdGraph>(GraphEditor.Get()->GetCurrentGraph());

	FAssetEditorToolkit::SaveAsset_Execute();
}

void FAbilityChainNodeEditor::DeleteSelectedNodes() const
{
	const TSharedPtr<SGraphEditor> CurrentGraphEditor = GraphEditor;
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());
	CurrentGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		if (UAbilityChainNodeEdGraphNode* Node = Cast<UAbilityChainNodeEdGraphNode>(*NodeIt))
		{
			if (Node->CanUserDeleteNode())
			{
				Node->Modify();
				if(Node->ParentNode)
				{
					Node->ParentNode->RemoveSubNode(Node);
				}
				Node->DestroyNode();
			}
		}
	}
}

bool FAbilityChainNodeEditor::CanDeleteNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		const UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node != Cast<UAbilityChainNodeEdGraph>(AbilityChainNodeAssetObject->EdGraph)->RootNode)
		{
			return true;
		}
	}

	return false;
}

void FAbilityChainNodeEditor::DeleteSelectedDuplicatableNodes() const
{
	const TSharedPtr<SGraphEditor> CurrentGraphEditor = GraphEditor;
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FGraphPanelSelectionSet OldSelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}

	// Delete the duplicatable nodes
	DeleteSelectedNodes();

	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes);
	     SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FAbilityChainNodeEditor::CutSelectedNodes() const
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FAbilityChainNodeEditor::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FAbilityChainNodeEditor::CopySelectedNodes() const
{
	// Export the selected nodes and place the text on the clipboard
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	TArray<UAbilityChainNodeEdGraphNode*> SubNodes;

	FString ExportedText;

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		const UAbilityChainNodeEdGraphNode* TreeNode = Cast<UAbilityChainNodeEdGraphNode>(Node);
		if (Node == nullptr)
		{
			SelectedIter.RemoveCurrent();
			continue;
		}

		Node->PrepareForCopying();

		if (TreeNode)
		{
		}
	}

	for (int32 Idx = 0; Idx < SubNodes.Num(); Idx++)
	{
		SelectedNodes.Add(SubNodes[Idx]);
		SubNodes[Idx]->PrepareForCopying();
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UAbilityChainNodeEdGraphNode* Node = Cast<UAbilityChainNodeEdGraphNode>(*SelectedIter);
		if (Node)
		{
			Node->PostCopyNode();
		}
	}
}

bool FAbilityChainNodeEditor::CanCopyNodes() const
{
	// If any of the nodes can be duplicated then we should allow copying
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		const UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node != Cast<UAbilityChainNodeEdGraph>(AbilityChainNodeAssetObject->EdGraph)->RootNode)
		{
			return true;
		}
	}

	return false;
}

void FAbilityChainNodeEditor::PasteNodes() const
{
	if (GraphEditor.IsValid())
	{
		PasteNodesHere(GraphEditor->GetPasteLocation());
	}
}

void FAbilityChainNodeEditor::PasteNodesHere(const FVector2D& Location) const
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GraphEditor;
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	// Undo/Redo support
	const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());
	UEdGraph* EdGraph = CurrentGraphEditor->GetCurrentGraph();
	UAbilityChainNodeEdGraph* TreeNodeEdGraph = Cast<UAbilityChainNodeEdGraph>(EdGraph);

	EdGraph->Modify();
	if (TreeNodeEdGraph)
	{
		//TreeNode->LockUpdates();
	}

	UAbilityChainNodeEdGraphNode* SelectedParent = nullptr;
	bool bHasMultipleNodesSelected = false;

	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UAbilityChainNodeEdGraphNode* Node = Cast<UAbilityChainNodeEdGraphNode>(*SelectedIter);

		if (Node)
		{
			if (SelectedParent == nullptr)
			{
				SelectedParent = Node;
			}
			else
			{
				bHasMultipleNodesSelected = true;
				break;
			}
		}
	}

	// Clear the selection set (newly pasted stuff will be selected)
	CurrentGraphEditor->ClearSelectionSet();

	// Grab the text to paste from the clipboard.
	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);

	// Import the nodes
	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(EdGraph, TextToImport, /*out*/ PastedNodes);

	//Average position of nodes so we can move them while still maintaining relative distances to each other
	FVector2D AvgNodePosition(0.0f, 0.0f);

	// Number of nodes used to calculate AvgNodePosition
	int32 AvgCount = 0;

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* EdNode = *It;
		UAbilityChainNodeEdGraphNode* Node = Cast<UAbilityChainNodeEdGraphNode>(EdNode);
		if (EdNode && (Node != nullptr))
		{
			AvgNodePosition.X += EdNode->NodePosX;
			AvgNodePosition.Y += EdNode->NodePosY;
			++AvgCount;
		}
	}

	if (AvgCount > 0)
	{
		float InvNumNodes = 1.0f / static_cast<float>(AvgCount);
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;
	}

	bool bPastedParentNode = false;

	TMap<FGuid/*New*/, FGuid/*Old*/> NewToOldNodeMapping;

	TMap<int32, UAbilityChainNodeEdGraphNode*> ParentMap;
	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* PasteNode = *It;
		UAbilityChainNodeEdGraphNode* PasteTreeNode = Cast<UAbilityChainNodeEdGraphNode>(PasteNode);

		if (PasteNode && (PasteTreeNode != nullptr))
		{
			bPastedParentNode = true;

			// Select the newly pasted stuff
			CurrentGraphEditor->SetNodeSelection(PasteNode, true);

			PasteNode->NodePosX = (PasteNode->NodePosX - AvgNodePosition.X) + Location.X;
			PasteNode->NodePosY = (PasteNode->NodePosY - AvgNodePosition.Y) + Location.Y;

			PasteNode->SnapToGrid(16);

			const FGuid OldGuid = PasteNode->NodeGuid;

			// Give new node a different Guid from the old one
			PasteNode->CreateNewGuid();

			const FGuid NewGuid = PasteNode->NodeGuid;

			NewToOldNodeMapping.Add(NewGuid, OldGuid);
		}
	}

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UAbilityChainNodeEdGraphNode* PasteNode = Cast<UAbilityChainNodeEdGraphNode>(*It);
		if (PasteNode)
		{
			//PasteNode->NodePosX = 0;
			//PasteNode->NodePosY = 0;

			// remove subnode from graph, it will be referenced from parent node
			//PasteNode->DestroyNode();
		}
	}

	// Update UI
	CurrentGraphEditor->NotifyGraphChanged();

	UObject* GraphOwner = EdGraph->GetOuter();
	if (GraphOwner)
	{
		GraphOwner->PostEditChange();
		GraphOwner->MarkPackageDirty();
	}
}

bool FAbilityChainNodeEditor::CanPasteNodes() const
{
	const TSharedPtr<SGraphEditor> CurrentGraphEditor = GraphEditor;
	if (!CurrentGraphEditor.IsValid())
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(CurrentGraphEditor->GetCurrentGraph(), ClipboardContent);
}

void FAbilityChainNodeEditor::DuplicateNodes() const
{
	CopySelectedNodes();
	PasteNodes();
}

bool FAbilityChainNodeEditor::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void FAbilityChainNodeEditor::ClickToJson()
{
	UE_LOG(LogAbilityChainEditor, Log, TEXT("ClickToJson"));
	this->AbilityChainNodeAssetObject->EdGraph->Nodes;

	NodeJsonData JsonData;

	if (Cast<UAbilityChainNodeEdGraph>(this->AbilityChainNodeAssetObject->EdGraph)->RootNode)
	{
		GetAbilityChainNodeInfo(JsonData, Cast<UAbilityChainNodeEdGraph>(this->AbilityChainNodeAssetObject->EdGraph)->RootNode);
	}

	FString JsonStr = JsonData.ToJson();
	UE_LOG(LogAbilityChainEditor, Log, TEXT("AbilityChainNode.ToJson: %s"), *JsonStr);

	TArray<FString> Path; //选中文件路径
	const FString FileType = TEXT("JsonFile (*.json)|*.json"); //过滤文件类型
	const FString DefaultPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()); //文件选择窗口默认开启路径
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	const FString DefaultFileName = this->AbilityChainNodeAssetObject->GetName();
	const bool bSuccess = DesktopPlatform->SaveFileDialog(nullptr, TEXT("JsonExportDialog"), DefaultPath,
	                                                      DefaultFileName,
	                                                      *FileType, EFileDialogFlags::None, Path);
	if (bSuccess)
	{
		//文件选择成功，文件路径 path
		UE_LOG(LogAbilityChainEditor, Log, TEXT("SaveJsonPath: %s"), *Path[0]);

		if (FFileHelper::SaveStringToFile(JsonStr, *Path[0]))
		{
			UE_LOG(LogAbilityChainEditor, Log, TEXT("SaveJson OK"));
		}
		else
		{
			UE_LOG(LogAbilityChainEditor, Log, TEXT("SaveJson Fail"));
		}
	}
}

void FAbilityChainNodeEditor::ClickBuild()
{
	SaveAsset_Execute();
}

void FAbilityChainNodeEditor::OnSelectedNodesChanged(const TSet<UObject*>& NewSelection) const
{
	if (NewSelection.Num() == 1)
	{
		for (auto& element : NewSelection)
		{
			NodeDetailContent->SetObject(element);
		}
	}
	else if (NewSelection.Num() == 0)
	{
		NodeDetailContent->SetObject(nullptr);
	}
	//for (auto& Element : NewSelection)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("Print set for-in: %p"), Element);
	//}
}

void FAbilityChainNodeEditor::GetAbilityChainNodeInfo(NodeJsonData& StrArray, UAbilityChainNodeEdGraphNode* Node)
{
	StrArray.InputID = StaticEnum<EPrimaryInputID2>()->GetNameStringByValue(static_cast<int32>(Node->InputIDInfo));
	for (int i = 0; i < Node->AbilityArrayInfo.Num(); i++)
	{
		AbilityInfo info;
		info.Priority = Node->AbilityArrayInfo.Num() - i -1;
		FString PathName = "";
		if(NodeEdGraph->AbilityNameMap.Contains(Node->AbilityArrayInfo[i]))
		{
			//PathName = Node->AbilityNameMap[Node->AbilityArrayInfo[i]]->GetPathName();
			const auto OBJ = NodeEdGraph->AbilityNameMap[Node->AbilityArrayInfo[i]].Get();
			PathName = UKismetSystemLibrary::Conv_ObjectToSoftObjectReference(OBJ).ToString();
			PathName += "_C";
		}
		UE_LOG(LogAbilityChainEditor, Log, TEXT("SaveJsonPath: %s"), *PathName);
		info.AbilityClass = PathName;
		StrArray.AlternateAbilityList.Add(info);
	}
	for (const auto& i : Node->SubNodes)
	{
		NodeJsonData ChildrenData;
		StrArray.ChildNodes.Add(ChildrenData);
		const int32 Index = StrArray.ChildNodes.Num() - 1;
		GetAbilityChainNodeInfo(StrArray.ChildNodes[Index], i);
	}
}

FLinearColor FAbilityChainNodeEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::Blue;
}

FName FAbilityChainNodeEditor::GetToolkitFName() const
{
	return TEXT("AbilityChainNodeEditor");
}

FText FAbilityChainNodeEditor::GetToolkitName() const
{
	return FText::FromString(AbilityChainNodeAssetObject->GetName());
}

FText FAbilityChainNodeEditor::GetBaseToolkitName() const
{
	return FText::FromString(AbilityChainNodeAssetObject->GetName());
}

FText FAbilityChainNodeEditor::GetToolkitToolTipText() const
{
	return GetToolTipTextForObject(AbilityChainNodeAssetObject);
}

FString FAbilityChainNodeEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("AbilityChainNode");
}

void FAbilityChainNodeEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(
		FText::FromString("WorkspaceMenu_AbilityChainNodeEditor"));

	UAbilityChainNodeEdGraph* OBJ;
	if (this->AbilityChainNodeAssetObject->EdGraph)
	{
		OBJ = Cast<UAbilityChainNodeEdGraph>(this->AbilityChainNodeAssetObject->EdGraph);
		if(OBJ->RootNode)
		{
			OBJ->RsetNodePin(OBJ->RootNode);
			OBJ->AbilityDirectoryArray = AbilityChainNodeAssetObject->AbilityDirectoryArray;
		}
	}
	else
	{
		this->AbilityChainNodeAssetObject->EdGraph = Cast<UAbilityChainNodeEdGraph>(
			FBlueprintEditorUtils::CreateNewGraph(this->AbilityChainNodeAssetObject,
			                                      TEXT("AbilityChainNode"), UAbilityChainNodeEdGraph::StaticClass(),
			                                      UAbilityChainNodeEdGraphSchema::StaticClass()));

		OBJ = Cast<UAbilityChainNodeEdGraph>(this->AbilityChainNodeAssetObject->EdGraph);
		OBJ->RebuildGraph();
	}

	NodeEdGraph = OBJ;
	NodeEdGraph->UpdateAbilityAssetCache();
	NodeEdGraph->CheckNodeInfo();

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(
		this, &FAbilityChainNodeEditor::OnSelectedNodesChanged);


	this->GraphEditor = SNew(SGraphEditor)
	.AdditionalCommands(GraphEditorCommands)
	.GraphToEdit(OBJ)
	.GraphEvents(InEvents);

	auto CreateDetail = [this, InTabManager](const FSpawnTabArgs& Args)-> TSharedRef<SDockTab>
	{
		this->CreateDetailWidget(InTabManager);

		TSharedRef<SDockTab> DockTab = SNew(SDockTab)
			.Label(FText::FromString("AssetDetail"))
			[
				DetailContent.ToSharedRef()
			];

		return DockTab;
	};
	auto CreateNodeDetail = [this, InTabManager](const FSpawnTabArgs& Args)-> TSharedRef<SDockTab>
	{
		this->CreateNodeDetailWidget(InTabManager);

		TSharedRef<SDockTab> DockTab = SNew(SDockTab)
			.Label(FText::FromString("NodeDetail"))
			[
				NodeDetailContent.ToSharedRef()
			];

		return DockTab;
	};
	auto CreateViewport = [this](const FSpawnTabArgs& Args)-> TSharedRef<SDockTab>
	{
		TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				this->GraphEditor.ToSharedRef()
			]
		];

		return DockTab;
	};

	InTabManager->RegisterTabSpawner(EditorNodeDetailTabId, FOnSpawnTab::CreateLambda(CreateNodeDetail))
	            .SetDisplayName(FText::FromString("NodeDataPanel"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef()).SetMenuType(ETabSpawnerMenuType::Enabled);

	InTabManager->RegisterTabSpawner(EditorDetailTabId, FOnSpawnTab::CreateLambda(CreateDetail))
	            .SetDisplayName(FText::FromString("DataPanel"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef());

	InTabManager->RegisterTabSpawner(EditorViewPortTabId, FOnSpawnTab::CreateLambda(CreateViewport))
	            .SetDisplayName(FText::FromString("EditorViewport"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void FAbilityChainNodeEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	InTabManager->UnregisterTabSpawner(EditorDetailTabId);
	InTabManager->UnregisterTabSpawner(EditorViewPortTabId);
	InTabManager->UnregisterTabSpawner(EditorNodeDetailTabId);

	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

void FAbilityChainNodeEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
                                               FEditPropertyChain* PropertyThatChanged)
{
	UE_LOG(LogAbilityChainEditor, Log, TEXT("NotifyPostChange %s"), *PropertyChangedEvent.GetPropertyName().ToString());

	const auto CurrentNode = PropertyThatChanged->GetActiveMemberNode();

	if (CurrentNode)
	{
		const FProperty* Property = CurrentNode->GetValue();
		if (Property)
		{
			if(Property->GetName()=="AbilityDirectoryArray")
			{
				NodeEdGraph->AbilityDirectoryArray = AbilityChainNodeAssetObject->AbilityDirectoryArray;

				NodeEdGraph->UpdateAbilityAssetCache();
			}
			else
			{
				const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

				for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
				{
					const UAbilityChainNodeEdGraphNode* Node = Cast<UAbilityChainNodeEdGraphNode>(*NodeIt);
					if (Node)
					{
						Node->OnUpdateSubNode.Execute();
						//Node->AbilityClassPath = Node->AbilityArrayInfo[0];
					}
				}
			}
		}
	}
}

void FAbilityChainNodeEditor::OnClose()
{
}


//void FPathControllerEditor::Tick(float DeltaSeconds)
//{
//
//}
