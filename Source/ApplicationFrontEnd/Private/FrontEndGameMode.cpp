#include "ApplicationFrontEnd/Public/FrontEndGameMode.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "AppFrameworkMessageDef.h"

AFrontEndGameMode::AFrontEndGameMode()
{
}

void AFrontEndGameMode::BeginPlay()
{
	Super::BeginPlay();

	auto& MsgSys = UGameplayMessageSubsystem::Get(this);
	MsgSys.BroadcastMessage(EFrameworkMsgType::FrontEndInit, FDLAppFrameworkMsg{});
}

void AFrontEndGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	auto& MsgSys = UGameplayMessageSubsystem::Get(this);
	MsgSys.BroadcastMessage(EFrameworkMsgType::FrontEndDeinit, FDLAppFrameworkMsg{});

	Super::EndPlay(EndPlayReason);
}
