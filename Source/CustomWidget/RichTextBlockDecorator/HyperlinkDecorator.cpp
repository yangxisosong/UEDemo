// Fill out your copyright notice in the Description page of Project Settings.

#include "HyperlinkDecorator.h"
#include <Runtime/Slate/Public/Framework/Text/SlateHyperlinkRun.h>
#include <Runtime/Slate/Public/Widgets/Input/SRichTextHyperlink.h>


class FRichInlineHyperLinkText : public FRichTextDecorator
{
public:
	FRichInlineHyperLinkText(URichTextBlock* InOnwer, UHyperlinkDecorator* InDecorator)
		:FRichTextDecorator(InOnwer)
		, Decorator(InDecorator)
	{
		del_.BindLambda([=]() {
			Decorator->ClickFunc();
		});
	}

	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		if (RunParseResult.Name == Decorator->HlName && RunParseResult.MetaData.Contains(TEXT("id")))
		{
			const FTextRange& IdRange = RunParseResult.MetaData[TEXT("id")];
			const FString TagId = Text.Mid(IdRange.BeginIndex, IdRange.EndIndex - IdRange.BeginIndex);

			const bool bWarnIfMissing = false;
			return Decorator->FindHyperLinkBrush(*TagId, bWarnIfMissing) != nullptr;
		}

		return false;
	}

protected:
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& DefaultTextStyle) const override
	{
		const bool bWarnIfMissing = true;
		const FHyperlinkStyle* HlStyle = Decorator->FindHyperLinkBrush(*RunInfo.MetaData[TEXT("id")], bWarnIfMissing);

		TSharedPtr<FSlateHyperlinkRun::FWidgetViewModel> model = MakeShareable(new FSlateHyperlinkRun::FWidgetViewModel);

		const FString* url = RunInfo.MetaData.Find(TEXT("href"));

		FText text_ = FText::FromString(TEXT("no href!"));

		if (url)
		{
			text_ = FText::FromString(*url);
			Decorator->SetHyperLinkContent(url);
		}

		return SNew(SRichTextHyperlink, model.ToSharedRef())
			.Text(text_)
			.Style(HlStyle)
			.OnNavigate(del_);
	}

private:
	UHyperlinkDecorator* Decorator;
	FSimpleDelegate del_;
};

UHyperlinkDecorator::UHyperlinkDecorator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TSharedPtr<ITextDecorator> UHyperlinkDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShareable(new FRichInlineHyperLinkText(InOwner, this));
}

const FHyperlinkStyle* UHyperlinkDecorator::FindHyperLinkBrush(FName TagOrId, bool bWarnIfMissing)
{
	const FRichHlinkRow* HlinkRow = FindHlinkRow(TagOrId, bWarnIfMissing);
	if (HlinkSet)
	{
		return &HlinkRow->HlinkStyle;
	}
	
	return nullptr;
}

void UHyperlinkDecorator::SetHyperLinkContent(const FString* str)
{
	HlContent = *str;
}

FRichHlinkRow* UHyperlinkDecorator::FindHlinkRow(FName TagOrId, bool bWarningIfMissing)
{
	if (HlinkSet)
	{
		FString ContextString;
		return HlinkSet->FindRow<FRichHlinkRow>(TagOrId, ContextString, bWarningIfMissing);
	}

	return nullptr;
}