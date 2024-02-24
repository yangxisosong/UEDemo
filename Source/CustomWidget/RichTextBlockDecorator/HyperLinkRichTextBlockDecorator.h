#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "HyperLinkRichTextBlockDecorator.generated.h"

class FRichInlineHyperLinkDecorator : public FRichTextDecorator
{
public:
	FRichInlineHyperLinkDecorator(URichTextBlock* InOwner, UHyperLinkRichTextBlockDecorator* decorator);
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;
protected:
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const override;
	FHyperlinkStyle linkstyle;
	FSimpleDelegate del;
	mutable FString idString;
};
//////////////////////////////////////////////////////////////////////////

UCLASS()
class UHyperLinkRichTextBlockDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()
public:
	UHyperLinkRichTextBlockDecorator(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	UPROPERTY(EditAnywhere, Category = Appearance)
		FHyperlinkStyle style;

	UFUNCTION(BlueprintNativeEvent)
		void ClickFun();
};