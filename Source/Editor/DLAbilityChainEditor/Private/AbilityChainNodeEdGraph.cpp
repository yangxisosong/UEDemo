#include "AbilityChainNodeEdGraph.h"
#include "AbilityChainNodeEdGraphNode.h"
#include "EditorAssetLibrary.h"
#include "SGraphNode.h"
#include "SGraphPanel.h"
#include "AbilityChainNodeEditor.h"

UAbilityChainNodeEdGraph::UAbilityChainNodeEdGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), RootNode(nullptr)
{
}

void UAbilityChainNodeEdGraph::RebuildGraph()
{
	UAbilityChainNodeEdGraphNode* node1= CreateGraphRootNode();
	node1->TitleName = "Root";
	RootNode = node1;
}


namespace AutoArrangeHelpers
{
	struct FNodeBoundsInfo
	{
		FVector2D SubGraphBBox;
		TArray<FNodeBoundsInfo> Children;
	};

	void AutoArrangeNodes(UAbilityChainNodeEdGraphNode* ParentNode, FNodeBoundsInfo& BBoxTree, float PosX, float PosY)
	{
		SGraphNode::FNodeSet ParentNodeFilter;
		ParentNode->DEPRECATED_NodeWidget.Pin()->MoveTo(FVector2D(PosX, PosY), ParentNodeFilter);

		if (ParentNode->SubNodes.Num()>0)
		{
			SGraphNode::FNodeSet NodeFilter;;

			for (int32 Idx = 0; Idx < ParentNode->SubNodes.Num(); Idx++)
			{
				UAbilityChainNodeEdGraphNode* GraphNode = Cast<UAbilityChainNodeEdGraphNode>(ParentNode->SubNodes[Idx]);
				if (GraphNode && BBoxTree.Children.Num() > 0)
				{
					const float ChildrenY = ParentNode->DEPRECATED_NodeWidget.Pin()->GetPosition().Y+
						(ParentNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().Y-50)/2+50-
						(GraphNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().Y-50)/2-50 + 
						Idx * 200;

					const float ChildrenX = ParentNode->DEPRECATED_NodeWidget.Pin()->GetPosition().X +
						ParentNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().X / 2 +
						GraphNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().X + 50;

					AutoArrangeNodes(GraphNode, BBoxTree.Children[Idx], ChildrenX, ChildrenY);
				}

			}
		}
	}

	struct FCompareNodeXLocation
	{
		FORCEINLINE bool operator()(const UAbilityChainNodeEdGraphNode& NodeA, const UAbilityChainNodeEdGraphNode& NodeB) const
		{
			//const UEdGraphNode NodeA = A;
			//const UEdGraphNode NodeB = B;

			if (NodeA.NodePosX == NodeB.NodePosX)
			{
				return NodeA.NodePosY < NodeB.NodePosY;
			}

			return NodeA.NodePosY < NodeB.NodePosY;
		}
	};


	void GetNodeSizeInfo(UAbilityChainNodeEdGraphNode* ParentNode, FNodeBoundsInfo& BBoxTree)
	{
		BBoxTree.SubGraphBBox = ParentNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize();
		if (ParentNode->SubNodes.Num()>0)
		{
			float LevelHeight = 0;
			float LevelWidth = 0;
			ParentNode->SubNodes.Sort(FCompareNodeXLocation());
			for (int32 Idx = 0; Idx < ParentNode->SubNodes.Num(); Idx++)
			{
				UAbilityChainNodeEdGraphNode* GraphNode = Cast<UAbilityChainNodeEdGraphNode>(ParentNode->SubNodes[Idx]);
				if (GraphNode)
				{
					const int32 ChildIdx = BBoxTree.Children.Add(FNodeBoundsInfo());
					FNodeBoundsInfo& ChildBounds = BBoxTree.Children[ChildIdx];

					GetNodeSizeInfo(GraphNode, ChildBounds);

					LevelWidth += ChildBounds.SubGraphBBox.X;
					if (ChildBounds.SubGraphBBox.Y > LevelHeight)
					{
						LevelHeight = ChildBounds.SubGraphBBox.Y;
					}
				}
			}

			if (LevelWidth > BBoxTree.SubGraphBBox.X)
			{
				BBoxTree.SubGraphBBox.X = LevelWidth;
			}

			BBoxTree.SubGraphBBox.Y += LevelHeight;
		}
	}
}

