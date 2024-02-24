// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FrontEndGameMode.generated.h"



/**
 * @brief 这个是客户端在启动还没有连接副本地图时 使用的 GM  会存在与客户端本地，与服务器无关
 */
UCLASS()
class APPLICATIONFRONTEND_API AFrontEndGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AFrontEndGameMode();


protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
