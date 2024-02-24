// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlock.h"
#include "CustomRichTextBlock.generated.h"

/**
 * 
 */
UCLASS()
class UCustomRichTextBlock : public URichTextBlock
{
	GENERATED_BODY()

		DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClickLink, FString, link);

protected:

	virtual TSharedPtr< class IRichTextMarkupParser > CreateMarkupParser() override;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Appearance)
		FTextBlockStyle CustomDefaultStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
		FHyperlinkStyle CustomHyperLinktStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
		FSlateFontInfo CustomBoldFont;

	UPROPERTY(BlueprintAssignable)
		FClickLink OnClickLink;

	UFUNCTION(BlueprintCallable)
		void UpdateStyle();
public:
#if WITH_EDITOR
	//~ Begin UWidget Interface
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	virtual void UpdateStyleData() override;


	virtual TSharedRef<SWidget> RebuildWidget() override;

};
