#include "CustomRichTextMarkupParser.h"
#include "Internationalization/Regex.h"
#include "Runtime/SlateCore/Public/Fonts/FontMeasure.h"


TSharedRef< FCustomRichTextMarkupParser > FCustomRichTextMarkupParser::CustomCreate()
{
	return MakeShareable(new FCustomRichTextMarkupParser());
}


FCustomRichTextMarkupParser::FCustomRichTextMarkupParser()
	//: RegexPattern(FRegexPattern(TEXT("<(color=)(\\S{7})>([\\S ]+?)</color>|<(size=)(\\d{1,3})>([\\S ]+?)</size>")))
	//: RegexPattern(FRegexPattern(TEXT("(<(color=)(\\S{7})>([\\S ]+?)</color>|<(size=)(\\d{1,3})>([\\S ]+?)</size>|<a ([\\S\\s]+?)=\"([\\S\\s]+?)\">([\\S\\s]+?)</a>)")))
	//原//: RegexPattern(FRegexPattern(TEXT("(<(color=)(\\S{7})>([\\S ]+?)</color>|<(size=)(\\d{1,3})>([\\S ]+?)</size>|<a ([\\S\\s]+?)=\"([\\S\\s]+?)\">([\\S\\s]+?)</a>)")))
	: RegexPattern(FRegexPattern(TEXT("(<(color=)(\\S{7})>([\\S\\s]+?)</color>|<(size=)(\\d{1,3})>([\\S ]+?)</size>|<(b)>([\\S ]+?)</b>|<a ([\\S\\s]+?)=\"([\\S\\s]+?)\">([\\S\\s]+?)</a>|(\\\\n))")))
	//: RegexPattern(FRegexPattern(TEXT("(<a ([\\S\\s]+?)=\"([\\S\\s]+?)\">([\\S\\s]+?)</a>)")))
{

}


void FCustomRichTextMarkupParser::Process(TArray<FTextLineParseResults>& Results, const FString& Input, FString& Output)
{
	TArray<FTextRange> LineRanges;
	FTextRange::CalculateLineRangesFromString(Input, LineRanges);
	ParseLineRanges(Input, LineRanges, Results);
	HandleEscapeSequences(Input, Results, Output);
}



void FCustomRichTextMarkupParser::ParseLineRanges(const FString& Input, const TArray<FTextRange>& LineRanges, TArray<FTextLineParseResults>& LineParseResultsArray) const
{

	// Special regular expression pattern for matching rich text markup elements. IE: <ElementName AttributeName="AttributeValue">Content</>
	//auto Gpattern = ACConfig::getInstance()->getConfigValue(TEXT("DevelopConfig"), TEXT("RegexPattern"));
	//FRegexPattern rRegexPattern(Gpattern.GetValue());
	FRegexMatcher ElementRegexMatcher(RegexPattern, Input);

	// Parse line ranges, creating line parse results and run parse results.
	for (int32 i = 0; i < LineRanges.Num(); ++i)
	{
		FTextLineParseResults LineParseResults;
		LineParseResults.Range = LineRanges[i];

		// Limit the element regex matcher to the current line.
		ElementRegexMatcher.SetLimits(LineParseResults.Range.BeginIndex, LineParseResults.Range.EndIndex);

		// Iterate through the line, each time trying to find a match for the element regex, adding it as a run and any intervening text as another run.
		int32 LastRunEnd = LineParseResults.Range.BeginIndex;
		
	

		while (ElementRegexMatcher.FindNext())
		{
			int32 ElementBegin = ElementRegexMatcher.GetMatchBeginning();
			int32 ElementEnd = ElementRegexMatcher.GetMatchEnding();

			FTextRange OriginalRange(ElementBegin, ElementEnd);

			TMap<FString, FTextRange> Attributes;
			
#pragma region test CaptureGroup
			FString name0 = ElementRegexMatcher.GetCaptureGroup(0);
			FString name1 = ElementRegexMatcher.GetCaptureGroup(1);
			FString name2 = ElementRegexMatcher.GetCaptureGroup(2);
			FString name3 = ElementRegexMatcher.GetCaptureGroup(3);
			FString name4 = ElementRegexMatcher.GetCaptureGroup(4);
			FString name5 = ElementRegexMatcher.GetCaptureGroup(5);
			FString name6 = ElementRegexMatcher.GetCaptureGroup(6);
			FString name7 = ElementRegexMatcher.GetCaptureGroup(7);
			FString name8 = ElementRegexMatcher.GetCaptureGroup(8);
			FString name9 = ElementRegexMatcher.GetCaptureGroup(9);
			FString name10 = ElementRegexMatcher.GetCaptureGroup(10);
#pragma endregion

			// 先把空的地方加进去，避免因为后面出现了标签嵌套，导致后面先加。
			FTextRange InterveningRunRange(LastRunEnd, ElementBegin);
			if (!InterveningRunRange.IsEmpty())
			{
				FTextRunParseResults InterveningRunParseResults(FString(), InterveningRunRange);
				LastRunEnd = ElementBegin;
				LineParseResults.Runs.Add(InterveningRunParseResults);
			}

			bool needAdd = true;
			FTextRange ContentRange = ParsetheAttributes(Attributes, ElementRegexMatcher.GetCaptureGroup(1), ElementBegin, ElementEnd, LineParseResults,needAdd);

			// Name
			FString ElementName = TEXT("rich");

			// Add element run to line.
			if (needAdd)
			{
				int nu = (ContentRange.EndIndex + ContentRange.BeginIndex)/2;
				FTextRange ContentRange1 = ContentRange;
				ContentRange1.EndIndex = nu;
				FTextRunParseResults RunParseResults(ElementName, OriginalRange, ContentRange);
				RunParseResults.MetaData = Attributes;

				FTextRange ContentRange2 = ContentRange;
				ContentRange2.BeginIndex = nu;
				FTextRunParseResults RunParseResults2(ElementName, OriginalRange, ContentRange2);
				RunParseResults2.MetaData = Attributes;

				LineParseResults.Runs.Add(RunParseResults);
				//LineParseResults.Runs.Add(RunParseResults2);

				//富文本包含的文字
				//FString str = Input.Mid(ContentRange.BeginIndex, ContentRange.EndIndex);
				//const FVector2D TextSize = FSlateApplication::Get().GetRenderer()->GetFontMeasureService()->Measure(FText::FromString(str), Font);

			}
			LastRunEnd = ElementEnd;
		}

		// Add dangling run to line.
		FTextRange InterveningRunRange(LastRunEnd, LineParseResults.Range.EndIndex);
		if (!InterveningRunRange.IsEmpty())
		{
			FTextRunParseResults InterveningRunParseResults(FString(), InterveningRunRange);
			LastRunEnd = LineParseResults.Range.EndIndex;
			LineParseResults.Runs.Add(InterveningRunParseResults);
		}

		// Add blank, empty run if none are present.
		if (LineParseResults.Runs.Num() == 0)
		{
			FTextRunParseResults EmptyRunParseResults(FString(), LineParseResults.Range);
			LastRunEnd = EmptyRunParseResults.OriginalRange.EndIndex;
			LineParseResults.Runs.Add(EmptyRunParseResults);
		}

		for (int32 j=0;j< LineParseResults.Runs.Num();j++)
		{
			
		}
		LineParseResultsArray.Add(LineParseResults);
	}
}

