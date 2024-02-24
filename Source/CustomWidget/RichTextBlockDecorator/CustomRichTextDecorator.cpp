#include "CustomRichTextDecorator.h"
#include <Components/RichTextBlock.h>
#include "Widgets/Layout/SBox.h"

#include "CustomRichTextBlock.h"

FCustomRichTextDecorator::FCustomRichTextDecorator(URichTextBlock* InOwner)
	: FRichTextDecorator(InOwner)
{
	MyStyle = InOwner->GetDefaultTextStyle();
	UCustomRichTextBlock* CustomOwner = Cast<UCustomRichTextBlock>(InOwner);
	
	m_wrapTextAt = InOwner->GetWrapTextAt();
	linkstyle = CustomOwner->CustomHyperLinktStyle;
	BoldFont = CustomOwner->CustomBoldFont;
	OnClickLink.BindLambda([=](FText Url) {
		CustomOwner->OnClickLink.Broadcast(Url.ToString());
	});

	OnHovered.BindLambda([this](bool ishovered) {
		this->m_OnSetHovered.Broadcast(ishovered);
		if (ishovered)
		{
			UE_LOG(LogTemp, Warning, TEXT("FCustomRichTextDecorator SetHovered %d"), 1);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FCustomRichTextDecorator SetHovered %d"), 0);
		}

		//testobj.Pin().Get()->SetHovered(ishovered);
	/*	for (auto& i :richlink)
		{
			i->SetHovered(ishovered);
		}*/
	});

}

FCustomRichTextDecorator::~FCustomRichTextDecorator()
{
	//richlink.Empty();
}

bool FCustomRichTextDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const

{
	bool bIsCreateWidget = false;
	if (RunParseResult.Name == TEXT("rich"))
	{
		/*
		if (RunParseResult.MetaData.Contains(TEXT("bold")))
		{
			const FTextRange& IdRange = RunParseResult.MetaData[TEXT("bold")];
			customBold = true;
		}
		if (RunParseResult.MetaData.Contains(TEXT("color")))
		{
			const FTextRange& IdRange = RunParseResult.MetaData[TEXT("color")];
			FString colorstr = Text.Mid(IdRange.BeginIndex, IdRange.EndIndex - IdRange.BeginIndex);
			fontcolor = FColor::FromHex(colorstr);
			fontcolorOverride = true;
		}
		if (RunParseResult.MetaData.Contains(TEXT("size")))
		{
			const FTextRange& IdRange = RunParseResult.MetaData[TEXT("size")];
			FString sizestr = Text.Mid(IdRange.BeginIndex, IdRange.EndIndex - IdRange.BeginIndex);
			fontsize = FCString::Atoi(*sizestr);
			fontsizeOverride = true;
		}*/
		if (RunParseResult.MetaData.Contains(TEXT("href")))
		{
			const FTextRange& IdRange = RunParseResult.MetaData[TEXT("href")];
			url = Text.Mid(IdRange.BeginIndex, IdRange.EndIndex - IdRange.BeginIndex);
			isUrl = true;
			bIsCreateWidget = true;
		}
		FString contentstr = Text.Mid(RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
		textcontent = FText::FromString(contentstr);

		if (RunParseResult.MetaData.Contains(TEXT("newline")))
		{
			contentstr = TEXT("\n");
			textcontent = FText::FromString(contentstr);
		}

		return bIsCreateWidget;
	}
	return false;
}

float FCustomRichTextDecorator::GetWrapTextAt()
{
	return m_wrapTextAt;
}

TSharedPtr<SWidget> FCustomRichTextDecorator::CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const
{
	/*
	//重载粗体
	if (customBold)
	{
		MyStyle.SetFont(BoldFont);
	}
	else
	{
		MyStyle.SetFont(TextStyle.Font);
	}

	//重载字体颜色
	if (fontcolorOverride)
	{
		FSlateColor color(fontcolor);
		MyStyle.SetColorAndOpacity(color);
	}
	else
	{
		MyStyle.SetColorAndOpacity(TextStyle.ColorAndOpacity);
	}
	//重载字体大小
	if (fontsizeOverride)
	{
		MyStyle.SetFontSize(fontsize);	
	}
	else
	{
		MyStyle.SetFontSize(TextStyle.Font.Size);
	}
	*/
	//重载为URL
	if (isUrl)
	{
		TSharedPtr<FSlateHyperlinkRun::FWidgetViewModel> model = MakeShareable(new FSlateHyperlinkRun::FWidgetViewModel);

		TSharedPtr<SCustomRichTextHyperlink> link = SNew(SCustomRichTextHyperlink, model.ToSharedRef())
			.Text(this->textcontent)
			.Style(&linkstyle)
			.OnNavigate(del)
			.OnNavigateUrl(OnClickLink)
			.OnHovered(OnHovered)
			.Url(FText::FromString(url));
		


		return link;
	}
	/*
	FString str = this->textcontent.ToString();
	TSharedPtr<STextBlock> mytext = SNew(STextBlock)
		.Text(this->textcontent)
		.TextStyle(&this->MyStyle);

	customBold = false;
	fontcolorOverride = false;
	fontsizeOverride = false;
	isUrl = false;

	return mytext;
	*/
	TSharedPtr<SBox> NullBox = SNew(SBox);
	customBold = false;
	fontcolorOverride = false;
	fontsizeOverride = false;
	isUrl = false;

	return NullBox;
}


//////////////////////////////////////////////////////////////////////////

TSharedPtr<ITextDecorator> UCustomRichTextBlockDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShareable(new FCustomRichTextDecorator(InOwner));
}





UCustomRichTextBlockDecorator::UCustomRichTextBlockDecorator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}