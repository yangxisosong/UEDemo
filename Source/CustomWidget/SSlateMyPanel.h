#pragma once

#include "CoreMinimal.h"
#include "Widgets/SPanel.h"
#include "Layout/Children.h"

//自定义布局
class CUSTOMWIDGET_API SSlateMyPanel : public SPanel
{

public:
	//自定义布局的锚点信息
	class FTestSlot : public TSlotBase<FTestSlot>
	{
	public:
		virtual ~FTestSlot() {};
		FTestSlot()
			: TSlotBase<FTestSlot>()
			, HAlignment(HAlign_Fill)
			, VAlignment(VAlign_Fill)
			, SizeParam(FStretch(1))
			, SlotPadding(FMargin(0))
			, MaxSize(0.0f)
		{ }
	public:
		int32 ZOrder;
		/** Horizontal positioning of child within the allocated slot */
		TEnumAsByte<EHorizontalAlignment> HAlignment;

		/** Vertical positioning of child within the allocated slot */
		TEnumAsByte<EVerticalAlignment> VAlignment;

		/**
		* How much space this slot should occupy along panel's direction.
		*   When SizeRule is SizeRule_Auto, the widget's DesiredSize will be used as the space required.
		*   When SizeRule is SizeRule_Stretch, the available space will be distributed proportionately between
		*   peer Widgets depending on the Value property. Available space is space remaining after all the
		*   peers' SizeRule_Auto requirements have been satisfied.
		*/
		FSizeParam SizeParam;

		/** The padding to add around the child. */
		TAttribute<FMargin> SlotPadding;

		/** The max size that this slot can be (0 if no max) */
		TAttribute<float> MaxSize;
	public:

		FTestSlot& AutoWidth()
		{
			SizeParam = FAuto();
			return *this;
		}

		FTestSlot& MaxWidth(const TAttribute< float >& InMaxWidth)
		{
			MaxSize = InMaxWidth;
			return *this;
		}

		FTestSlot& FillWidth(const TAttribute< float >& StretchCoefficient)
		{
			SizeParam = FStretch(StretchCoefficient);
			return *this;
		}
		FTestSlot& Padding(float Uniform)
		{
			SlotPadding = FMargin(Uniform);
			return *this;
		}

		FTestSlot& Padding(float Horizontal, float Vertical)
		{
			SlotPadding = FMargin(Horizontal, Vertical);
			return *this;
		}

		FTestSlot& Padding(float Left, float Top, float Right, float Bottom)
		{
			SlotPadding = FMargin(Left, Top, Right, Bottom);
			return *this;
		}

		FTestSlot& Padding(TAttribute<FMargin> InPadding)
		{
			SlotPadding = InPadding;
			return *this;
		}

		FTestSlot& HAlign(EHorizontalAlignment InHAlignment)
		{
			HAlignment = InHAlignment;
			return *this;
		}

		FTestSlot& VAlign(EVerticalAlignment InVAlignment)
		{
			VAlignment = InVAlignment;
			return *this;
		}

		//FTestSlot& operator[](TSharedRef<SWidget> InWidget)
		//{
		//	FTestSlot::operator[](InWidget);
		//	return *this;
		//}

		FTestSlot& Expose(FTestSlot*& OutVarToInit)
		{
			OutVarToInit = this;
			return *this;
		}
	};

	SLATE_BEGIN_ARGS(SSlateMyPanel){}

	SLATE_SUPPORTS_SLOT(SSlateMyPanel::FTestSlot)

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
protected:
	/** The Box Panel's children. */
	TPanelChildren<FTestSlot> Children;


};
