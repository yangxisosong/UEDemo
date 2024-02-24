// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Text/RichTextLayoutMarshaller.h"
#include "Framework/Text/ITextDecorator.h"



class FRichTextLayoutMarshaller;

/**
 * 
 */
class FCustomRichTextLayoutMarshaller : public FRichTextLayoutMarshaller
{
public:

	static TSharedRef< FCustomRichTextLayoutMarshaller > CreateCustom(TArray< TSharedRef< ITextDecorator > > InDecorators, const ISlateStyle* const InDecoratorStyleSet);

	static TSharedRef< FCustomRichTextLayoutMarshaller > CreateCustom(TSharedPtr< IRichTextMarkupParser > InParser, TSharedPtr< IRichTextMarkupWriter > InWriter, TArray< TSharedRef< ITextDecorator > > InDecorators, const ISlateStyle* const InDecoratorStyleSet, FSlateFontInfo InBoldFont);


	void SetText(const FString& SourceString, FTextLayout& TargetTextLayout) override;

protected:

	FCustomRichTextLayoutMarshaller(TArray< TSharedRef< ITextDecorator > > InDecorators, const ISlateStyle* const InDecoratorStyleSet);
	FCustomRichTextLayoutMarshaller(TSharedPtr< IRichTextMarkupParser > InParser, TSharedPtr< IRichTextMarkupWriter > InWriter, TArray< TSharedRef< ITextDecorator > > InDecorators, const ISlateStyle* const InDecoratorStyleSet, FSlateFontInfo InBoldFont);

protected:

	virtual void AppendRunsForText(
		const int32 LineIndex,
		const FTextRunParseResults& TextRun,
		const FString& ProcessedString,
		const FTextBlockStyle& DefaultTextStyle,
		const TSharedRef<FString>& InOutModelText,
		FTextLayout& TargetTextLayout,
		TArray<TSharedRef<IRun>>& Runs,
		TArray<FTextLineHighlight>& LineHighlights,
		TMap<const FTextBlockStyle*, TSharedPtr<FSlateTextUnderlineLineHighlighter>>& CachedUnderlineHighlighters,
		TMap<const FTextBlockStyle*, TSharedPtr<FSlateTextStrikeLineHighlighter>>& CachedStrikeLineHighlighters
	) override;

	bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text, FTextBlockStyle* NewColorStyle);

	void AddArry(int32 lineindex, int32 linewidth, TArray<FTextLineParseResults>& arry, FSlateFontInfo font, FTextRunParseResults info);

protected:

	FSlateFontInfo BoldFont;

	//拆分富文本时记录的信息 key 拆分行的第几个rans  value 拆分的信息
	TMap<int32 , TArray<FTextLineParseResults>> clipinfo;
};