void UAbilityChainNodeEdGraph::AutoArrange() const
{
	if (!RootNode)
	{
		return;
	}

	AutoArrangeHelpers::FNodeBoundsInfo BBoxTree;
	AutoArrangeHelpers::GetNodeSizeInfo(RootNode, BBoxTree);
	AutoArrangeHelpers::AutoArrangeNodes(RootNode, BBoxTree, 0,0);

	RootNode->NodePosX = BBoxTree.SubGraphBBox.X / 2 - RootNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().X / 2;
	RootNode->NodePosY = 0;

	RootNode->DEPRECATED_NodeWidget.Pin()->GetOwnerPanel()->ZoomToFit(/*bOnlySelection=*/ false);
}

void UAbilityChainNodeEdGraph::RsetNodePin(UAbilityChainNodeEdGraphNode* Node)
{
	auto Pins = Node->GetAllPins();
	for (const auto& i : Pins)
	{
		if(i->Direction== EEdGraphPinDirection::EGPD_Input)
		{
			Node->InputPin = i;
		}
		else if(i->Direction == EEdGraphPinDirection::EGPD_Output)
		{
			Node->OutputPin = i;
		}
	}

	for (const auto& j : Node->SubNodes)
	{
		RsetNodePin(j);
	}
}

UAbilityChainNodeEdGraphNode* UAbilityChainNodeEdGraph::CreateGraphNode()
{
	FGraphNodeCreator<UAbilityChainNodeEdGraphNode> NodeCreator(*this);

	UAbilityChainNodeEdGraphNode* Node = NodeCreator.CreateNode();
	Node->Graph = this;
	NodeCreator.Finalize();
	return Node;
}

UAbilityChainNodeEdGraphNode* UAbilityChainNodeEdGraph::CreateGraphRootNode()
{
	FGraphNodeCreator<UAbilityChainNodeEdGraphNode> NodeCreator(*this);

	UAbilityChainNodeEdGraphNode* Node = NodeCreator.CreateNode();
	Node->Graph = this;
	Node->NodeType = EAbilityChainNodeType::RootNode;
	NodeCreator.Finalize();
	return Node;
}

void UAbilityChainNodeEdGraph::UpdateAbilityAssetCache()
{
	TArray<FString> Array;
	AbilityNameMap.Empty();
	for (const auto& i : AbilityDirectoryArray)
	{
		TArray<FAssetData> AssetDataList;
		auto AssetList = UEditorAssetLibrary::ListAssets(i.Path);
		for (auto AssetPath : AssetList)
		{
			AssetDataList.Add(UEditorAssetLibrary::FindAssetData(AssetPath));
		}

		for (const auto& j : AssetDataList)
		{
			if (j.AssetClass != FName("Blueprint"))
			{
				continue;
			}
			if (!j.AssetName.ToString().StartsWith("GA"))
			{
				continue;
			}
			const auto Aset = j.GetAsset();
			if (UBlueprint* BP = Cast<UBlueprint>(Aset))
			{
				const bool OK = false;//BP->GeneratedClass->IsChildOf<UDLGameplayAbilityBase>();

				if (OK)
				{
					AbilityNameMap.Add(BP->GetFName(), MakeWeakObjectPtr<UObject>(BP));
					Array.Add(BP->GetName());
				}
			}
		}
	}

	NodeSelectCache = Array;

	if (Array.Num() == 0)
	{
		UE_LOG(LogAbilityChainEditor, Warning, TEXT("Find Asset Is None From Path"));
	}
}

void UAbilityChainNodeEdGraph::CheckNodeInfo()
{
	for(const auto& i: Nodes)
	{
		TArray<FName> CheckArray;

		for(auto& j:Cast<UAbilityChainNodeEdGraphNode>(i)->AbilityArrayInfo)
		{
			if(AbilityNameMap.Num()==0)
			{
				UE_LOG(LogAbilityChainEditor, Warning, TEXT("AbilityNameMap Is None"));
			}
			if(!AbilityNameMap.Contains(j))
			{
				j = FName("None");
			}
		}
	}
}

TArray<FString> UAbilityChainNodeEdGraph::GetNodeSelectOptions()
{
	return NodeSelectCache;
}
