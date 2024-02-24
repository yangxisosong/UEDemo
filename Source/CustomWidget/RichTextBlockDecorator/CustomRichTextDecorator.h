#include "CoreMinimal.h"
#include "Framework/SlateDelegates.h"
#include "Components/RichTextBlockDecorator.h"
#include "SCustomRichTextHyperlink.h"
#include "CustomRichTextDecorator.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(OnSetHovered, bool);
class FCustomRichTextDecorator : public FRichTextDecorator
{
public:
	FCustomRichTextDecorator(URichTextBlock* InOwner);
	virtual ~FCustomRichTextDecorator();

	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;
	float GetWrapTextAt();

protected:
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const override;
	mutable FTextBlockStyle MyStyle;
	FSlateFontInfo BoldFont;
	mutable bool customBold = false;
	mutable bool fontcolorOverride = false;
	mutable FColor fontcolor = FColor::White;
	mutable bool fontsizeOverride = false;
	mutable int32 fontsize = 10;
	mutable FString url = "";
	FHyperlinkStyle linkstyle;
	mutable bool isUrl = false;
	mutable FText textcontent;
	FSimpleDelegate del;
	FOnTextChanged OnClickLink;
	FHoveredDelegate OnHovered;

	OnSetHovered m_OnSetHovered;
private:
	float m_wrapTextAt;
	TArray<TSharedPtr<SWidget>> richlink;
	TWeakPtr<SCustomRichTextHyperlink> testobj;
};
//////////////////////////////////////////////////////////////////////////

UCLASS()
class UCustomRichTextBlockDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()
public:
	UCustomRichTextBlockDecorator(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	UPROPERTY(EditAnywhere, Category = Appearance)
		FHyperlinkStyle linkstyle;


};