// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLUIPolicyBase.h"
#include "UObject/Object.h"
#include "UDLUIPolicyForDemo.generated.h"

class UDLPrimaryLayoutBase;


/**
 *  Demo 版本的测试 UI 策略
 */
UCLASS()
class DLUICORE_API UDLUIPolicyForDemo
	: public UDLUIPolicyBase
{

	GENERATED_BODY()
public:

	virtual TScriptInterface<IDLPrimaryLayout> GetPrimaryLayout() override;

	virtual void OnLocalPlayerReady(UDLLocalPlayer* LocalPlayer) override;

	virtual void OnLocalPlayerRemove(UDLLocalPlayer* LocalPlayer) override;

private:

	void OnPrimaryLayoutReady(UDLLocalPlayer* LocalPlayer, APlayerController* PlayerController);


public:


	// 游戏的主布局类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<UDLPrimaryLayoutBase> PrimaryLayoutClass;

private:

	UPROPERTY()
		UDLPrimaryLayoutBase* PrimaryLayout = nullptr;
};
