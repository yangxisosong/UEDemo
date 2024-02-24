// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomRichTextLayoutMarshaller.h"
#include "Framework/Text/SlateTextRun.h"
#include "Framework/Text/SlateTextUnderlineLineHighlighter.h"
#include "Framework/Text/SlateTextLayout.h"
#include "Framework/Text/RichTextLayoutMarshaller.h"
#include "Framework/Text/IRichTextMarkupParser.h"
#include "CustomRichTextDecorator.h"
#include "Runtime/SlateCore/Public/Fonts/FontMeasure.h"

TSharedRef< FCustomRichTextLayoutMarshaller > FCustomRichTextLayoutMarshaller::CreateCustom(TArray< TSharedRef< ITextDecorator > > InDecorators, const ISlateStyle* const InDecoratorStyleSet)
{
	return MakeShareable(new FCustomRichTextLayoutMarshaller(MoveTemp(InDecorators), InDecoratorStyleSet));
}

TSharedRef< FCustomRichTextLayoutMarshaller > FCustomRichTextLayoutMarshaller::CreateCustom(TSharedPtr< IRichTextMarkupParser > InParser, TSharedPtr< IRichTextMarkupWriter > InWriter, TArray< TSharedRef< ITextDecorator > > InDecorators, const ISlateStyle* const InDecoratorStyleSet, FSlateFontInfo InBoldFont)
{
	return MakeShareable(new FCustomRichTextLayoutMarshaller(MoveTemp(InParser), MoveTemp(InWriter), MoveTemp(InDecorators), InDecoratorStyleSet,InBoldFont));
}

