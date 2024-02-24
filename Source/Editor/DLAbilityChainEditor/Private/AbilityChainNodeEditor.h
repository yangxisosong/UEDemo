#pragma once
#include "CoreMinimal.h"
#include "BlueprintEditor.h"
#include "IDLAbilityChainEditor.h"
#include "Serialization/JsonSerializerMacros.h"

class UAbilityChainAsset;
class UAbilityChainNodeEdGraphNode;
class UAbilityChainNodeEdGraph;

struct AbilityInfo : public FJsonSerializable
{
	int32 Priority = 0;
	FString AbilityClass = "";

public:
	BEGIN_JSON_SERIALIZER;
		JSON_SERIALIZE("Priority", Priority);
		JSON_SERIALIZE("AbilityClass", AbilityClass);
		END_JSON_SERIALIZER;
};

struct NodeJsonData
	: public FJsonSerializable
{
	FString InputID;
	TArray<NodeJsonData> ChildNodes;
	TArray<AbilityInfo> AlternateAbilityList;

public:
	BEGIN_JSON_SERIALIZER;
		JSON_SERIALIZE("InputID", InputID);
		JSON_SERIALIZE_ARRAY_SERIALIZABLE("AlternateAbilityList", AlternateAbilityList, AbilityInfo);
		JSON_SERIALIZE_ARRAY_SERIALIZABLE("ChildNodes", ChildNodes, NodeJsonData);
		END_JSON_SERIALIZER;
};


//自定义日志类型的声明
DECLARE_LOG_CATEGORY_EXTERN(LogAbilityChainEditor, Log, All);

class FAbilityChainNodeEditor
	: public IDLAbilityChainEditor
	  , public FWorkflowCentricApplication
	  , public FEditorUndoClient
	  , public FNotifyHook
	  , public FGCObject
{
protected:
#pragma region FGCObject

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObject(AbilityChainNodeAssetObject);
	}

	virtual FString GetReferencerName() const override
	{
		return TEXT("FAbilityChainNodeEditor");
	}

#pragma endregion

#pragma region ITreeNodeEditor

	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	virtual FName GetToolkitFName() const override;

	virtual FText GetToolkitName() const override;

	virtual FText GetBaseToolkitName() const override;

	virtual FText GetToolkitToolTipText() const override;

	virtual FString GetWorldCentricTabPrefix() const override;

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
	                              FProperty* PropertyThatChanged) override
	{
	}

	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
	                              FEditPropertyChain* PropertyThatChanged) override;

	virtual void NotifyPreChange(FProperty* PropertyAboutToChange) override
	{
	}

	virtual void NotifyPreChange(FEditPropertyChain* PropertyAboutToChange) override
	{
	}

	virtual void OnClose() override;

#pragma endregion

public:
	static const FName TreeNodeEditorAppIdentifier;

	UAbilityChainNodeEdGraph* NodeEdGraph;

public:
	FAbilityChainNodeEditor();

	void CreateCommandList();

	void AddButtonToToolBar();

	FGraphPanelSelectionSet GetSelectedNodes() const;

	struct FInitArg
	{
		UObject* Object = nullptr;
		TSharedPtr<class IToolkitHost> InitToolkitHost;
	};

	void InitTreeNodeEditor(const FInitArg& Arg);

	/*virtual void Tick(float DeltaSeconds) override;*/

private:
	void CreateDetailWidget(const TSharedRef<class FTabManager>& InTabManager);
	void CreateNodeDetailWidget(const TSharedRef<class FTabManager>& InTabManager);
protected:
	virtual void SaveAsset_Execute() override;

	void DeleteSelectedNodes() const;
	bool CanDeleteNodes() const;
	void DeleteSelectedDuplicatableNodes() const;
	void CutSelectedNodes() const;
	bool CanCutNodes() const;
	void CopySelectedNodes() const;
	bool CanCopyNodes() const;
	void PasteNodes() const;
	void PasteNodesHere(const FVector2D& Location) const;
	bool CanPasteNodes() const;
	void DuplicateNodes() const;
	bool CanDuplicateNodes() const;

	void ClickToJson();
	void ClickBuild();
	void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection) const;
	void GetAbilityChainNodeInfo(NodeJsonData& StrArray, UAbilityChainNodeEdGraphNode* Node);
private:
	UAbilityChainAsset* AbilityChainNodeAssetObject;

	TSharedPtr<IDetailsView> DetailContent;

	TSharedPtr<IDetailsView> NodeDetailContent;

	TSharedPtr<SGraphEditor> GraphEditor;

	TSharedPtr<FUICommandList> GraphEditorCommands;
};
