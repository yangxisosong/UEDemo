// Copyright Epic Games, Inc. All Rights Reserved.

#include "SGraphNode_AbilityChainNode.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SToolTip.h"
#include "GraphEditorSettings.h"
#include "SGraphPanel.h"
#include "SCommentBubble.h"
#include "AbilityChainNodeEdGraphNode.h"
#include "AbilityChainNodeEditorStyle.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
//#include "GameAbilitySysDef.h"

#define LOCTEXT_NAMESPACE "BehaviorTreeEditor"

void SGraphNode_AbilityChainNode::Construct(const FArguments& InArgs, UAbilityChainNodeEdGraphNode* InNode)
{
	GraphNode = InNode;
	NodeType = InNode->NodeType;
	InNode->OnUpdateSubNode.BindLambda([this]()
	{
		this->UpdateSubNode();
	});

	Name = FText::FromString(InNode->TitleName);
	UpdateGraphNode();
}

TSharedPtr<SToolTip> SGraphNode_AbilityChainNode::GetComplexTooltip()
{
	//FString Str = "节点：类型";
	const FText TooltipText = Name;
	return SNew(SToolTip)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.Padding(2.0f)
		[
			SNew(STextBlock)
			.Text(TooltipText)
			.ColorAndOpacity(FSlateColor::UseSubduedForeground())
		]
	];
}

void SGraphNode_AbilityChainNode::UpdateGraphNode()
{
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	//TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode).Text(FText::FromString(Name));

	CenterArea = SNew(SVerticalBox);

	if (NodeType == EAbilityChainNodeType::RootNode)
	{
		LeftPinVisibility = EVisibility::Collapsed;
	}

	auto NormalFont = FStyleDefaults::GetFontInfo(20);

	NodeTitleText = SNew(STextBlock).Text(Name)
	                                .ColorAndOpacity(FLinearColor::White)
	                                .TextStyle(FAppStyle::Get(), TEXT("GraphBreadcrumbButtonText"));
	//.Font(NormalFont);

	UpdateSubNode();

	//控件界面：
	GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
				.Padding(0)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(50)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							NodeTitleText.ToSharedRef()
						]
					]
					+ SVerticalBox::Slot()
					  .AutoHeight()
					  .HAlign(HAlign_Fill)
					  .VAlign(VAlign_Top)
					[
						// NODE CONTENT AREA
						SNew(SBorder)
						.BorderImage(FAppStyle::GetBrush("NoBorder"))
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(FMargin(0, 3))
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							  .AutoWidth()
							  .VAlign(VAlign_Center)
							[
								// LEFT
								SNew(SBox)
								.WidthOverride(40)
								.Visibility(LeftPinVisibility)
								[
									SAssignNew(LeftNodeBox, SVerticalBox)
								]
							]

							+ SHorizontalBox::Slot()
							  .VAlign(VAlign_Fill)
							  .HAlign(HAlign_Fill)
							  .FillWidth(1.0f)
							[
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									SNew(SBox)
									.WidthOverride(100)
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Center)
									[
										SNew(STextBlock).Text(FText::FromString(""))
										                .ColorAndOpacity(FLinearColor::White)
									]
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									CenterArea.ToSharedRef()
								]

							]

							+ SHorizontalBox::Slot()
							  .AutoWidth()
							  .VAlign(VAlign_Center)
							[
								// RIGHT
								SNew(SBox)
								.WidthOverride(40)
								[
									SAssignNew(RightNodeBox, SVerticalBox)
								]
							]
						]
					]
				]
			]
		];

	CreatePinWidgets();
}

void SGraphNode_AbilityChainNode::UpdateSubNode()
{
	const auto Node = Cast<UAbilityChainNodeEdGraphNode>(GetNodeObj());
	if (Node)
	{
		this->Name = StaticEnum<EPrimaryInputID2>()->GetDisplayNameTextByIndex(static_cast<int32>(Node->InputIDInfo));

		if (NodeTitleText.IsValid() && Node->NodeType != EAbilityChainNodeType::RootNode)
		{
			NodeTitleText->SetText(this->Name);
		}

		CenterArea.Get()->ClearChildren();

		for (auto& i : Node->AbilityArrayInfo)
		{
			CenterArea.Get()->AddSlot()
			          .Padding(FMargin(0, 10, 0, 10))
			[
				CreatSubCard(i.ToString()).ToSharedRef()
			];
		}
	}
}

TSharedPtr<SWidget> SGraphNode_AbilityChainNode::CreatSubCard(const FString& Str)
{
	//Graph.Node.TitleBackground

	const auto brush = FAppStyle::GetBrush("Graph.Node.TitleBackground");

	FSlateFontInfo FontInfo;
	FontInfo.Size = 20;

	auto Icon = FSlateIcon(FAbilityChainNodeEditorStyle::GetStyleSetName(), 
		"AbilityChainNodeEditorStyle.FindNode");

	const auto NormalText = FStyleDefaults::GetFontInfo(15);
	//FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont"));
	//	.SetFont(DEFAULT_FONT("Regular", FCoreStyle::RegularTextSize))
	//	.SetColorAndOpacity(FSlateColor::UseForeground())
	//	.SetShadowOffset(FVector2D::ZeroVector)
	//	.SetShadowColorAndOpacity(FLinearColor::Black);

	return SNew(SBox)
		.WidthOverride(230)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SImage)
				.ColorAndOpacity(FLinearColor(FColor(193, 255, 193)))
				.Image(brush)
		]
		+ SOverlay::Slot()
		  .VAlign(VAlign_Center)
		  .HAlign(HAlign_Fill)
		[
			SNew(STextBlock).Text(FText::FromString(Str))
			                .Font(NormalText)
			                .ColorAndOpacity(FLinearColor::Black)
			                .WrapTextAt(220)
			                .AutoWrapText(false)
			                .WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
			                .Margin(FMargin(5, 5, 5, 15))
		]
		+ SOverlay::Slot()
		.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Right)
		[
			SNew(SBox)
			.WidthOverride(25)
			.HeightOverride(25)
			[
				SNew(SButton)
				.ContentPadding(FMargin(0,0,0,0))
				.OnClicked_Lambda([this, Str]()
				{
					this->ClickNode(Str);
					return FReply::Handled();
				})
				[
					SNew(SBox)
					.WidthOverride(25)
					.HeightOverride(25)
					[
						SNew(SImage)
						.Image(Icon.GetIcon())
					]
				]
			]
		]
	];
}

void SGraphNode_AbilityChainNode::ClickNode(const FString& Str) const
{
	const auto Node = Cast<UAbilityChainNodeEdGraphNode>(GetNodeObj());
	if (Node)
	{
		Node->FindNodePath(Str);
	}
}


#undef LOCTEXT_NAMESPACE
