#pragma once
#include "CoreMinimal.h"
#include "DLUIExtensionPoint.h"
#include "DLUIExtensionWidgetInterface.h"
#include "Blueprint/UserWidget.h"
#include "DLUIExtensionWidget.generated.h"

/**
 *  在扩展点上动态生成的UI控件
 *	UI扩展点上只支持该类型的控件生成
 */
UCLASS(Blueprintable)
class DLUIMANAGERSUBSYSTEM_API UDLUIExtensionWidget
	:public UUserWidget
	,public IDLExtensionUserWidgetInterface 
{
	GENERATED_BODY()

public:
	//IDLExtensionUserWidgetInterface
	virtual void OnExtensionAction(const EDLUIExtensionActionType ActionType) override;

	virtual void OnWidgetInstance(const FDLUIExtensionActionPayloadData& PayloadData) override;

	virtual void OnWidgetDestroy(const FDLUIExtensionActionPayloadData& PayloadData) override;
	//IDLExtensionUserWidgetInterface

	UFUNCTION(BlueprintNativeEvent)
		void OnAddWidgetToPoint(bool IsShow);

private:

};


