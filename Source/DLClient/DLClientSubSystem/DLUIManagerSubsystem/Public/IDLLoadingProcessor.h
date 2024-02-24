#pragma once
#include "CoreMinimal.h"
#include "Components/Widget.h"

#include "IDLLoadingProcessor.generated.h"

enum EDLLoadingProcessorPriority
{
	Low,
	Normal,
	High,
};

/**
* @brief 加载处理者，游戏中有各种各样的 Loading 情况，这个接口用来处理 Loading 时的界面展示
*
* 在 UI 框架中 Loading 会放在统一的层级处理，以防止各种套娃或者错误遮挡其他UI的情况
*
* 注意：这里的 Loading 不包括 打包后加载引擎的 Loading
*/
UINTERFACE()
class DLUIMANAGERSUBSYSTEM_API UDLLoadingProcessor : public UInterface
{
	GENERATED_BODY()
};

class DLUIMANAGERSUBSYSTEM_API IDLLoadingProcessor : public IInterface
{
	GENERATED_BODY()
public:

	/**
	* @brief 检查是否在 Loading 状态
	* @return 
	*/
	virtual bool IsLoading(FString& OutDebugLoadingReason) const = 0;


	/**
	* @brief 获取处理者的名字
	* @return 
	*/
	virtual const FString& GetProcessorName() const = 0;


	/**
	* @brief 创建或者获取需要展示的 Widget
	* @return 
	*/
	virtual TSharedRef<SWidget> GetOrCreateLoadingUI() = 0;


	/**
	* @brief 当展示 Loading UI 时
	*/
	virtual void OnShowLoadingUI() = 0;

	/**
	* @brief 当移除正在展示的UI时
	*/
	virtual void OnRemoveLoadingUI() = 0;


	/**
	* @brief 获取处理者的优先级
	* @return 
	*/
	virtual EDLLoadingProcessorPriority GetPriority() const = 0;
};
