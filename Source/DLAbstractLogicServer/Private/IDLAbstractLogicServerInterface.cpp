#include "IDLAbstractLogicServerInterface.h"

#include "DefaultAbstractLogicServer/DefaultAbstractLogicServer.h"
#include "Kismet/GameplayStatics.h"

IDLAbstractLogicServerInterface* IDLAbstractLogicServerInterface::Get(const UObject* WorldContext)
{
	const auto Ins = UGameplayStatics::GetGameInstance(WorldContext);
	const auto DefaultGS = UGameInstance::GetSubsystem<UDefaultAbsLogicServer>(Ins);

	return DefaultGS;
}
