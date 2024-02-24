// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomRichTextBlock.h"
#include "Framework/Text/RichTextMarkupProcessing.h"
#include "CustomRichTextMarkupParser.h"
#include "CustomRichTextLayoutMarshaller.h"
#include "Styling/SlateStyle.h"
#include "Components/SlateWrapperTypes.h"
#include "Widgets/Text/SRichTextBlock.h"
#define LOCTEXT_NAMESPACE "UMG"

TSharedPtr< class IRichTextMarkupParser > UCustomRichTextBlock::CreateMarkupParser()
{
	return FCustomRichTextMarkupParser::CustomCreate();
}

void UCustomRichTextBlock::UpdateStyle()
{
	UpdateStyleData();
}
#if WITH_EDITOR
const FText UCustomRichTextBlock::GetPaletteCategory()
{
	return LOCTEXT("LongYuan", "LongYuan");
}
#endif
void UCustomRichTextBlock::UpdateStyleData()
{
	DefaultTextStyle = CustomDefaultStyle;
	Super::UpdateStyleData();

}

TSharedRef<SWidget> UCustomRichTextBlock::RebuildWidget()
{
	UpdateStyleData();

	TArray< TSharedRef< class ITextDecorator > > CreatedDecorators;
	CreateDecorators(CreatedDecorators);

	TSharedRef<FCustomRichTextLayoutMarshaller> Marshaller = FCustomRichTextLayoutMarshaller::CreateCustom(CreateMarkupParser(), CreateMarkupWriter(), CreatedDecorators, StyleInstance.Get(),CustomBoldFont);

	MyRichTextBlock =
		SNew(SRichTextBlock)
		.TextStyle(&DefaultTextStyle)
		.Marshaller(Marshaller);

	return MyRichTextBlock.ToSharedRef();
}