void FCustomRichTextMarkupParser::HandleEscapeSequences(const FString& Input, TArray<FTextLineParseResults>& LineParseResultsArray, FString& ConcatenatedUnescapedLines) const
{
	ConcatenatedUnescapedLines = Input;
}

//这里说明一下，如果出现HTML标签包裹的情况，例如<size><b>啊啊啊啊啊啊</b><size>，则第一次切割出来的文本应该是<b>啊啊啊啊啊啊</b>
//这时候需要调用接口，再一次对文本做切割，然后直接放回到原来的list里面。
//并且要处理一下index的偏移问题。
void FCustomRichTextMarkupParser::ParseSpecialString(const FString& Input, FTextLineParseResults& LineParseResults,int ContentBegin, TMap<FString, FTextRange>& Attributes) const
{
	TArray<FTextRange> LineRanges;
	TArray<FTextLineParseResults> LineParseResultsArray;
	FTextRange::CalculateLineRangesFromString(Input, LineRanges);
	ParseLineRanges(Input, LineRanges, LineParseResultsArray);
	for (auto v : LineParseResultsArray)
	{
		for (auto d : v.Runs)
		{
			FString ElementName = TEXT("rich");
			d.OriginalRange.BeginIndex += ContentBegin;
			d.OriginalRange.EndIndex += ContentBegin;
			if (d.ContentRange.BeginIndex > 0)
			{
				d.ContentRange.BeginIndex += ContentBegin;
				d.ContentRange.EndIndex += ContentBegin;
			}
			else
			{
				d.ContentRange.BeginIndex = d.OriginalRange.BeginIndex;
				d.ContentRange.EndIndex = d.OriginalRange.EndIndex;
			}
			FTextRunParseResults RunParseResults(ElementName, d.OriginalRange, d.ContentRange);
			for (auto meta : Attributes)
			{
				RunParseResults.MetaData.Add(meta);
			}
			for (auto meta : d.MetaData)
			{
				meta.Value.BeginIndex += ContentBegin;
				meta.Value.EndIndex += ContentBegin;
				RunParseResults.MetaData.Add(meta);
			}
			LineParseResults.Runs.Add(RunParseResults);
		}
	}
}


