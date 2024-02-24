// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SGraphNode.h"
#include "AbilityChainNodeEdGraphNode.h"

class SHorizontalBox;
class SToolTip;
class SVerticalBox;
class UBehaviorTreeGraphNode;

struct FNodeBounds
{
	FVector2D Position;
	FVector2D Size;

	FNodeBounds(FVector2D InPos, FVector2D InSize)
	{
		Position = InPos;
		Size = InSize;
	}
};
struct FTreeNodeBounds
{
	FVector2D Position;
	FVector2D Size;

	FTreeNodeBounds(FVector2D InPos, FVector2D InSize)
	{
		Position = InPos;
		Size = InSize;
	}
};
class SGraphNode_AbilityChainNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_AbilityChainNode){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UAbilityChainNodeEdGraphNode* InNode);

	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;

	// SGraphNode interface
	virtual void UpdateGraphNode() override;

	void UpdateSubNode();

private:
	TSharedPtr<SWidget> CreatSubCard(const FString& Str);

	void ClickNode(const FString& Str) const;
protected:
	FText Name;

	TSharedPtr<SVerticalBox>  CenterArea;

	EVisibility LeftPinVisibility = EVisibility::SelfHitTestInvisible;

	EAbilityChainNodeType NodeType = EAbilityChainNodeType::None;

	TSharedPtr<STextBlock> NodeTitleText;
	
};
