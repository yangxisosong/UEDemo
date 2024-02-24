// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLUIExtension/DLUIExtensionSysInterface.h"
#include "DLUIManagerSubsystem.generated.h"


class UDLLocalPlayer;
class UDLUIPolicyBase;
class IDLPrimaryLayout;
class UDLUIExtensionSystemBase;

DECLARE_MULTICAST_DELEGATE(FOnPrimaryLayoutReady);


/**
 * 负责从游戏全局管理 UI
 *
 * 这个对象全局只有一个，所以几乎可以在客户端的任何时机进行访问, 不同于 AHUD  HUD 是跟着Map创建与销毁的，当前计划在游戏的不同阶段会切换不同的Map，所以需要一个更加全局的对象
 *
 * 当前主要负责
 * 1. 管理 持有的 RootLayout
 * 2. 连接游戏GamePlay逻辑 与 UI
 * 3. 3D UI 对象的管理
 *
 * 注意 : 这个对象依旧是比较偏向与逻辑层，请不要加入非常表现层的业务
 *
 * @see 文档 https://alidocs.dingtalk.com/i/team/E0Vzgobd8Vv56zJe/docs/E0Vzg8pNeaKM6XJe
 */
UCLASS(Config = Game)
class DLUIMANAGERSUBSYSTEM_API UDLUIManagerSubsystem
	: public UGameInstanceSubsystem
	, public FTickableGameObject
{

	GENERATED_BODY()

public:

	static UDLUIManagerSubsystem* Get(UObject* WorldContext);


	void RegisterLoadingProcessor(TScriptInterface<class IDLLoadingProcessor> Processor);

	void UnRegisterLoadingProcessor(TScriptInterface<class IDLLoadingProcessor> Processor);

	void NotifyPrimaryLayoutReady() const;

public:

	UFUNCTION(BlueprintCallable)
		TScriptInterface<IDLPrimaryLayout> GetPrimaryLayout();

	UFUNCTION(BlueprintCallable)
		UDLUIExtensionSystemInterface* GetUIExtensionSystem();

public:

	FOnPrimaryLayoutReady OnPrimaryLayoutReady;

protected:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UDLUIManagerSubsystem, STATGROUP_Tickables);
	}


	virtual void Tick(float DeltaTime) override;

protected:

	virtual void OnLocalPlayerAdd(ULocalPlayer* LocalPlayer);

	virtual void OnLocalPlayerRemove(ULocalPlayer* LocalPlayer);

private:

	UPROPERTY(Transient)
		UDLUIPolicyBase* PolicyInstance;

	UPROPERTY(Transient)
		UDLUIExtensionSystemInterface* UIExtensionSystem = nullptr;

	UPROPERTY(Config, EditAnywhere)
		TSoftClassPtr<UDLUIPolicyBase> UIPolicyClass;

	UPROPERTY()
		TArray<TScriptInterface<IDLLoadingProcessor>> LoadingProcessors;

	UPROPERTY()
		TScriptInterface<IDLLoadingProcessor> LoadingProcessor;
	UPROPERTY(Config, EditAnywhere)
		TSoftClassPtr<UDLUIExtensionSystemInterface> UIExtensionSystemClass;

private:

	FDelegateHandle RemoveLocalPlayerHandle;

	FDelegateHandle AddLocalPlayerHandle;
};

