#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "AbilityChainNodeSchemaActions.generated.h"


USTRUCT()
struct FTreeNodeActions_Test : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY()

public:

	FTreeNodeActions_Test() :FEdGraphSchemaAction(
		FText::FromString("category_test")			//InNodeCategory
		, FText::FromString("test")					//InMenuDesc
		, FText::FromString("test_tooltip")			//InToolTip
		, 0	//InGrouping:/** This is a priority number for overriding alphabetical order in the action list (higher value  == higher in the list). */					
	) {};

	// FEdGraphSchemaAction interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	// End of FEdGraphSchemaAction interface
};

/** Action to add a node to the graph */
USTRUCT()
struct FAbilityChaiNodeSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	/** Template of node we want to create */
	UPROPERTY()
		class UAbilityChainNodeEdGraphNode* NodeTemplate;

	int32 NodeDistance = 60;

	FAbilityChaiNodeSchemaAction_NewNode()
		: FEdGraphSchemaAction()
		, NodeTemplate(nullptr)
	{}

	FAbilityChaiNodeSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
		, NodeTemplate(nullptr)
	{}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	//~ End FEdGraphSchemaAction Interface

	template <typename NodeType>
	static NodeType* SpawnNodeFromTemplate(class UEdGraph* ParentGraph, NodeType* InTemplateNode, const FVector2D Location)
	{
		FAbilityChaiNodeSchemaAction_NewNode Action;
		Action.NodeTemplate = InTemplateNode;

		return Cast<NodeType>(Action.PerformAction(ParentGraph, nullptr, Location));
	}
};