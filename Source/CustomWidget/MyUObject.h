#pragma once

#include "CoreMinimal.h"

#include "Components/Widget.h"
#include "Components/PanelSlot.h"
#include "Layout/Margin.h"
#include "SSlateMyPanel.h"
#include "Types/SlateEnums.h"

#include "MyUObject.generated.h"

UCLASS()
class CUSTOMWIDGET_API UMyTestSlot : public UPanelSlot
{
	GENERATED_BODY()
public:
	/** 插槽 */
	SSlateMyPanel::FTestSlot* MySlot;

	/** 间距 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|MyTestSlot Slot")
		FMargin MyPadding;

	/** 水平布局 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|MyTestSlot Slot")
		TEnumAsByte<EHorizontalAlignment> MyHorizontalAlignment;

	/** 垂直布局 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|MyTestSlot Slot")
		TEnumAsByte<EVerticalAlignment> MyVerticalAlignment;

public:
	//根据数据自定义函数
	UFUNCTION(BlueprintCallable, Category="Layout|Overlay Slot")
	void SetPadding(FMargin InPadding);

	UFUNCTION(BlueprintCallable, Category = "Layout|Overlay Slot")
		void SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment);

	UFUNCTION(BlueprintCallable, Category = "Layout|Overlay Slot")
		void SetVerticalAlignment(EVerticalAlignment InVerticalAlignment);

	//修改细节面板会回调该函数
	virtual void SynchronizeProperties() override;

	
	//创建插槽
	virtual void BuildSlot(TSharedRef<SSlateMyPanel> InOverlay);

	//必须实现的函数
	//清空资源
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
};