FTextRange FCustomRichTextMarkupParser::ParsetheAttributes(TMap<FString, FTextRange>& Attributes, const FString& content,int beginLoc,int endLoc, FTextLineParseResults& LineParseResults,bool& needAdd) const
{
	
	int n = 0;
	FString tagtype;
	FString attriName = "";
	int32 attriBegin = -1;
	int32 attriEnd = -1;

	FString ContentName = "";
	int32 ContentBegin = -1;
	int32 ContentEnd = -1;
	//auto Gpattern = ACConfig::getInstance()->getConfigValue(TEXT("DevelopConfig"), TEXT("RegexPattern"));
	//FRegexPattern rRegexPattern(Gpattern.GetValue());
	FRegexMatcher ElementRegexMatcher(RegexPattern, content);

	if (ElementRegexMatcher.FindNext())
	{
		while (n < 13)
		{
			tagtype = ElementRegexMatcher.GetCaptureGroup(n);
			if (tagtype != ""&&n > 1)
				break;
			else
				n++;
		}

		//select tagtype
		if (tagtype == "color=") {
			attriName = TEXT("color");		
			if (content[0] != '<')
			{
				beginLoc = content.Find("<");
			}
			attriBegin = beginLoc + ElementRegexMatcher.GetCaptureGroupBeginning(3);
			attriEnd = beginLoc + ElementRegexMatcher.GetCaptureGroupEnding(3);

			ContentName = ElementRegexMatcher.GetCaptureGroup(4);
			ContentBegin = ElementRegexMatcher.GetCaptureGroupBeginning(4);
			ContentEnd = ElementRegexMatcher.GetCaptureGroupEnding(4);

			Attributes.Add(attriName, FTextRange(attriBegin, attriEnd));
			if (ContentName.Find("<") > 0 || ContentName.Find(">") > 0)//说明是一段话被两个html标签包裹了，重新处理
			{
				needAdd = false;
				ParseSpecialString(ContentName, LineParseResults, beginLoc + ContentBegin, Attributes);
			}
		}
		else if (tagtype == "size=") {
			attriName = TEXT("size");
			if (content[0] != '<')
				beginLoc = 0;
			attriBegin = beginLoc + ElementRegexMatcher.GetCaptureGroupBeginning(6);
			attriEnd = beginLoc + ElementRegexMatcher.GetCaptureGroupEnding(6);


			ContentName = ElementRegexMatcher.GetCaptureGroup(7);
			ContentBegin = ElementRegexMatcher.GetCaptureGroupBeginning(7);
			ContentEnd = ElementRegexMatcher.GetCaptureGroupEnding(7);
			Attributes.Add(attriName, FTextRange(attriBegin, attriEnd));
			if (ContentName.Find("<") > 0 || ContentName.Find(">") > 0)//说明是一段话被两个html标签包裹了，重新处理
			{
				needAdd = false;
				ParseSpecialString(ContentName, LineParseResults, beginLoc + ContentBegin, Attributes);
			}
		}
		else if (tagtype == "b") {
			attriName = TEXT("bold");
			if (content[0] != '<')
				beginLoc = 0;
			attriBegin = beginLoc + ElementRegexMatcher.GetCaptureGroupBeginning(8);
			attriEnd = beginLoc + ElementRegexMatcher.GetCaptureGroupEnding(8);


			ContentName = ElementRegexMatcher.GetCaptureGroup(9);
			ContentBegin = ElementRegexMatcher.GetCaptureGroupBeginning(9);
			ContentEnd = ElementRegexMatcher.GetCaptureGroupEnding(9);

			Attributes.Add(attriName, FTextRange(attriBegin, attriEnd));
			if (ContentName.Find("<") > 0 || ContentName.Find(">") > 0)//说明是一段话被两个html标签包裹了，重新处理
			{
				needAdd = false;
				ParseSpecialString(ContentName, LineParseResults, beginLoc + ContentBegin, Attributes);
			}
		}
		else if (tagtype == "href") {
			attriName = TEXT("href");
			if (content[0] != '<')
				beginLoc = 0;
			attriBegin = beginLoc + ElementRegexMatcher.GetCaptureGroupBeginning(11);
			attriEnd = beginLoc + ElementRegexMatcher.GetCaptureGroupEnding(11);


			ContentName = ElementRegexMatcher.GetCaptureGroup(12);
			ContentBegin = ElementRegexMatcher.GetCaptureGroupBeginning(12);
			ContentEnd = ElementRegexMatcher.GetCaptureGroupEnding(12);

			Attributes.Add(attriName, FTextRange(attriBegin, attriEnd));
			if (ContentName.Find("<") > 0 || ContentName.Find(">") > 0)//说明是一段话被两个html标签包裹了，重新处理
			{
				needAdd = false;
				ParseSpecialString(ContentName, LineParseResults, beginLoc + ContentBegin, Attributes);
			}

		}
		else if (tagtype == "\\n") {
			attriName = TEXT("newline");
			attriBegin = beginLoc + ElementRegexMatcher.GetCaptureGroupBeginning(13);
			attriEnd = beginLoc + ElementRegexMatcher.GetCaptureGroupEnding(13);

			Attributes.Add(attriName, FTextRange(attriBegin, attriEnd));
			return FTextRange(beginLoc, endLoc);

		}
		else {
			 FTextRange ContentRange(ContentBegin+beginLoc,ContentEnd+beginLoc);
			 return ContentRange;
		}
		//recursive parse attributes
		return ParsetheAttributes(Attributes, ContentName, ContentBegin + beginLoc, ContentEnd + beginLoc, LineParseResults, needAdd);
	}
	//return content range
	return FTextRange(beginLoc, endLoc);
}
