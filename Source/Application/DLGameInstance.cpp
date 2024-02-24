
#include "DLGameInstance.h"
#include "AbilitySystemGlobals.h"
#include "AppFrameworkMessageDef.h"
#include "DLAppFrameworkSetting.h"
#include "DLLocalPlayerGameplaySubsystem.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Kismet/GameplayStatics.h"


void UDLGameInstance::Init()
{
	Super::Init();

	UAbilitySystemGlobals::Get().InitGlobalData();


	UGameplayMessageSubsystem* MsgSystem = this->GetSubsystem<UGameplayMessageSubsystem>();
	if (MsgSystem)
	{
		const auto& MsgType = UDLAppFrameworkSetting::Get()->MsgTypeGameInstanceInitPost;
		MsgSystem->BroadcastMessage(MsgType, FDLAppFrameworkMsg { });
	}
}

int32 UDLGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, int32 ControllerId)
{
	const int32 Ret = Super::AddLocalPlayer(NewPlayer, ControllerId);

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(EFrameworkMsgType::LocalPlayerInitPost, FDLAppFrameworkMsg { });

	return Ret;
}

IDLLocalPlayerGameplayInterface* UDLGameInstance::GetGameplayInterface()
{
	return UDLLocalPlayerGameplaySubsystem::GetInstanceOnlyLocalPlayer(this);
}
