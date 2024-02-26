#include "SSlateMyPanel.h"
#include "SlateOptMacros.h"
#include "Layout/LayoutUtils.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSlateMyPanel::Construct(const SSlateMyPanel::FArguments& InArgs)
{
	//const int32 NumSlots = InArgs.Slots.Num();
	//for (int32 SlotIndex = 0; SlotIndex < NumSlots; ++SlotIndex)
	//{
	//	Children.Add(InArgs.Slots[SlotIndex]);
	//}
}

SSlateMyPanel::SSlateMyPanel():Children(this)
{
	SetCanTick(false);
	bCanSupportFocus = false;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

int32 SSlateMyPanel::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	FArrangedChildren ArrangedChildren(EVisibility::Visible);
	{
		// The box panel has no visualization of its own; it just visualizes its children.
		ArrangeChildren(AllottedGeometry, ArrangedChildren);
	}
	int32 MaxLayerId = LayerId;
	FPaintArgs NewArgs = Args.WithNewParent(this);
	const bool bChildrenEnabled = ShouldBeEnabled(bParentEnabled);


	for (int32 ChildIndex = 0; ChildIndex < ArrangedChildren.Num(); ++ChildIndex)
	{
		FArrangedWidget& CurArrangedWidget = ArrangedChildren[ChildIndex];

		// We don't increment the first layer.
		if (ChildIndex > 0)
		{
			MaxLayerId++;
		}

		const int32 CurWidgetsMaxLayerId =
			CurArrangedWidget.Widget->Paint(
				NewArgs,
				CurArrangedWidget.Geometry,
				MyCullingRect,
				OutDrawElements,
				MaxLayerId,
				InWidgetStyle,
				bChildrenEnabled);

		// This is a hack to account for widgets incrementing their layer id inside an overlay in global invalidation mode.  
		// Overlay slots that do not update will not know about the new layer id.  This padding adds buffering to avoid that being a problem for now
		// This is a temporary solution until we build a full rendering tree
		const int32 OverlaySlotPadding = 10;
		MaxLayerId = CurWidgetsMaxLayerId + FMath::Min(FMath::Max((CurWidgetsMaxLayerId - MaxLayerId) / OverlaySlotPadding, 1) * OverlaySlotPadding, 100);

		// Non padding method
		//MaxLayerId = FMath::Max(CurWidgetsMaxLayerId, MaxLayerId);

	}
	return LayerId;
}

void SSlateMyPanel::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ++ChildIndex)
	{
		const FTestSlot& CurChild = Children[ChildIndex];
		const EVisibility ChildVisibility = CurChild.GetWidget()->GetVisibility();
		if (ArrangedChildren.Accepts(ChildVisibility))
		{
			const FMargin SlotPadding(LayoutPaddingWithFlow(GSlateFlowDirection, CurChild.SlotPadding.Get()));
			AlignmentArrangeResult XResult = AlignChild<Orient_Horizontal>(GSlateFlowDirection, AllottedGeometry.GetLocalSize().X, CurChild, SlotPadding);
			AlignmentArrangeResult YResult = AlignChild<Orient_Vertical>(AllottedGeometry.GetLocalSize().Y, CurChild, SlotPadding);

			ArrangedChildren.AddWidget(ChildVisibility, AllottedGeometry.MakeChild(
				CurChild.GetWidget(),
				FVector2D(XResult.Offset/2, YResult.Offset/2),
				FVector2D(XResult.Size, YResult.Size)
			));
		}
	}
}

FVector2D SSlateMyPanel::ComputeDesiredSize(float) const
{
	return FVector2D(0,0);
}

FChildren* SSlateMyPanel::GetChildren()
{
	return &Children;
}

SSlateMyPanel::FTestSlot& SSlateMyPanel::AddSlot(int32 ZOrder /*= INDEX_NONE*/)
{
	FTestSlot& NewSlot = *new FTestSlot();
	if (ZOrder == INDEX_NONE)
	{
		// No ZOrder was specified; just add to the end of the list.
		// Use a ZOrder index one after the last elements.
		ZOrder = (Children.Num() == 0)
			? 0
			: (Children[Children.Num() - 1].ZOrder + 1);

		this->Children.Add(&NewSlot);
	}
	else
	{
		// Figure out where to add the widget based on ZOrder
		bool bFoundSlot = false;
		int32 CurSlotIndex = 0;
		for (; CurSlotIndex < Children.Num(); ++CurSlotIndex)
		{
			const FTestSlot& CurSlot = Children[CurSlotIndex];
			if (ZOrder < CurSlot.ZOrder)
			{
				// Insert before
				bFoundSlot = true;
				break;
			}
		}

		// Add a slot at the desired location
		this->Children.Insert(&NewSlot, CurSlotIndex);
	}

	Invalidate(EInvalidateWidget::Layout);

	NewSlot.ZOrder = ZOrder;
	return NewSlot;
}

bool SSlateMyPanel::RemoveSlot(TSharedRef< SWidget > Widget)
{
	// Search and remove
	for (int32 CurSlotIndex = 0; CurSlotIndex < Children.Num(); ++CurSlotIndex)
	{
		const FTestSlot& CurSlot = Children[CurSlotIndex];
		if (CurSlot.GetWidget() == Widget)
		{
			Children.RemoveAt(CurSlotIndex);
			Invalidate(EInvalidateWidget::Layout);
			return true;
		}
	}

	return false;
}

EHorizontalAlignment SSlateMyPanel::GetHorizontalAlignment() const
{
	return EHorizontalAlignment();
}

EVerticalAlignment SSlateMyPanel::GetVerticalAlignment() const
{
	return EVerticalAlignment();
}
