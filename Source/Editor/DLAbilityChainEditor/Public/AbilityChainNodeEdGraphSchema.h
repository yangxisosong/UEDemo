#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "AbilityChainNodeSchemaActions.h"
#include "AbilityChainNodeEdGraphSchema.generated.h"


UCLASS()
class UAbilityChainNodeEdGraphSchema : public UEdGraphSchema
{
	GENERATED_UCLASS_BODY()
public:
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;

	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;

	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID,
		int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect,
		class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;


	static TSharedPtr<FAbilityChaiNodeSchemaAction_NewNode> AddNewNodeAction(FGraphActionListBuilderBase& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip);
};