#include "GameplayFramwork/Component/DLCharacterStateExtensionComp.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "Interface/IDLLocalPlayerGameplayInterface.h"
#include "Net/UnrealNetwork.h"


UDLCharacterStateExtensionComp::UDLCharacterStateExtensionComp()
{
	
}

void UDLCharacterStateExtensionComp::OnDied_CallAndRegister(const FSimpleMulticastDelegate::FDelegate& Delegate)
{
	if (bIsDied)
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		Delegate.ExecuteIfBound();
	}
	else
	{
		OnDied.Add(Delegate);
	}
}

void UDLCharacterStateExtensionComp::OnDied_Unregister(const UObject* BindObj)
{
	OnDied.RemoveAll(BindObj);
}

void UDLCharacterStateExtensionComp::ServerSetCharacterDied()
{
	this->CharacterDiedBroadcast();
}

void UDLCharacterStateExtensionComp::ServerSetup()
{
	bIsDied = false;
}

bool UDLCharacterStateExtensionComp::IsDied() const
{
	return bIsDied;
}


void UDLCharacterStateExtensionComp::BeginPlay()
{
	Super::BeginPlay();

	
}

void UDLCharacterStateExtensionComp::CharacterDiedBroadcast()
{
	bIsDied = true;
	OnDied.Broadcast();
}

void UDLCharacterStateExtensionComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDLCharacterStateExtensionComp, bIsDied, COND_None, REPNOTIFY_Always);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UDLCharacterStateExtensionComp::OnRep_IsDied()
{
	if (bIsDied)
	{
		this->CharacterDiedBroadcast();
	}
}
