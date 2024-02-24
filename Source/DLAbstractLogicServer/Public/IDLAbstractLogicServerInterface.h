// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Json.h"
#include "IDLAbstractLogicServerInterface.generated.h"


struct FDLGameplayServerPlayerInfo : public FJsonSerializable
{
	int32 PlayerId = 0;

	FString CharacterId;

public:

	BEGIN_JSON_SERIALIZER
		JSON_SERIALIZE("PlayerId", PlayerId);
		JSON_SERIALIZE("CharacterId", CharacterId);
	END_JSON_SERIALIZER

};


struct FDLGameplayInfo : public FJsonSerializable
{
	TArray<FDLGameplayServerPlayerInfo> PlayerInfo;

public:

	BEGIN_JSON_SERIALIZER
		JSON_SERIALIZE_ARRAY_SERIALIZABLE("PlayerInfo", PlayerInfo, FDLGameplayServerPlayerInfo);
	END_JSON_SERIALIZER
};


DECLARE_DELEGATE_OneParam(FOnRecvGameplayInfo, const FDLGameplayInfo&);


UINTERFACE()
class UDLAbstractLogicServerInterface : public UInterface
{
	GENERATED_BODY()
};

class DLABSTRACTLOGICSERVER_API IDLAbstractLogicServerInterface
{
	GENERATED_BODY()

public:

	// GS 启动完成
	virtual void GameplayServerSetupReady() {};

	// 等待收到game的详细数据
	virtual FOnRecvGameplayInfo& OnRecvGameplayInfo() { static FOnRecvGameplayInfo Ins; return Ins; };

	// GS 加载完成，这意味着服务准备就绪，可以让玩家进行连接了
	virtual void GameplayServerLoadComplate(const FString& ErrStr) { };

	// 获取接口
	static IDLAbstractLogicServerInterface* Get(const UObject* WorldContext);
};
