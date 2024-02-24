#pragma once
#include "CoreMinimal.h"
#include "DLUIExtension/DLUIExtensionSysInterface.h"
#include "DLUIExtension/DLUIExtensionWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "DLUIExtensionSystem.generated.h"

class UDLUIExtensionPoint;

//自定义日志类型的声明
DECLARE_LOG_CATEGORY_EXTERN(LogUIExtensionSystem, Log, All);

UCLASS()
class UDLUIExtensionSystem :
	public UDLUIExtensionSystemInterface
{
private:
	GENERATED_BODY()
public:
	//UDLUIExtensionSystemInterface
	virtual FDLUIExtensionHandle RegisterExtensionPoint(const FDLUIExtensionPointDesc& PointDesc) override;

	//Handle 需要是 RegisterExtensionPoint 返回的
	virtual void UnregisterExtensionPoint(FDLUIExtensionHandle Handle) override;

	virtual void RegisterExtensionWidget(FDLUIExtensionWidgetDesc WidgetDesc,
		const FDLUIExtensionWidgetCondition& Condition) override;

	virtual void RegisterOrInstanceExtensionWidget(FDLUIExtensionWidgetDesc WidgetDesc,
		const FDLUIExtensionWidgetCondition& Condition,
		const FCanInstanceExtensionWidget& CheckCanInstanceWidget) override;

	virtual FDLUIExtensionHandle InstanceExtensionWidget(FDLUIExtensionWidgetDesc WidgetDesc) override;

	//Handle 需要是 InstanceExtensionWidget 返回的
	virtual void DestroyExtensionWidget(FDLUIExtensionHandle Handle) override;

	virtual UUserWidget* GetWidgetWithTag(FGameplayTag Tag) override;

	virtual void SetWidgetVisibleWithTag(FGameplayTag Tag,bool IsShow) override;

	//UDLUIExtensionSystemInterface
private:

	void OnGetMessage(const FGameplayTag Tag, EDLUIExtensionActionType ActionType,
		const UScriptStruct* Type, const void* PayloadData);

	void AddWidgetToPoint(const FGameplayTag Tag, EDLUIExtensionActionType ActionType,
	const UScriptStruct* Type, const void* PayloadData);

	UDLUIExtensionWidget* CreatWidgetByClass(const FGameplayTag Tag,TSoftClassPtr<UUserWidget> ClassPtr);

	TSoftClassPtr<UUserWidget> GetWidgetClassWithTag(FGameplayTag Tag);

	//FDLUIExtensionPointDesc GetExtensionPointDescWithTag(FGameplayTag Tag);

protected:
	UPROPERTY(Transient)
		TArray<FGameplayMessageListenerHandle> ListenerHandles;

	//扩展UI对象
	UPROPERTY(Transient)
		TMap<FGameplayTag, UDLUIExtensionWidget*> WidgetMap;

	//扩展点信息
	UPROPERTY(Transient)
		TMap<FGameplayTag, FDLUIExtensionPointDesc> PointMap;

	//扩展UI信息
	UPROPERTY(Transient)
		TMap<FGameplayTag, FDLUIExtensionWidgetDesc> UIExtensionWidgetInfo;

	// FDLUIExtensionHandle 对应的扩展点信息
	UPROPERTY(Transient)
		TMap<FDLUIExtensionHandle, FDLUIExtensionPointDesc> PointHandleMap;

	// FDLUIExtensionHandle 对应的UI实例信息
	UPROPERTY(Transient)
		TMap<FDLUIExtensionHandle, FGameplayTag> WidgetHandleMap;

	//注册时直接添加窗口 会出现扩展点没有注册的情况
	//将窗口信息缓存 在扩展点生成时添加
	UPROPERTY(Transient)
		TMap<FGameplayTag, EDLUIExtensionActionType> CacheWidgetAction;
};