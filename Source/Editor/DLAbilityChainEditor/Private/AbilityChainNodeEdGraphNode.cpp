#include "AbilityChainNodeEdGraphNode.h"
#include "AbilityChainNodeEdGraph.h"

UAbilityChainNodeEdGraphNode::UAbilityChainNodeEdGraphNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), InputIDInfo(), Graph(nullptr), ParentNode(nullptr), NodeInstance(nullptr),
	  OutputPin(nullptr),
	  InputPin(nullptr)
{
}

void UAbilityChainNodeEdGraphNode::AllocateDefaultPins()
{

	OutputPin = CreatePin(EEdGraphPinDirection::EGPD_Output, NAME_None, NAME_None);
	if(NodeType != EAbilityChainNodeType::RootNode)
	{
		InputPin = CreatePin(EEdGraphPinDirection::EGPD_Input, NAME_None, NAME_None);
	}
	//InputPin2 = CreatePin(EEdGraphPinDirection::EGPD_Input, NAME_None, NAME_None);
}

void UAbilityChainNodeEdGraphNode::PostCopyNode()
{

}

void UAbilityChainNodeEdGraphNode::NodeConnectionListChanged()
{
	Super::NodeConnectionListChanged();
}

void UAbilityChainNodeEdGraphNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if(Pin->Direction == EEdGraphPinDirection::EGPD_Output)
	{
		if ((Pin->LinkedTo.Num() > 0) && (Pin->GetOuter() == Pin->LinkedTo[0]->GetOuter()))
		{
			Pin->BreakLinkTo(Pin->LinkedTo[0]);
			return;
		}
		SubNodes.Empty();
		for (const auto& i : Pin->LinkedTo)
		{
			SubNodes.Add(Cast<UAbilityChainNodeEdGraphNode>(i->GetOuter()));
		}
	}
	else if (Pin->Direction == EEdGraphPinDirection::EGPD_Input)
	{
		if ((Pin->LinkedTo.Num() > 0) && (Pin->GetOuter() == Pin->LinkedTo[0]->GetOuter()))
		{
			Pin->BreakLinkTo(Pin->LinkedTo[0]);
			return;
		}
		if(Pin->LinkedTo.Num() > 0)
		{
			if (Pin->LinkedTo.Num() > 1)
			{
				Pin->BreakLinkTo(Pin->LinkedTo[0]);

			}
			ParentNode = Cast<UAbilityChainNodeEdGraphNode>(Pin->LinkedTo[0]->GetOuter());
		}
	}
}

void UAbilityChainNodeEdGraphNode::RemoveSubNode(UAbilityChainNodeEdGraphNode* Node)
{
	this->SubNodes.Remove(Node);
}

void UAbilityChainNodeEdGraphNode::FindNodePath(const FString& Str) const
{
	const auto Obj = Cast<UAbilityChainNodeEdGraph>(Graph)->AbilityNameMap.Find(FName(Str));
	if(Obj)
	{
		TArray<UObject*> Array;
		Array.Push(Obj->Get());
		GEditor->SyncBrowserToObjects(Array);
	}
}

TArray<FString> UAbilityChainNodeEdGraphNode::GetSelectOptions() const
{
	return Cast<UAbilityChainNodeEdGraph>(Graph)->GetNodeSelectOptions();
}
