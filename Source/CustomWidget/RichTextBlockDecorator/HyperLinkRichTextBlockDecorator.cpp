#include "HyperLinkRichTextBlockDecorator.h"
#include "Engine/Engine.h"
#include "Framework/Text/SlateHyperlinkRun.h"
#include "Widgets/Input/SRichTextHyperlink.h"

FRichInlineHyperLinkDecorator::FRichInlineHyperLinkDecorator(URichTextBlock* InOwner, UHyperLinkRichTextBlockDecorator* decorator)
	: FRichTextDecorator(InOwner)
{
	linkstyle = decorator->style;
	del.BindLambda([=]() {
		decorator->ClickFun();
	});
}

bool FRichInlineHyperLinkDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const

{
	if (RunParseResult.Name == TEXT("a") && RunParseResult.MetaData.Contains(TEXT("id")))
	{
		const FTextRange& IdRange = RunParseResult.MetaData[TEXT("id")];
		idString = Text.Mid(IdRange.BeginIndex, IdRange.EndIndex - IdRange.BeginIndex);
		return true;
	}
	return false;
}

TSharedPtr<SWidget> FRichInlineHyperLinkDecorator::CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const
{
	const bool bWarnIfMissing = true;

	TSharedPtr<FSlateHyperlinkRun::FWidgetViewModel> model = MakeShareable(new FSlateHyperlinkRun::FWidgetViewModel);

	TSharedPtr<SRichTextHyperlink> link = SNew(SRichTextHyperlink, model.ToSharedRef())
		.Text(FText::FromString("link"))
		.Style(&linkstyle)
		.OnNavigate(del);

	return link;
}

//////////////////////////////////////////////////////////////////////////

TSharedPtr<ITextDecorator> UHyperLinkRichTextBlockDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShareable(new FRichInlineHyperLinkDecorator(InOwner, this));
}

void UHyperLinkRichTextBlockDecorator::ClickFun_Implementation()
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, "click link");
	}
}

UHyperLinkRichTextBlockDecorator::UHyperLinkRichTextBlockDecorator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}