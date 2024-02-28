// Copyright Epic Games, Inc. All Rights Reserved.

#include "DLDebugWidget.h"

#include "Widgets/Images/SImage.h"
#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"


SDLDebugWidget::SDLDebugWidget()
{
}

SDLDebugWidget::~SDLDebugWidget()
{

}

void SDLDebugWidget::Construct(const FArguments& InArgs)
{
	OnSClicked = InArgs._OnSClicked;

	OnClassClicked = InArgs._OnClassClicked;

	OnBrowseWorld();
}

void SDLDebugWidget::OnBrowseWorld()
{
	// Remove all binding to an old world
	ChildSlot
		[
			SNullWidget::NullWidget
		];

	ChildSlot
		[
			SNew(SVerticalBox)

			// Toolbar
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(100)
				.HeightOverride(100)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(SImage)
				]

			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(100)
				.HeightOverride(100)
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				[
					SNew(SClassPropertyEntryBox)
					.AllowNone(true)
					.MetaClass(UObject::StaticClass())
					.SelectedClass(nullptr)
					.OnSetClass_Lambda([this](const UClass* Select)
					{
						this->OnClassClicked.Execute(Select);
					})
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(100)
				.HeightOverride(100)
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				[
					SNew(SButton)
					.OnClicked_Lambda([this]() {
						UE_LOG(LogTemp, Warning, TEXT("SCommand1Widget ClickButton"));

						return this->OnSClicked.Execute();
					})
				]
			]
		];

}


