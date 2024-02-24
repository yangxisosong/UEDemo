// Fill out your copyright notice in the Description page of Project Settings.


#include "DLLocalPlayerGameplaySubsystem.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayFramwork/DLCharacterBase.h"
#include "GameplayFramwork/DLPlayerStateBase.h"
#include "GameplayFramwork/Component/DLCharacterBattleStateComp.h"
#include "GameplayFramwork/Component/DLCharacterStateExtensionComp.h"
#include "GameplayFramwork/Component/DLLocalPlayerAbilitySysComp.h"
#include "GameplayFramwork/Component/DLPlayerStateExtensionComp.h"

TScriptInterface<IDLPrimaryPlayerAbilitySystem> UDLLocalPlayerGameplaySubsystem::GetPrimaryPlayerAbilitySystem()
{
	if (LocalPlayerState)
	{
		return LocalPlayerState->FindComponentByClass<UDLLocalPlayerAbilitySysComp>();
	}
	return nullptr;
}

TScriptInterface <ICharacterStateAccessor> UDLLocalPlayerGameplaySubsystem::GetPrimaryPlayerStateAccessor()
{
	if (LocalPlayerState)
	{
		return LocalPlayerState->GetCharacterBase();
	}

	return nullptr;
}

TScriptInterface <ICharacterStateListener> UDLLocalPlayerGameplaySubsystem::GetPrimaryPlayerStateListener()
{
	return LocalPlayerState;
}

TScriptInterface<ICharacterStateAccessor> UDLLocalPlayerGameplaySubsystem::GetEnemyUnitStateAccessor()
{
	if(EnemyUnitActor)
	{
		if (const auto PS = Cast<APlayerState>(EnemyUnitActor))
		{
			return PS->GetPawn();
		}
	}
	return nullptr;
}

TScriptInterface<ICharacterStateListener> UDLLocalPlayerGameplaySubsystem::GetEnemyUnitStateListener()
{
	if (EnemyUnitActor)
	{
		return EnemyUnitActor;
	}
	return nullptr;
}

void UDLLocalPlayerGameplaySubsystem::OnLocalPlayerGameplayReady_CallOrRegister(
	const FSimpleMulticastDelegate::FDelegate& Delegate)
{
	if (IsGameplayReady())
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		Delegate.ExecuteIfBound();
	}
	else
	{
		OnGameplayReady.Add(Delegate);
	}
}

void UDLLocalPlayerGameplaySubsystem::OnLocalPlayerGameplayReady_Unregister(UObject* BindObject)
{
	OnGameplayReady.RemoveAll(BindObject);
}

void UDLLocalPlayerGameplaySubsystem::OnPlayerAbilitySysReady_CallOrRegister(
	const FSimpleMulticastDelegate::FDelegate& Delegate)
{
	if (GetPrimaryPlayerAbilitySystem() != nullptr)
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		Delegate.ExecuteIfBound();
	}
	else
	{
		OnPlayerAbilitySysReady.Add(Delegate);
	}
}

void UDLLocalPlayerGameplaySubsystem::OnPlayerAbilitySysReady_Unregister(UObject* BindObject)
{
	OnPlayerAbilitySysReady.RemoveAll(BindObject);
}

bool UDLLocalPlayerGameplaySubsystem::IsInBattleState()
{
	return bInBattleState;
}

bool UDLLocalPlayerGameplaySubsystem::IsGameplayReady()
{
	return bIsReady;
}

bool UDLLocalPlayerGameplaySubsystem::HasEnemyUnitFocus()
{
	if (EnemyUnitActor)
	{
		return true;
	}
	return false;
}

void UDLLocalPlayerGameplaySubsystem::PlayerReborn()
{
	if (!ICharacterStateAccessor::Execute_K2_IsDied(GetPrimaryPlayerStateAccessor().GetObject()))
	{
		return;
	}

	// 重生
	const auto PC = GetLocalPlayerChecked<>()->GetPlayerController(this->GetWorld());
	if (PC)
	{
		PC->ServerRestartPlayer();
	}
}

void UDLLocalPlayerGameplaySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);



}

void UDLLocalPlayerGameplaySubsystem::Deinitialize()
{
	Super::Deinitialize();

}

bool UDLLocalPlayerGameplaySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}



void UDLLocalPlayerGameplaySubsystem::SetPlayerState(ADLPlayerStateBase* PS)
{
	if (PS == LocalPlayerState)
	{
		return;
	}

	if (LocalPlayerState)
	{
		const auto PSExt = LocalPlayerState->FindComponentByClass<UDLPlayerStateExtensionComp>();
		if (PSExt)
		{
			PSExt->OnPlayerPawnInitReady_Unregister(this);
		}
	}

	LocalPlayerState = PS;

	const auto PSExt = LocalPlayerState->FindComponentByClass<UDLPlayerStateExtensionComp>();
	if (PSExt)
	{
		PSExt->OnPlayerPawnInitReady_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate::CreateUObject(this, &ThisClass::OnPlayerPawnInitReady));

		PSExt->OnPlayerAbilitySystemInit_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate::CreateWeakLambda(this, [this](auto P) { OnPlayerAbilitySysReady.Broadcast(); }));
	}

	const auto CharacterStateExt = LocalPlayerState->FindComponentByClass<UDLCharacterStateExtensionComp>();
	if (CharacterStateExt)
	{
		CharacterStateExt->OnDied_CallAndRegister(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnPawnDied));
	}


	const auto PSBattleStateComp = LocalPlayerState->FindComponentByClass<UDLCharacterBattleStateComp>();
	if (PSBattleStateComp)
	{
		PSBattleStateComp->OnCharacterBattleStateChange.AddDynamic(this, &ThisClass::OnBattleStateChange);

		PSBattleStateComp->OnChangeAsEnemyUnit.AddUObject(this, &ThisClass::OnChangeAsEnemyUnit);

		if (PSBattleStateComp->GetAsEnemyActor())
		{
			this->OnChangeAsEnemyUnit(PSBattleStateComp->GetAsEnemyActor());
		}
	}
}


void UDLLocalPlayerGameplaySubsystem::OnBattleStateChange(const bool InBattle)
{
	bInBattleState = InBattle;

	auto& MsgSys = UGameplayMessageSubsystem::Get(this);

	if (bInBattleState)
	{
		MsgSys.BroadcastMessage(EDLGameplayMsgType::LocalPlayer_Gameplay_InBattleState, FDLGameplayMsgBody{});
	}
	else
	{
		MsgSys.BroadcastMessage(EDLGameplayMsgType::LocalPlayer_Gameplay_OutBattleState, FDLGameplayMsgBody{});
	}
}


void UDLLocalPlayerGameplaySubsystem::OnPlayerPawnInitReady(ADLPlayerStateBase* PS)
{
	bIsReady = true;
	OnGameplayReady.Broadcast();

	auto& MsgSys = UGameplayMessageSubsystem::Get(this);
	MsgSys.BroadcastMessage(EDLGameplayMsgType::LocalPlayer_Gameplay_PlayerStart, FDLGameplayMsgBody{});
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UDLLocalPlayerGameplaySubsystem::OnPawnDied()
{
	auto& MsgSys = UGameplayMessageSubsystem::Get(this);
	MsgSys.BroadcastMessage(EDLGameplayMsgType::LocalPlayer_Gameplay_PlayerDied, FDLGameplayMsgBody{});
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UDLLocalPlayerGameplaySubsystem::OnChangeAsEnemyUnit(AActor* Actor)
{
	auto& MsgSys = UGameplayMessageSubsystem::Get(this);
	if (Actor)
	{
		EnemyUnitActor = Actor;
		FDLLocalPlayerFocusNewTargetMsgBody Body;
		Body.FocusTarget = Actor;
		Body.StateListener = Actor;
		if (const auto PS = Cast<APlayerState>(Actor))
		{
			Body.StateAccessor = PS->GetPawn();
		}

		MsgSys.BroadcastMessage(EDLGameplayMsgType::LocalPlayer_Gameplay_FocusNewTarget, Body);
	}
	else
	{
		MsgSys.BroadcastMessage(EDLGameplayMsgType::LocalPlayer_Gameplay_LoseTarget, FDLGameplayMsgBody{});
	}
}
