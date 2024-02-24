#pragma once
#include "CoreMinimal.h"
#include "DLUIPolicyBase.generated.h"


class IDLPrimaryLayout;


/**
* @brief UI 的策略基类， 这个类只提供 接口规范，具体实现在真正的UI模块
*
*  这个策略主要是解决 跨平台 以及 Debug 与 Release 等存在UI差异的问题
*/
UCLASS(Blueprintable)
class DLUIMANAGERSUBSYSTEM_API UDLUIPolicyBase : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	virtual void OnLocalPlayerReady(class UDLLocalPlayer* LocalPlayer) { }

	virtual void OnLocalPlayerRemove(class UDLLocalPlayer* LocalPlayer) { }

	virtual TScriptInterface<IDLPrimaryLayout> GetPrimaryLayout() { return nullptr; }

	virtual void Init(class UDLUIManagerSubsystem* Manager) { UIManager = Manager; }

protected:

	UPROPERTY()
		class UDLUIManagerSubsystem* UIManager;
};