// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLUIExtensionTypeDef.h"
#include "UObject/Interface.h"
#include "DLUIExtensionSysInterface.generated.h"


DECLARE_DELEGATE_RetVal(bool, FCanInstanceExtensionWidget);

USTRUCT(BlueprintType)
struct FUIExtensionAction
{
	GENERATED_BODY()
public:
	bool IsShow = false;
};


/**
* @brief UI 扩展系统接口
*/
UCLASS()
class DLUIMANAGERSUBSYSTEM_API UDLUIExtensionSystemInterface : public UObject
{
	GENERATED_BODY()
public:


	/**
	* @brief 注册扩展点 到 UI 扩展系统中
	* @param PointDesc
	*/
	virtual FDLUIExtensionHandle RegisterExtensionPoint(const FDLUIExtensionPointDesc& PointDesc);


	/**
	* @brief 注销扩展点
	* @param Handle
	*/
	virtual void UnregisterExtensionPoint(FDLUIExtensionHandle Handle);



public:

	/**
	* @brief 注册要扩展的 Widget信息，并且会根据 InstanceCondition 进行创建
	* @param WidgetDesc
	* @param Condition
	*/
	UFUNCTION(BlueprintCallable)
		virtual void RegisterExtensionWidget(FDLUIExtensionWidgetDesc WidgetDesc, const FDLUIExtensionWidgetCondition& Condition);


	/**
	* @brief 注册要扩展的 Widget信息，并且会调用 CheckCanInstanceWidget 来立刻检查是否应该进行实例化，以确保小部件因为错过 InstanceCondition 而无法创建
	* @param WidgetDesc
	* @param Condition
	* @param CheckCanInstanceWidget
	*/
	virtual void RegisterOrInstanceExtensionWidget(
		FDLUIExtensionWidgetDesc WidgetDesc,
		const FDLUIExtensionWidgetCondition& Condition,
		const FCanInstanceExtensionWidget& CheckCanInstanceWidget
	);


	/**
	* @brief 直接实例化 Widget 在扩展点
	* @param WidgetDesc
	* @return
	*/
	UFUNCTION(BlueprintCallable)
		virtual FDLUIExtensionHandle InstanceExtensionWidget(FDLUIExtensionWidgetDesc WidgetDesc);


	/**
	* @brief 直接移除 Widget 在扩展点
	* @param Handle
	*/
	UFUNCTION(BlueprintCallable)
		virtual void DestroyExtensionWidget(FDLUIExtensionHandle Handle);


	/**
	 * @brief 根据Tag获取已经生成的UI控件
	 * @param Tag 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
		virtual UUserWidget* GetWidgetWithTag(FGameplayTag Tag);

	/**
	* @brief 根据Tag 显示UI控件（用于无消息发送时的情况）
	* @param Tag
	* @return
	*/
	UFUNCTION(BlueprintCallable)
		virtual void SetWidgetVisibleWithTag(FGameplayTag Tag, bool IsShow);

};

