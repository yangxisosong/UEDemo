#include "DemoGamePlayWidget.h"
#include "DLDemoGamePlaySubsystem.h"

void UDLDemoGamePlayWidget::InitCurrentSubsystem(UDLDemoGamePlaySubsystem* Subsystem)
{
	GamePlaySubsystem = MakeWeakObjectPtr<UDLDemoGamePlaySubsystem>(Subsystem);
}

void UDLDemoGamePlayWidget::InitPlayerBaseInfo(const FPlayerBaseInfo& PlayerBaseInfo)
{
	OnInitPlayerBaseInfo(PlayerBaseInfo);
}

void UDLDemoGamePlayWidget::UpdatePlayerBaseInfo(const FPlayerBaseInfo& PlayerBaseInfo)
{
	OnUpdatePlayerBaseInfo(PlayerBaseInfo);
}

void UDLDemoGamePlayWidget::InitPlayerAbilityInfo(const TArray<FPlayerAbilityInfo>& PlayerAbilityInfo)
{
	OnInitPlayerAbilityInfo(PlayerAbilityInfo);
}

void UDLDemoGamePlayWidget::PlayerAbilityCDChange(const FPlayerAbilityInfo PlayerAbilityInfo)
{
	OnPlayerAbilityCDChange(PlayerAbilityInfo);
}

void UDLDemoGamePlayWidget::InitBossBaseInfo(const FBossBaseInfo& BossBaseInfo)
{
	OnInitBossBaseInfo(BossBaseInfo);
}

void UDLDemoGamePlayWidget::UpdateBossBaseInfo(const FBossBaseInfo BossBaseInfo)
{
	OnUpdateBossBaseInfo(BossBaseInfo);
}

void UDLDemoGamePlayWidget::SetButtonTipsShow(bool IsShow, const FDLUIShowButtonTipsMsgBody& Msg)
{
	OnButtonTipsStateChange(IsShow, Msg);
}

void UDLDemoGamePlayWidget::KeyClickEvent(FKey Key)
{
	OnKeyClickEvent(Key);
}

void UDLDemoGamePlayWidget::PlayerBeginBattle()
{
	OnPlayerBeginBattle();
}

void UDLDemoGamePlayWidget::PlayerQuiteBattle()
{
	OnPlayerQuiteBattle();
}

void UDLDemoGamePlayWidget::PlayerDied()
{
	OnPlayerDied();
}

void UDLDemoGamePlayWidget::PlayerRebirth()
{
	OnPlayerRebirth();
}

void UDLDemoGamePlayWidget::PlayerEnterScene(const FText& SceneName)
{
	OnPlayerEnterScene(SceneName);
}

void UDLDemoGamePlayWidget::PlayerBattleVictory()
{
	OnPlayerBattleVictory();
}

void UDLDemoGamePlayWidget::OnPlayerEnterScene_Implementation(const FText& SceneName)
{
}

FBossBaseInfo UDLDemoGamePlayWidget::GetBossBaseInfo()
{
	if(GamePlaySubsystem.Get())
	{
		return GamePlaySubsystem.Get()->GetBossInfo();
	}
	return FBossBaseInfo();
}

void UDLDemoGamePlayWidget::ReStart()
{
	GamePlaySubsystem->Restart();
}

void UDLDemoGamePlayWidget::BackFrontEnd()
{
	GamePlaySubsystem->BackFrontEnd();
}

void UDLDemoGamePlayWidget::OnUpdateBossBaseInfo_Implementation(const FBossBaseInfo& PlayerAbilityInfo)
{
}

void UDLDemoGamePlayWidget::OnInitBossBaseInfo_Implementation(const FBossBaseInfo& BossBaseInfo)
{
}

void UDLDemoGamePlayWidget::OnInitPlayerBaseInfo_Implementation(const FPlayerBaseInfo& BossBaseInfo)
{
}

void UDLDemoGamePlayWidget::OnInitPlayerAbilityInfo_Implementation(const TArray<FPlayerAbilityInfo>& PlayerAbilityInfo)
{
}

void UDLDemoGamePlayWidget::OnPlayerAbilityCDChange_Implementation(const FPlayerAbilityInfo& PlayerAbilityInfo)
{
}

void UDLDemoGamePlayWidget::OnUpdatePlayerBaseInfo_Implementation(const FPlayerBaseInfo& PlayerBaseInfo)
{

}
