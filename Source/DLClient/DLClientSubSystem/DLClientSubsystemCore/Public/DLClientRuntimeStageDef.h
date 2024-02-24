#pragma once

#include "CoreMinimal.h"
#include "DLClientRuntimeStageDef.generated.h"

UENUM(BlueprintType)
enum class EDLClientRuntimeStage : uint8
{
	None,

	// 游戏实例创建完成
	GameInstanceInitPost,

	// 本地玩家创建
	LocalPlayerInitPost,

	// 登录完成
	PlayLoginComplate,

	// 前端初始化
	FrontEndInit,

	// 前端初始化
	FrontEndDeInit,

	// 游戏玩法相关的数据准备完成
	GameplayReady,

	// 游戏玩法结束
	GameplayEnd,
};
