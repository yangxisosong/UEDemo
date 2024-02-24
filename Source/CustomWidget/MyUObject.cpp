#include "MyUObject.h"
#include "SSlateMyPanel.h"

#define LOCTEXT_NAMESPACE "UMG"


void UMyTestSlot::SetPadding(FMargin InPadding)
{
	MyPadding = InPadding;

	if (MySlot)
	{
		MySlot->Padding(InPadding);
	}
}


void UMyTestSlot::SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	MyHorizontalAlignment = InHorizontalAlignment;
	if (MySlot)
	{
		MySlot->HAlign(InHorizontalAlignment);
	}
}


void UMyTestSlot::SetVerticalAlignment(EVerticalAlignment InVerticalAlignment)
{
	MyVerticalAlignment = InVerticalAlignment;
	if (MySlot)
	{
		MySlot->VAlign(InVerticalAlignment);
	}
}

void UMyTestSlot::SynchronizeProperties()
{
	SetPadding(MyPadding);
	SetHorizontalAlignment(MyHorizontalAlignment);
	SetVerticalAlignment(MyVerticalAlignment);
}

void UMyTestSlot::BuildSlot(TSharedRef<SSlateMyPanel> InOverlay)
{
	MySlot = &InOverlay->AddSlot(0)
		.Padding(MyPadding)
		.HAlign(MyHorizontalAlignment)
		.VAlign(MyVerticalAlignment)
		[
			Content == NULL ? SNullWidget::NullWidget : Content->TakeWidget()
		];
}

void UMyTestSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MySlot = NULL;
}
