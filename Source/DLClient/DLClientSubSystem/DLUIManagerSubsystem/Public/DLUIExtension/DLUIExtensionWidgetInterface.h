#pragma once
#include "GameplayTags.h"
#include "DLUIExtensionWidgetInterface.generated.h"


struct FDLUIExtensionActionPayloadData;

enum class EDLUIExtensionActionType : uint8;


// 支持扩展的 Widget Base
UINTERFACE(MinimalAPI)
class UDLExtensionUserWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class DLUIMANAGERSUBSYSTEM_API IDLExtensionUserWidgetInterface : public IInterface
{
	GENERATED_BODY()

public:

	/**
	 * @brief 获取这个 Widget 的 Tag
	 * @return 
	 */
	virtual FGameplayTagContainer GetExtensionWidgetTags() { return FGameplayTagContainer{}; }


	/**
	* @brief 这个函数做为这个小部件 被添加或者移除 的回调，子类可以用其进行一些 逻辑控制
	* @param ActionType
	*/
	virtual void OnExtensionAction(const EDLUIExtensionActionType ActionType);


	/**
	* @brief 当窗口实例化时 调用
	* @param PayloadData 获取负载数据
	*/
	virtual void OnWidgetInstance(const FDLUIExtensionActionPayloadData& PayloadData)
	{

	}

	/**
	* @brief 当窗口移除时 调用
	* @param PayloadData 获取负载数据
	*/
	virtual void OnWidgetDestroy(const FDLUIExtensionActionPayloadData& PayloadData)
	{

	}
};