void FCustomRichTextLayoutMarshaller::SetText(const FString& SourceString, FTextLayout& TargetTextLayout)
{
	const FTextBlockStyle& DefaultTextStyle = static_cast<FSlateTextLayout&>(TargetTextLayout).GetDefaultTextStyle();

	TArray<FTextLineParseResults> LineParseResultsArray;
	FString ProcessedString;
	Parser->Process(LineParseResultsArray, SourceString, ProcessedString);

	auto size= TargetTextLayout.GetSize();

	float boxwidth = TargetTextLayout.GetWrappingWidth();
	TargetTextLayout.SetWrappingWidth(0);
	UE_LOG(LogTemp, Warning, TEXT("TargetTextLayout GetWrappingWidth x:%f"), boxwidth);

	TArray<FTextLineParseResults> LineParseResultsArrayNew;

	int32 currentwidth = 0;
	for (int32 LineIndex = 0; LineIndex < LineParseResultsArray.Num(); ++LineIndex)
	{
		FTextLineParseResults& LineParseResults = LineParseResultsArray[LineIndex];
		//记录当前行所占宽度 超出增加行 重置该标志位
		TArray< FTextRunParseResults > oldRuns;
		for (FTextRunParseResults& RunParseResult : LineParseResults.Runs)
		{
			
			FVector2D TextSize;
			FString str;
			//分别计算各个run所占空间
			if (RunParseResult.Name=="rich")
			{
				str = SourceString.Mid(RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
				UE_LOG(LogTemp, Warning, TEXT("FCustomRichTextLayoutMarshaller rich str = %s BeginIndex %f EndIndex  %f"), *str, RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex);
				//富文本
				auto stylefont = DefaultTextStyle.Font;
				TextSize = FSlateApplication::Get().GetRenderer()->GetFontMeasureService()->Measure(str, stylefont);
				UE_LOG(LogTemp, Warning, TEXT("FCustomRichTextLayoutMarshaller %s rich width == %f"), *str,TextSize.X);
			}
			else if(RunParseResult.Name == "")
			{
				str = SourceString.Mid(RunParseResult.OriginalRange.BeginIndex, RunParseResult.OriginalRange.EndIndex - RunParseResult.OriginalRange.BeginIndex);
				UE_LOG(LogTemp, Warning, TEXT("FCustomRichTextLayoutMarshaller normal str = %s BeginIndex %f EndIndex  %f"), *str, RunParseResult.OriginalRange.BeginIndex, RunParseResult.OriginalRange.EndIndex);
				//纯文本
				auto stylefont = DefaultTextStyle.Font;
				TextSize = FSlateApplication::Get().GetRenderer()->GetFontMeasureService()->Measure(str, stylefont);
				UE_LOG(LogTemp, Warning, TEXT("FCustomRichTextLayoutMarshaller %s normal width == %f"), *str, TextSize.X);
				//oldRuns.Add(RunParseResult);
			}
			float newlinewidth = currentwidth + TextSize.X;
			//一行内放不下富文本， 需要将文本拆分
			if (newlinewidth > boxwidth)
			{
				//每个字符的平均宽度
				int onesize = FMath::CeilToInt(TextSize.X / str.Len());

				if (boxwidth < onesize)
				{
					return;
				}

				int32 sumnum = str.Len();
				int32 lastindex = 0;
				//需要把文本分割成几行
				while (sumnum>0)
				{
					//剩余宽度(多5像素空间 )
					float widthresult = boxwidth - currentwidth - 5;
					//剩余宽度可以放下几个字符
					int32 maxnum = FMath::FloorToInt(widthresult / onesize);
					int addnum = FMath::Min( sumnum, maxnum );

					if (maxnum < sumnum)
					{
						FTextLineParseResults newline1;
						FTextRange ContentRange1;
						if (RunParseResult.Name == "rich")
						{
							ContentRange1.BeginIndex = FMath::Max(RunParseResult.ContentRange.BeginIndex, lastindex);
							ContentRange1.EndIndex = ContentRange1.BeginIndex + addnum;
							lastindex = ContentRange1.EndIndex;
							FTextRunParseResults rs1(RunParseResult.Name, RunParseResult.OriginalRange, ContentRange1);
							rs1.MetaData = RunParseResult.MetaData;
							newline1.Runs = oldRuns;
							newline1.Runs.Add(rs1);
						}
						else
						{
							ContentRange1.BeginIndex = FMath::Max(RunParseResult.OriginalRange.BeginIndex, lastindex);
							ContentRange1.EndIndex = ContentRange1.BeginIndex + addnum;
							lastindex = ContentRange1.EndIndex;
							FTextRunParseResults rs1(RunParseResult.Name, ContentRange1);
							rs1.MetaData = RunParseResult.MetaData;
							newline1.Runs = oldRuns;
							newline1.Runs.Add(rs1);
							
						}

						newline1.Range = LineParseResults.Range;
						LineParseResultsArrayNew.Add(newline1);
						oldRuns.Empty();
						currentwidth = 0;
					}
					else
					{
						FTextRange ContentRange2;
						if (RunParseResult.Name == "rich")
						{
							ContentRange2.BeginIndex = FMath::Max(RunParseResult.ContentRange.BeginIndex, lastindex);
							ContentRange2.EndIndex = ContentRange2.BeginIndex + addnum;
							FTextRunParseResults rs2(RunParseResult.Name, RunParseResult.OriginalRange, ContentRange2);
							rs2.MetaData = RunParseResult.MetaData;
							oldRuns.Add(rs2);
							lastindex = ContentRange2.EndIndex;
						}
						else
						{
							ContentRange2.BeginIndex = FMath::Max(RunParseResult.OriginalRange.BeginIndex, lastindex);
							ContentRange2.EndIndex = ContentRange2.BeginIndex + addnum;
							FTextRunParseResults rs2(RunParseResult.Name, ContentRange2);
							rs2.MetaData = RunParseResult.MetaData;
							oldRuns.Add(rs2);
							lastindex = ContentRange2.EndIndex;
						};

						currentwidth = (ContentRange2.EndIndex - ContentRange2.BeginIndex) * onesize;
					}
					sumnum = sumnum - addnum;
				}
			}
			else
			{
				oldRuns.Add(RunParseResult);
				currentwidth += TextSize.X;
			}

		}
		if (oldRuns.Num() > 0)
		{
			FTextLineParseResults newline1;
			newline1.Runs = oldRuns;
			newline1.Range = LineParseResults.Range;
			LineParseResultsArrayNew.Add(newline1);
			oldRuns.Empty();
		}
	};



	TArray<FTextLayout::FNewLineData> LinesToAdd;
	LinesToAdd.Reserve(LineParseResultsArray.Num());

	TArray<FTextLayout::FNewLineData> LinesToAdd2;
	LinesToAdd.Reserve(LineParseResultsArray.Num());

	TArray<FTextLineHighlight> LineHighlightsToAdd;
	TMap<const FTextBlockStyle*, TSharedPtr<FSlateTextUnderlineLineHighlighter>> CachedUnderlineHighlighters;
	TMap<const FTextBlockStyle*, TSharedPtr<FSlateTextStrikeLineHighlighter>> CachedStrikeLineHighlighters;

	// Iterate through parsed line results and create processed lines with runs.
	for (int32 LineIndex = 0; LineIndex < LineParseResultsArrayNew.Num(); ++LineIndex)
	{
		const FTextLineParseResults& LineParseResults = LineParseResultsArrayNew[LineIndex];

		TSharedRef<FString> ModelString = MakeShareable(new FString());
		TArray< TSharedRef< IRun > > Runs;

		for (const FTextRunParseResults& RunParseResult : LineParseResults.Runs)
		{
			AppendRunsForText(LineIndex, RunParseResult, ProcessedString, DefaultTextStyle, ModelString, TargetTextLayout, Runs, LineHighlightsToAdd, CachedUnderlineHighlighters, CachedStrikeLineHighlighters);
		}

		TArray< TSharedRef< IRun > > Runs2;
		if (Runs.Num() > 2)
		{
			//Runs2.Add(Runs[1]);
		}

		for (int32 k = 1; k < Runs.Num(); ++k)
		{
			
		}

		TSharedRef<FString> str = MakeShareable( new FString("01234"));
		FRunInfo RunInfotest;
		FTextRange ModelRange;
		ModelRange.BeginIndex = 0;
		ModelRange.EndIndex = str->Len();

		const FTextBlockStyle* TextBlockStyle = &DefaultTextStyle;
		//FSlateTextRun::Create( const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& Style )
		auto rs = FSlateTextRun::Create(RunInfotest, str, *TextBlockStyle, ModelRange);
		Runs2.Add(rs);
		LinesToAdd2.Emplace(str, Runs2);

		LinesToAdd.Emplace(MoveTemp(ModelString), MoveTemp(Runs));
	}
	TargetTextLayout.AddLines(LinesToAdd);
	//TargetTextLayout.AddLines(LinesToAdd2);
	TargetTextLayout.SetLineHighlights(LineHighlightsToAdd);
}

FCustomRichTextLayoutMarshaller::FCustomRichTextLayoutMarshaller(TArray< TSharedRef< ITextDecorator > > InDecorators, const ISlateStyle* const InDecoratorStyleSet)
	:FRichTextLayoutMarshaller(InDecorators, InDecoratorStyleSet)
{

}

FCustomRichTextLayoutMarshaller::FCustomRichTextLayoutMarshaller(TSharedPtr< IRichTextMarkupParser > InParser, TSharedPtr< IRichTextMarkupWriter > InWriter, TArray< TSharedRef< ITextDecorator > > InDecorators, const ISlateStyle* const InDecoratorStyleSet, FSlateFontInfo InBoldFont)
	: FRichTextLayoutMarshaller(InParser, InWriter, InDecorators, InDecoratorStyleSet)
{
	BoldFont = InBoldFont;
}

void FCustomRichTextLayoutMarshaller::AppendRunsForText(const int32 LineIndex, const FTextRunParseResults& TextRun, const FString& ProcessedString, const FTextBlockStyle& DefaultTextStyle, const TSharedRef<FString>& InOutModelText, FTextLayout& TargetTextLayout, TArray<TSharedRef<IRun>>& Runs, TArray<FTextLineHighlight>& LineHighlights, TMap<const FTextBlockStyle*, TSharedPtr<FSlateTextUnderlineLineHighlighter>>& CachedUnderlineHighlighters, TMap<const FTextBlockStyle*, TSharedPtr<FSlateTextStrikeLineHighlighter>>& CachedStrikeLineHighlighters)
{

	TSharedPtr< ISlateRun > Run;
	TSharedPtr< ISlateRun > Run2;
	TSharedPtr< ITextDecorator > Decorator = TryGetDecorator(ProcessedString, TextRun);

	if (Decorator.IsValid())
	{
		// Create run and update model string.
		Run = Decorator->Create(TargetTextLayout.AsShared(), TextRun, ProcessedString, InOutModelText, DecoratorStyleSet);

		//Run2 = Decorator->Create(TargetTextLayout.AsShared(), TextRun, ProcessedString, InOutModelText, DecoratorStyleSet);
	}

	else
	{
		FRunInfo RunInfo(TextRun.Name);
		for (const TPair<FString, FTextRange>& Pair : TextRun.MetaData)
		{
			int32 Length = FMath::Max(0, Pair.Value.EndIndex - Pair.Value.BeginIndex);
			RunInfo.MetaData.Add(Pair.Key, ProcessedString.Mid(Pair.Value.BeginIndex, Length));
		}

		const FTextBlockStyle* TextBlockStyle;
		FTextRange ModelRange;
		ModelRange.BeginIndex = InOutModelText->Len();
		FTextBlockStyle NewColorStyle = DefaultTextStyle;
		if (Supports(TextRun, ProcessedString, &NewColorStyle))
		{
			*InOutModelText += ProcessedString.Mid(TextRun.ContentRange.BeginIndex, TextRun.ContentRange.EndIndex - TextRun.ContentRange.BeginIndex);
			TextBlockStyle = &NewColorStyle;
		}
		else
		{
			*InOutModelText += ProcessedString.Mid(TextRun.OriginalRange.BeginIndex, TextRun.OriginalRange.EndIndex - TextRun.OriginalRange.BeginIndex);
			TextBlockStyle = &DefaultTextStyle;
		}
		ModelRange.EndIndex = InOutModelText->Len();

		// Create run.
		Run = FSlateTextRun::Create(RunInfo, InOutModelText, *TextBlockStyle, ModelRange);

		if (!TextBlockStyle->UnderlineBrush.GetResourceName().IsNone())
		{
			TSharedPtr<FSlateTextUnderlineLineHighlighter> UnderlineLineHighlighter = CachedUnderlineHighlighters.FindRef(TextBlockStyle);
			if (!UnderlineLineHighlighter.IsValid())
			{
				UnderlineLineHighlighter = FSlateTextUnderlineLineHighlighter::Create(TextBlockStyle->UnderlineBrush, TextBlockStyle->Font, TextBlockStyle->ColorAndOpacity, TextBlockStyle->ShadowOffset, TextBlockStyle->ShadowColorAndOpacity);
				CachedUnderlineHighlighters.Add(TextBlockStyle, UnderlineLineHighlighter);
			}

			LineHighlights.Add(FTextLineHighlight(LineIndex, ModelRange, FSlateTextUnderlineLineHighlighter::DefaultZIndex, UnderlineLineHighlighter.ToSharedRef()));
		}

		if (!TextBlockStyle->StrikeBrush.GetResourceName().IsNone())
		{
			TSharedPtr<FSlateTextStrikeLineHighlighter> StrikeLineHighlighter = CachedStrikeLineHighlighters.FindRef(TextBlockStyle);
			if (!StrikeLineHighlighter.IsValid())
			{
				StrikeLineHighlighter = FSlateTextStrikeLineHighlighter::Create(TextBlockStyle->StrikeBrush, TextBlockStyle->Font, TextBlockStyle->ColorAndOpacity, TextBlockStyle->ShadowOffset, TextBlockStyle->ShadowColorAndOpacity);
				CachedStrikeLineHighlighters.Add(TextBlockStyle, StrikeLineHighlighter);
			}

			LineHighlights.Add(FTextLineHighlight(LineIndex, ModelRange, FSlateTextStrikeLineHighlighter::DefaultZIndex, StrikeLineHighlighter.ToSharedRef()));
		}
	}

	Runs.Add(Run.ToSharedRef());
	if (Run2 != nullptr)
	{
		Runs.Add(Run2.ToSharedRef());
	}
}

bool FCustomRichTextLayoutMarshaller::Supports(const FTextRunParseResults& RunParseResult, const FString& Text, FTextBlockStyle* NewColorStyle)
{
	if (RunParseResult.Name == TEXT("rich"))
	{
		if (RunParseResult.MetaData.Contains(TEXT("bold")))
		{
			NewColorStyle->SetFont(BoldFont);
		}
		if (RunParseResult.MetaData.Contains(TEXT("color")))
		{
			const FTextRange& IdRange = RunParseResult.MetaData[TEXT("color")];
			FString HexStr = Text.Mid(IdRange.BeginIndex, IdRange.EndIndex - IdRange.BeginIndex);
			NewColorStyle->SetColorAndOpacity(FSlateColor(FLinearColor(FColor::FromHex(HexStr))));
		}
		if (RunParseResult.MetaData.Contains(TEXT("size")))
		{
			const FTextRange& IdRange = RunParseResult.MetaData[TEXT("size")];
			FString FontSizeStr = Text.Mid(IdRange.BeginIndex, IdRange.EndIndex - IdRange.BeginIndex);
			int32 TexSize = FPlatformString::Atoi(*FontSizeStr);
			NewColorStyle->SetFontSize((uint16)TexSize);
		}
		return true;
	}
	return false;
}

void FCustomRichTextLayoutMarshaller::AddArry(int32 lineindex, int32 linewidth, TArray<FTextLineParseResults>& arry, FSlateFontInfo font, FTextRunParseResults info)
{
	
}
