#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IDemoGamePlayInterface.h"
#include "DemoGamePlayWidget.generated.h"

class UDLDemoGamePlaySubsystem;
UCLASS()
class DLUIDEMO_API UDLDemoGamePlayWidget :
	public UUserWidget,
	public IDLDemoGamePlayInterface
{
	GENERATED_BODY()
public:
	//IDLIFrontEndInterface
	virtual void InitCurrentSubsystem(UDLDemoGamePlaySubsystem* Subsystem) override;

	virtual void InitPlayerBaseInfo(const FPlayerBaseInfo& PlayerBaseInfo) override;

	virtual void UpdatePlayerBaseInfo(const FPlayerBaseInfo& PlayerBaseInfo) override;

	virtual void InitPlayerAbilityInfo(const TArray<FPlayerAbilityInfo>& PlayerAbilityInfo) override;

	virtual void PlayerAbilityCDChange(const FPlayerAbilityInfo PlayerAbilityInfo) override;

	virtual void InitBossBaseInfo(const FBossBaseInfo& BossBaseInfo) override;

	virtual void UpdateBossBaseInfo(const FBossBaseInfo BossBaseInfo) override;

	virtual void SetButtonTipsShow(bool IsShow, const FDLUIShowButtonTipsMsgBody& Msg) override;

	virtual void KeyClickEvent(FKey Key) override;

	virtual void PlayerBeginBattle() override;

	virtual void PlayerQuiteBattle() override;

	virtual void PlayerDied() override;

	virtual void PlayerRebirth() override;

	virtual void PlayerEnterScene(const FText& SceneName) override;

	virtual void PlayerBattleVictory() override;
	//IDLIFrontEndInterface

	UFUNCTION(BlueprintNativeEvent)
		void OnInitPlayerBaseInfo(const FPlayerBaseInfo& PlayerBaseInfo);

	UFUNCTION(BlueprintNativeEvent)
		void OnUpdatePlayerBaseInfo(const FPlayerBaseInfo& PlayerBaseInfo);

	UFUNCTION(BlueprintNativeEvent)
		void OnInitPlayerAbilityInfo(const TArray<FPlayerAbilityInfo>& PlayerAbilityInfo);

	UFUNCTION(BlueprintNativeEvent)
		void OnPlayerAbilityCDChange(const FPlayerAbilityInfo& PlayerAbilityInfo);

	UFUNCTION(BlueprintNativeEvent)
		void OnInitBossBaseInfo(const FBossBaseInfo& BossBaseInfo);

	UFUNCTION(BlueprintNativeEvent)
		void OnUpdateBossBaseInfo(const FBossBaseInfo& BossBaseInfo);

	UFUNCTION(BlueprintImplementableEvent)
		void OnButtonTipsStateChange(bool IsShow, const FDLUIShowButtonTipsMsgBody& Msg);

	UFUNCTION(BlueprintImplementableEvent)
		void OnKeyClickEvent(FKey Key);

	UFUNCTION(BlueprintImplementableEvent)
		void OnPlayerBeginBattle();

	UFUNCTION(BlueprintImplementableEvent)
		void OnPlayerQuiteBattle();

	UFUNCTION(BlueprintImplementableEvent)
		void OnPlayerDied();

	UFUNCTION(BlueprintImplementableEvent)
		void OnPlayerRebirth();

	UFUNCTION(BlueprintNativeEvent)
		void OnPlayerEnterScene(const FText& SceneName);

	UFUNCTION(BlueprintImplementableEvent)
		void OnPlayerBattleVictory();

	UFUNCTION(BlueprintCallable)
		FBossBaseInfo GetBossBaseInfo();

	UFUNCTION(BlueprintCallable)
		void ReStart();

	UFUNCTION(BlueprintCallable)
		void BackFrontEnd();

private:
	TWeakObjectPtr<UDLDemoGamePlaySubsystem> GamePlaySubsystem;
};
