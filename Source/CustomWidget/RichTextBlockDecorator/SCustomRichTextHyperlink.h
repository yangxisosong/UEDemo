// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SlateGlobals.h"
#include "Input/Reply.h"
#include "Layout/Margin.h"
#include "Styling/SlateTypes.h"
#include "Framework/Text/SlateHyperlinkRun.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/SHyperlink.h"

class FWidgetViewModel;
enum class ETextShapingMethod : uint8;
DECLARE_DELEGATE_OneParam(FHoveredDelegate, bool);

class SCustomRichTextHyperlink : public SHyperlink
{
public:
	SLATE_BEGIN_ARGS(SCustomRichTextHyperlink)
		: _Text()
		, _Style(&FCoreStyle::Get().GetWidgetStyle< FHyperlinkStyle >("Hyperlink"))
		, _OnNavigate()
		, _OnNavigateUrl()
		, _OnHovered()
		, _Url()
		, _TextShapingMethod()
		, _TextFlowDirection()
	{}
		SLATE_ATTRIBUTE( FText, Text )
		SLATE_STYLE_ARGUMENT( FHyperlinkStyle, Style )
		SLATE_EVENT(FSimpleDelegate, OnNavigate)
		SLATE_EVENT(FOnTextChanged, OnNavigateUrl)
		SLATE_EVENT(FHoveredDelegate, OnHovered)
		SLATE_ATTRIBUTE(FText, Url)
		SLATE_ARGUMENT( TOptional<ETextShapingMethod>, TextShapingMethod )
		SLATE_ARGUMENT( TOptional<ETextFlowDirection>, TextFlowDirection )
	SLATE_END_ARGS()

public:

	void Construct( const FArguments& InArgs, const TSharedRef< FSlateHyperlinkRun::FWidgetViewModel >& InViewModel )
	{
		ViewModel = InViewModel;
		Url = InArgs._Url;
		OnNavigateUrlCallback = InArgs._OnNavigateUrl;
		HoveredCallback = InArgs._OnHovered;

		SHyperlink::Construct(
			SHyperlink::FArguments()
			.Text( InArgs._Text )
			.Style( InArgs._Style )
			.Padding( FMargin(0))
			.OnNavigate(InArgs._OnNavigate)
			.TextShapingMethod( InArgs._TextShapingMethod )
			.TextFlowDirection( InArgs._TextFlowDirection )
		);
	}
	//void bindevent(FCustomRichTextDecorator* obj)
	//{
	//	obj->GetSetHovered().AddUObject(this, &SCustomRichTextHyperlink::SetHovered);
	//}

	virtual void OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override
	{
		OnHovered(true);
		SHyperlink::OnMouseEnter( MyGeometry, MouseEvent );
		ViewModel->SetIsHovered( true );
	}

	virtual void OnMouseLeave( const FPointerEvent& MouseEvent ) override
	{
		OnHovered(false);
		SHyperlink::OnMouseLeave( MouseEvent );
		ViewModel->SetIsHovered( false );
	}

	virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override
	{
		FReply Reply = SHyperlink::OnMouseButtonDown( MyGeometry, MouseEvent );
		ViewModel->SetIsPressed( bIsPressed );

		return Reply;
	}

	virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override
	{
		FReply Reply = SHyperlink::OnMouseButtonUp( MyGeometry, MouseEvent );
		ViewModel->SetIsPressed( bIsPressed );
		OnNavigateUrl(Url.Get());

		return Reply;
	}

	void OnNavigateUrl(const FText& InText)
	{
		OnNavigateUrlCallback.ExecuteIfBound(InText);
	}

	void SetHovered(bool ishovered)
	{
		ViewModel->SetIsHovered(ishovered);
	}

	void OnHovered(bool ishovered)
	{
		HoveredCallback.ExecuteIfBound(ishovered);
	}

	virtual bool IsHovered() const override
	{
		return ViewModel->IsHovered();
	}

	virtual bool IsPressed() const override
	{
		return ViewModel->IsPressed();
	}


protected:
	FOnTextChanged OnNavigateUrlCallback;
	FHoveredDelegate HoveredCallback;

	TAttribute< FText > Url;
private:

	TSharedPtr< FSlateHyperlinkRun::FWidgetViewModel > ViewModel;
};
