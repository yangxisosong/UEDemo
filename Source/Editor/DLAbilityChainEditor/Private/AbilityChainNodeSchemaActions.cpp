#include "AbilityChainNodeSchemaActions.h"
#include "AbilityChainNodeEdGraph.h"
#include "AbilityChainNodeEdGraphNode.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "TreeNodeActions"

UEdGraphNode* FTreeNodeActions_Test::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const UAbilityChainNodeEdGraph* Graph = Cast<UAbilityChainNodeEdGraph>(ParentGraph);
	if (Graph)
	{
		Graph->AutoArrange();
	}
	return nullptr;
}

UEdGraphNode* FAbilityChaiNodeSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	// If there is a template, we actually use it
	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("AddNode", "Add Node"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		NodeTemplate->SetFlags(RF_Transactional);

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(nullptr, ParentGraph, REN_NonTransactional);
		ParentGraph->AddNode(NodeTemplate, true);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();

		// For input pins, new node will generally overlap node being dragged off
		// Work out if we want to visually push away from connected node
		int32 XLocation = Location.X;
		if (FromPin && FromPin->Direction == EGPD_Input)
		{
			const UEdGraphNode* PinNode = FromPin->GetOwningNode();
			const float XDelta = FMath::Abs(PinNode->NodePosX - Location.X);

			if (XDelta < NodeDistance)
			{
				// Set location to edge of current node minus the max move distance
				// to force node to push off from connect node enough to give selection handle
				XLocation = PinNode->NodePosX - NodeDistance;
			}
		}

		NodeTemplate->NodePosX = XLocation;
		NodeTemplate->NodePosY = Location.Y;
		NodeTemplate->SnapToGrid(100);

		// setup pins after placing node in correct spot, since pin sorting will happen as soon as link connection change occurs
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

UEdGraphNode* FAbilityChaiNodeSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
	const FVector2D Location, bool bSelectNewNode)
{
	UAbilityChainNodeEdGraph* Graph = Cast<UAbilityChainNodeEdGraph>(ParentGraph);

	//创建新节点
	UAbilityChainNodeEdGraphNode* NewNode = Graph->CreateGraphNode();
	//设置信息
	NewNode->TitleName = MenuDescriptionArray[0];

	for (const EPrimaryInputID2 Thing : TEnumRange<EPrimaryInputID2>())
	{
		auto InputName = StaticEnum<EPrimaryInputID2>()->GetDisplayNameTextByIndex(static_cast<int32>(Thing));
		if (InputName.ToString() == GetMenuDescription().ToString())
		{
			NewNode->InputIDInfo = Thing;
		}
	};

	//设置节点的位置
	NewNode->NodePosX = Location.X;
	NewNode->NodePosY = Location.Y;

	if (FromPins.Num() > 0)//如果有引脚则连接
	{
		Graph->GetSchema()->TryCreateConnection(FromPins[0], NewNode->InputPin);
	};
	return nullptr;
}

void FAbilityChaiNodeSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
}

#undef LOCTEXT_NAMESPACE