#include "AbilityChainNodeEdGraphSchema.h"

#include "AbilityChainConnectionDrawingPolicy.h"
#include "AbilityChainAsset2.h"
#include "GraphEditorActions.h"
#include "ToolMenu.h"
#include "Slate/Public/Framework/Commands/GenericCommands.h"
#include "Developer/ToolMenus/Public/ToolMenuSection.h"

#define LOCTEXT_NAMESPACE "TreeNodeEdGraphSchema"

UAbilityChainNodeEdGraphSchema::UAbilityChainNodeEdGraphSchema(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

const FPinConnectionResponse UAbilityChainNodeEdGraphSchema::CanCreateConnection(const UEdGraphPin* A,
	const UEdGraphPin* B) const
{
	if (A->GetOuter() == B->GetOuter())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("节点不能首尾相连"));
	}
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("OK"));
}

void UAbilityChainNodeEdGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	//TSharedPtr<FTreeNodeActions_Test> NewAction(new FTreeNodeActions_Test());
	//ContextMenuBuilder.AddAction(NewAction);
	//FCategorizedGraphActionListBuilder CompositesBuilder(TEXT("NodeType"));

	for (const EPrimaryInputID2 Thing : TEnumRange<EPrimaryInputID2>())
	{
		if((Thing == EPrimaryInputID2::None)||(Thing == EPrimaryInputID2::DebugKeyBegin))
		{
			continue;
		}

		auto InputName = StaticEnum<EPrimaryInputID2>()->GetDisplayNameTextByIndex(static_cast<int32>(Thing));

		//获取 UEnum 对象：
		//const UEnum* EnumObject = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPrimaryInputID2"));
		//获取对应字符串：
		//const FName EnumName = (EnumObject->GetNameByValue());

		FString name = "Name:"+InputName.ToString();

		AddNewNodeAction(ContextMenuBuilder, FText::GetEmpty(), InputName,
		FText::FromString(name));
	};
	//ContextMenuBuilder.Append(CompositesBuilder);
}

void UAbilityChainNodeEdGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (Context->Node)
	{
		{
			FToolMenuSection& Section = Menu->AddSection("TreeNodeGraphSchemaNodeActions", LOCTEXT("ClassActionsMenuHeader", "Node Actions"));
			Section.AddMenuEntry(FGenericCommands::Get().Delete);
			Section.AddMenuEntry(FGenericCommands::Get().Cut);
			Section.AddMenuEntry(FGenericCommands::Get().Copy);
			Section.AddMenuEntry(FGenericCommands::Get().Duplicate);

			Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
		}
	}

	Super::GetContextMenuActions(Menu, Context);
}

FLinearColor UAbilityChainNodeEdGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::White;
}

FConnectionDrawingPolicy* UAbilityChainNodeEdGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID,
	int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
	UEdGraph* InGraphObj) const
{
	return new FAbilityChainConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

TSharedPtr<FAbilityChaiNodeSchemaAction_NewNode> UAbilityChainNodeEdGraphSchema::AddNewNodeAction(
	FGraphActionListBuilderBase& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip)
{
	TSharedPtr<FAbilityChaiNodeSchemaAction_NewNode> NewAction =
		MakeShared<FAbilityChaiNodeSchemaAction_NewNode>(
			Category, MenuDesc, Tooltip, 0);
	ContextMenuBuilder.AddAction(NewAction);

	return NewAction;
}

#undef LOCTEXT_NAMESPACE 