#include "TestUPanel.h"
#include "Components/SizeBoxSlot.h"
#include "MyUObject.h"

#define LOCTEXT_NAMESPACE "UMG"

void UTestUPanel::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UTestUPanel::ReleaseSlateResources(bool bReleaseChildren)
{
	Myui.Reset();
}

#if WITH_EDITOR
const FText UTestUPanel::GetPaletteCategory()
{
	return LOCTEXT("CustomPaletteCategory", "My custom category2!");
}
#endif

TSharedRef<SWidget> UTestUPanel::RebuildWidget()
{
	Myui = SNew(SSlateMyPanel);


	for (UPanelSlot* PanelSlot : Slots)
	{
		UMyTestSlot* TypedSlot = Cast<UMyTestSlot>(PanelSlot);
		if (TypedSlot!=nullptr)
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(Myui.ToSharedRef());
		}
	}

	return Myui.ToSharedRef();
}

UClass* UTestUPanel::GetSlotClass() const
{
	return UMyTestSlot::StaticClass();
}

void UTestUPanel::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live canvas if it already exists
	if (Myui.IsValid())
	{
		CastChecked<UMyTestSlot>(InSlot)->BuildSlot(Myui.ToSharedRef());
	}
}

void UTestUPanel::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (Myui.IsValid())
	{
		TSharedPtr<SWidget> Widget = InSlot->Content->GetCachedWidget();
		if (Widget.IsValid())
		{
			Myui->RemoveSlot(Widget.ToSharedRef());
		}
	}
}
