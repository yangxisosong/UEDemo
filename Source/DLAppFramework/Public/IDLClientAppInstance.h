#pragma once

#include "CoreMinimal.h"
#include "IDLClientAppInstance.generated.h"


class IDLLocalPlayerGameplayInterface;
class ICharacterStateListener;
class ICharacterStateAccessor;

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class DLAPPFRAMEWORK_API UDLClientAppInstance : public UInterface
{
	GENERATED_BODY()
};


/**
 * @brief 客户端实例的 接口
 *
 *	通过这一层接口，屏蔽项目代码的复杂性（客户端/服务器 一套代码）
 *	专注与获取 本地客户端相关的对象 以及处理他们的业务逻辑
 */
class DLAPPFRAMEWORK_API IDLClientAppInstance
{
	GENERATED_BODY()

public:

	virtual IDLLocalPlayerGameplayInterface* GetGameplayInterface() = 0;

	static IDLClientAppInstance* Get(const UObject* WorldContext);

	static IDLClientAppInstance* GetChecked(const UObject* WorldContext);
};





