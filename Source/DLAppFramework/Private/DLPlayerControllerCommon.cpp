#include "DLPlayerControllerCommon.h"
#include "DLLocalPlayer.h"


ADLPlayerControllerCommon::ADLPlayerControllerCommon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADLPlayerControllerCommon::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (UDLLocalPlayer* LocalPlayer = Cast<UDLLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerControllerSet.Broadcast(LocalPlayer, this);
	}
}