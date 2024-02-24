#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "Engine/DataTable.h"
#include "HyperlinkDecorator.generated.h"

USTRUCT()
struct FRichHlinkRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Appearance)
	FHyperlinkStyle HlinkStyle;
};

/**
 * 
 */
UCLASS(Blueprintable)
class UHyperlinkDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()
public:
	UHyperlinkDecorator(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	virtual const FHyperlinkStyle* FindHyperLinkBrush(FName TagOrId, bool bWarnIfMissing);

	UFUNCTION(BlueprintImplementableEvent)
	void ClickFunc();

	UPROPERTY(EditAnywhere)
	FString HlName;

	void SetHyperLinkContent(const FString* str);
protected:

	FRichHlinkRow* FindHlinkRow(FName TagOrId, bool bWarningIfMissing);

	UPROPERTY(EditAnywhere, Category = Appearance, meta = (RowType = "RichHlinkRow"))
	class UDataTable* HlinkSet;

	UPROPERTY(BlueprintReadOnly)
	FString HlContent;
};