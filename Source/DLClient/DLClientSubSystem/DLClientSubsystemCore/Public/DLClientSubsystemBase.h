// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLClientRuntimeStageDef.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GameFramework/PlayerController.h"
#include "DLClientSubsystemBase.generated.h"


struct FClientSubsystemCollection;
class UDLClientSubsysManager;

USTRUCT()
struct FDLClientSubsysCustomLifeArg
{
	GENERATED_BODY()
public:

	UPROPERTY()
		UDLClientSubsysManager* Manager = nullptr;

	UPROPERTY()
		EDLClientRuntimeStage CurrentStage = EDLClientRuntimeStage::None;
};


/**
 * ClientSubsystem 用来处理游戏客户端各种业务逻辑
 *
 * 这些子系统的生命周期是完全被框架自动托管的，依托于客户端的 EDLClientRuntimeStage 进行创建以及销毁
 *
 * 注意:
 *		这些子系统是可动态拆卸的，所以设计具体的业务逻辑时需要具体判断是否能够获取到该系统;
 *		子系统不应该依赖于 UI 模块，正确的做法是 UI 模块来依赖 子系统；
 *		如果子系统确实需要控制一些 UI 的展示，请使用 接口 + UWidget 的方式进行抽象，总之 子系统模块不能出现任何项目的UI模块;
 */
UCLASS(Abstract, Blueprintable)
class DLCLIENTSUBSYSTEMCORE_API UDLClientSubsystemBase
			: public UObject
			, public FTickableGameObject
{

	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
		EDLClientRuntimeStage InitStage = EDLClientRuntimeStage::None;

	UPROPERTY(EditDefaultsOnly)
		EDLClientRuntimeStage DestroyStage = EDLClientRuntimeStage::None;

public:

	virtual bool CustomShouldCreateInstance(const FDLClientSubsysCustomLifeArg& Arg) const { return false; }

	virtual bool CustomShouldDestroyInstance(const FDLClientSubsysCustomLifeArg& Arg) const { return false; }

	virtual void InitSubsystem(const FClientSubsystemCollection& Collection) { }

	virtual void UninitSubsystem() { }


protected:

	virtual void Tick(float DeltaTime) override { }

	virtual ETickableTickType GetTickableTickType() const override;

	virtual bool IsTickable() const override;

	virtual TStatId GetStatId() const override;

public:

	UGameInstance* GetGameInstance() const;

	APlayerController* GetPlayerControllerChecked() const;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bEnabledTick = false;
};
