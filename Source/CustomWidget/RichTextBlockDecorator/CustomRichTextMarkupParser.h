#include "CoreMinimal.h"
#include "Internationalization/Regex.h"
#include "Framework/Text/IRichTextMarkupParser.h"

class FCustomRichTextMarkupParser : public IRichTextMarkupParser
{

public:
	static TSharedRef< FCustomRichTextMarkupParser > CustomCreate();

public:
	virtual void Process(TArray<FTextLineParseResults>& Results, const FString& Input, FString& Output) override;

private:
	FCustomRichTextMarkupParser();

	void ParseLineRanges(const FString& Input, const TArray<FTextRange>& LineRanges, TArray<FTextLineParseResults>& LineParseResultsArray) const;
	void HandleEscapeSequences(const FString& Input, TArray<FTextLineParseResults>& LineParseResultsArray, FString& ConcatenatedUnescapedLines) const;

	void ParseSpecialString(const FString& Input, FTextLineParseResults& LineParseResults, int ContentBegin, TMap<FString, FTextRange>& Attributes) const;

	FTextRange ParsetheAttributes(TMap<FString, FTextRange>& Attributes, const FString& content,int beginLoc,int endLoc, FTextLineParseResults& LineParseResults, bool& needAdd) const;
	FRegexPattern RegexPattern;

};