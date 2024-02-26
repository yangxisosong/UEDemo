#pragma once

#include "CoreMinimal.h"
#include "Widgets/SPanel.h"
#include "Layout/Children.h"

//自定义布局
class CUSTOMWIDGET_API SSlateMyPanel : public SPanel
{

public:
	//自定义布局的锚点信息
	class FTestSlot : public TSlotBase<FTestSlot>, public TAlignmentWidgetSlotMixin<FTestSlot>
	{
	public:
		FTestSlot()
			: TSlotBase<FTestSlot>()
			, TAlignmentWidgetSlotMixin<FTestSlot>(HAlign_Fill, VAlign_Fill)
			, SlotPadding(0.0f)
			, Offset(FVector2D::ZeroVector)
			, AllowScale(true)
		{ }

		SLATE_SLOT_BEGIN_ARGS_OneMixin(FTestSlot, TSlotBase<FTestSlot>, TAlignmentWidgetSlotMixin<FTestSlot>)
		SLATE_ATTRIBUTE(FMargin, Padding)
		SLATE_ATTRIBUTE(FVector2D, SlotOffset)
		SLATE_ATTRIBUTE(FVector2D, SlotSize)
		SLATE_ATTRIBUTE(bool, AllowScaling)
		SLATE_SLOT_END_ARGS()

		void Construct(const FChildren& SlotOwner, FSlotArguments&& InArgs);

	public:
		UE_DEPRECATED(5.0, "Padding is now deprecated. Use the FSlotArgument or the SetPadding function.")
			FTestSlot& Padding(const TAttribute<FMargin> InPadding)
		{
			SlotPadding = InPadding;
			return *this;
		}

		UE_DEPRECATED(5.0, "SlotOffset is now deprecated. Use the FSlotArgument or the SetSlotOffset function.")
			FTestSlot& SlotOffset(const TAttribute<FVector2D> InOffset)
		{
			Offset = InOffset;
			return *this;
		}

		UE_DEPRECATED(5.0, "SlotSize is now deprecated. Use the FSlotArgument or the SetSlotSize function.")
			FTestSlot& SlotSize(const TAttribute<FVector2D> InSize)
		{
			Size = InSize;
			return *this;
		}

		UE_DEPRECATED(5.0, "AllowScaling is now deprecated. Use the FSlotArgument or the SetAllowScalingfunction.")
			FTestSlot& AllowScaling(const TAttribute<bool> InAllowScale)
		{
			AllowScale = InAllowScale;
			return *this;
		}

	public:

		void SetPadding(TAttribute<FMargin> InPadding)
		{
			SlotPadding = MoveTemp(InPadding);
		}

		FMargin GetPadding() const
		{
			return SlotPadding.Get();
		}

		void SetSlotOffset(TAttribute<FVector2D> InOffset)
		{
			Offset = MoveTemp(InOffset);
		}

		FVector2D GetSlotOffset() const
		{
			return Offset.Get();
		}

		void SetSlotSize(TAttribute<FVector2D> InSize)
		{
			Size = MoveTemp(InSize);
		}

		FVector2D GetSlotSize() const
		{
			return Size.Get();
		}

		void SetAllowScaling(TAttribute<bool> InAllowScaling)
		{
			AllowScale = MoveTemp(InAllowScaling);
		}

		bool GetAllowScaling() const
		{
			return AllowScale.Get();
		}

	public:
		/** The child widget contained in this slot. */
		TAttribute<FMargin> SlotPadding;
		TAttribute<FVector2D> Offset;
		TAttribute<FVector2D> Size;
		TAttribute<bool> AllowScale;
		int32 ZOrder;
	};



	SLATE_BEGIN_ARGS(SSlateMyPanel){}

	SLATE_SLOT_ARGUMENT(SSlateMyPanel::FTestSlot, Slots)
	
	SLATE_END_ARGS()

public:

	//申明构造函数（没有默认构造函数）
	SSlateMyPanel();
	void Construct(const FArguments& InArgs);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

	/**
 * Panels arrange their children in a space described by the AllottedGeometry parameter. The results of the arrangement
 * should be returned by appending a FArrangedWidget pair for every child widget. See StackPanel for an example
 *
 * @param AllottedGeometry    The geometry allotted for this widget by its parent.
 * @param ArrangedChildren    The array to which to add the WidgetGeometries that represent the arranged children.
 */
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;

	/**
	 * A Panel's desired size in the space required to arrange of its children on the screen while respecting all of
	 * the children's desired sizes and any layout-related options specified by the user. See StackPanel for an example.
	 *
	 * @return The desired size.
	 */
	virtual FVector2D ComputeDesiredSize(float) const override;

	/**
	 * All widgets must provide a way to access their children in a layout-agnostic way.
	 * Panels store their children in Slots, which creates a dilemma. Most panels
	 * can store their children in a TPanelChildren<Slot>, where the Slot class
	 * provides layout information about the child it stores. In that case
	 * GetChildren should simply return the TPanelChildren<Slot>. See StackPanel for an example.
	 */
	virtual FChildren* GetChildren() override ;

	FTestSlot& AddSlot(int32 ZOrder = INDEX_NONE);

	bool RemoveSlot(TSharedRef< SWidget > Widget);

	EHorizontalAlignment GetHorizontalAlignment() const;

	EVerticalAlignment GetVerticalAlignment() const;
protected:
	/** The Box Panel's children. */
	TPanelChildren<FTestSlot> Children;


};
