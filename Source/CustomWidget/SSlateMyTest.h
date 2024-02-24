#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class CUSTOMWIDGET_API SSlateMyTest : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSlateMyTest)
		:_IsFocusable(true)
	{}

	SLATE_EVENT(FOnClicked, OnClicked)
	SLATE_EVENT(FPointerEventHandler, OnMouseButtonDown)
	SLATE_EVENT(FPointerEventHandler, OnMouseButtonUp)

	SLATE_ARGUMENT(bool, IsFocusable)

	SLATE_ARGUMENT(FSlateBrush*, Brush)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	                      FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	                      bool bParentEnabled) const override;

	void SetBrush(FSlateBrush* InBrush);
	void SetImage(TAttribute<const FSlateBrush*> InImage);
	virtual bool SupportsKeyboardFocus() const override;

	void TestSpline(const FGeometry& InParams);

	FOnClicked OnClicked;

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

protected:
	FInvalidatableBrushAttribute Brush;

	FInvalidatableBrushAttribute Image;

	int num = 0;

	uint8 bCanSupportFocus : 1;

	uint8 bIsFocusable : 1;
};
