#pragma once

#include "CoreMinimal.h"
#include "SSlateMyPanel.h"
#include "Components/PanelWidget.h"
#include "Components/PanelSlot.h"
#include "TestUPanel.generated.h"

UCLASS()
class CUSTOMWIDGET_API UTestUPanel: public UPanelWidget
{
	GENERATED_BODY()
public:
	//构造函数之后执行 在这对数据初始化 防止数据与UI不同步
	virtual void SynchronizeProperties() override;
	//释放资源
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;


#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
	
protected:
	//初始化控件 打开和编译蓝图时会调用
	virtual TSharedRef<SWidget> RebuildWidget() override;

	//插槽事件
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* Slot) override;
	virtual void OnSlotRemoved(UPanelSlot* Slot) override;

	//slate对象
	TSharedPtr<SSlateMyPanel> Myui;
};
