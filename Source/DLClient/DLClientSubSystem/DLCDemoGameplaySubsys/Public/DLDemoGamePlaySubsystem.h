// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "DLClientSubsystemBase.h"
#include "IDemoGamePlayInterface.h"
#include "Components/Widget.h"
#include "DLAbilitySystem/private/TargetData/DLAbilityTargetData.h"
#include "DLUIExtension/DLUIExtensionSysInterface.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Interface/IDLLocalPlayerGameplayInterface.h"
#include "Interface/IDLPrimaryPlayerAbilitySystem.h"
#include "DLDemoGamePlaySubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(Log_DLDemoGamePlay, Log, All);

class ICharacterStateListener;
class ICharacterStateAccessor;
class IDLPrimaryPlayerAbilitySystem;
class UDLUIManagerSubsystem;
class IDLDemoGamePlayInterface;

/**
 * 战斗系统
 */
UCLASS(Abstract, Blueprintable)
class DLCDEMOGAMEPLAYSUBSYS_API UDLDemoGamePlaySubsystem
		: public UDLClientSubsystemBase
{
	GENERATED_BODY()
public:
	void OnPlayerAbilitySysReady();
	virtual void InitSubsystem(const FClientSubsystemCollection& Collection) override;

	virtual void UninitSubsystem() override;

	FBossBaseInfo GetBossInfo();

	void Restart();
	void BackFrontEnd();
private:

	void OnPrimaryLayoutReady();

	void InitPlayerBaseInfo();

	void UpdatePlayerBaseInfo();

	void SetPlayerAbilityInfo();

	void InitUIExtensionSystem();

	void BindAttributeListener();

	void BindAbilityListener();

	void BindUIEventListener();

	void InitBossInfo(IDLLocalPlayerGameplayInterface* GameplayInterface);

	void OnPlayerInfoChange(const FOnAttributeChangeData& Data);

	void OnBossInfoChange(bool IsFocus, const FBossBaseInfo& Data);

	void OnPlayerAbilityChange(const FDLLocalPlayerAbilityApplyCDMsg& Data);

	void OnButtonTipsStateChange(bool IsShow, const FDLUIShowButtonTipsMsgBody& Msg);

	void OnButtonClick(const FDLUITriggerButtonClickMsgBody& Msg);

	void OnPlayerLifeStateChange(bool IsDied);

	void OnPlayerBattleStateChange(bool IsInBattle);

	void OnPlayerEnterScene(const FDLLocalPlayerEnterNewSceneMsgBody& Msg);

	void OnPlayerFocusChange(bool IsLose, const FDLLocalPlayerFocusNewTargetMsgBody& Msg);

	void OnPlayerBattleVictory();
protected:

	// 主界面类型
	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<UWidget> WidgetClass;

	//Tag对应的窗口类
	UPROPERTY(EditDefaultsOnly)
		TArray<FDLUIExtensionWidgetDesc> TagAndWidgetClass;

	UPROPERTY(EditDefaultsOnly)
		FDLUIExtensionWidgetCondition WidgetCondition;

	//技能配置
	UPROPERTY(EditDefaultsOnly)
		TSoftObjectPtr<UDataTable> PlayerAbilityConfig;

	//场景信息
	UPROPERTY(EditDefaultsOnly)
		TSoftObjectPtr<UDataTable> SceneInfo;

	TArray<FPlayerAbilityInfo> PlayerAbilityInfo;

	IDLDemoGamePlayInterface* MainWidget;

	FPlayerBaseInfo PlayerBaseInfo;

	FBossBaseInfo BossBaseInfo;

	FCanInstanceExtensionWidget CanInstanceExtensionWidget;

private:
	TScriptInterface<ICharacterStateListener> CharacterStateListener = nullptr;

	TScriptInterface<ICharacterStateAccessor> CharacterStateAccessor = nullptr;

	TScriptInterface<IDLPrimaryPlayerAbilitySystem> PlayerAbilitySystem = nullptr;

	TMap<FGameplayTag, FDLUIExtWidgetMsgCondition> WidgetMsgConditionMap;

	TMap<FGameplayTag, FGameplayMessageListenerHandle> MessageListenerHandleMap;

	bool IsShowButtonTips = false;

	bool IsShowTipsUI = false;
};
