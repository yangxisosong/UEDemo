#include "GameplayFramwork/DLPlayerControllerBase.h"

#include "DLAssetManager.h"
#include "DLLocalPlayer.h"
#include "DLLocalPlayerGameplaySubsystem.h"
#include "DLPlayerSetupInfo.h"
#include "IDLGameSavingSystem.h"
#include "IDLPlayerSavingAccessor.h"
#include "Misc/RuntimeErrors.h"
#include "GameplayCoreLog.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayFramwork/DLCharacterBase.h"
#include "GameplayFramwork/DLGameModeBase.h"
#include "GameplayFramwork/DLPlayerStateBase.h"
#include "GameplayFramwork/Component/DLCharacterStateExtensionComp.h"
#include "Types/DLCharacterAssetDef.h"

void ADLPlayerControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}

void ADLPlayerControllerBase::OnUnPossess()
{
	Super::OnUnPossess();

}

void ADLPlayerControllerBase::TryTurnToRotation(const FRotator Rotator, bool ImmediatelyTurn, float OverrideYawSpeed)
{
	const auto MyCharacter = GetCharacterBase();
	if (MyCharacter)
	{
		MyCharacter->TurnToTargetRotation(Rotator, ImmediatelyTurn, OverrideYawSpeed);
	}
	else
	{
		UE_LOG(LogDLGameplayCore, Warning, TEXT("获取 Controller 所属的 Character 失败，调用时机错误"));
	}
}

void ADLPlayerControllerBase::ServerRestartPlayer_Implementation()
{
	ADLGameModeBase* GameMode = Cast<ADLGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!GameMode->PlayerCanRestart(this))
	{
		return;
	}

	if (GetPawn() != nullptr)
	{
		UnPossess();
	}

	if (!PlayerState)
	{
		return;
	}

	GameMode->RestartPlayer(this);

	GameMode->SetupPlayerCharacter(this);
}



bool ADLPlayerControllerBase::ReadPlayerSetupInfo(FDLPlayerSetupInfo& OutSetup)
{
	IDLGameSavingSystem* GameSavingSystem = IDLGameSavingSystem::Get(this);
	IDLPlayerSavingAccessor* PlayerSavingAccessor = GameSavingSystem->GetPlayerSavingAccessor();

	if (ensureAlwaysMsgf(PlayerSavingAccessor, TEXT("IDLPlayerSavingAccessor 接口是空的, 玩家丢失数据")))
	{
		return PlayerSavingAccessor->ReadSetupInfo(OutSetup);
	}

	return false;
}

void ADLPlayerControllerBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (GetLocalPlayer())
	{
		const auto GameplaySys = UDLLocalPlayerGameplaySubsystem::GetInstanceOnlyLocalPlayer(this);
		if (GameplaySys)
		{
			GameplaySys->SetPlayerState(GetPlayerStateBase());
		}
	}

	const auto Extension = GetPlayerStateBase()->FindComponentByClass<UDLCharacterStateExtensionComp>();
	if (Extension)
	{
		Extension->OnDied_CallAndRegister(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnCharacterDied));
	}
}

void ADLPlayerControllerBase::InitPlayerState()
{
	Super::InitPlayerState();

	const auto Extension = PlayerState->FindComponentByClass<UDLCharacterStateExtensionComp>();
	if (Extension)
	{
		Extension->OnDied_CallAndRegister(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnCharacterDied));
	}
}

void ADLPlayerControllerBase::CleanupPlayerState()
{
	const auto Extension = PlayerState->FindComponentByClass<UDLCharacterStateExtensionComp>();
	if (Extension)
	{
		Extension->OnDied_Unregister(this);
	}

	Super::CleanupPlayerState();
}


ADLPlayerStateBase* ADLPlayerControllerBase::GetPlayerStateBase() const
{
	return GetPlayerState<ADLPlayerStateBase>();
}

void ADLPlayerControllerBase::OnCharacterDied()
{
	if (HasAuthority())
	{
		// 标记角色死亡
		GetCharacterBase()->ServerSetCharacterDied();

		// 移除玩家的控制
		this->UnPossess();
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ADLPlayerControllerBase::ServerSetupCharacter(const FDLPlayerSetupInfo* InSetupInfo, ADLCharacterBase* InCharacter,
												   ADLPlayerStateBase* InPlayerState)
{
	ADLPlayerStateBase::FPlayerBaseInfo Arg;
	Arg.Pawn = InCharacter;

	const auto Extension = InPlayerState->FindComponentByClass<UDLCharacterStateExtensionComp>();
	if (Extension)
	{
		Extension->ServerSetup();
	}

	const FDLPlayerSetupInfo& SetupInfo = *InSetupInfo;

	for (const auto& Weapon : SetupInfo.WeaponInfoArray)
	{
		auto& WeaponInfo = Arg.CharacterWeaponInfo.AddZeroed_GetRef();
		WeaponInfo.WeaponAssetId = Weapon.WeaponAssetId;

		for (const auto& Info : Weapon.WeaponAttachInfos)
		{
			auto& AddInfo = WeaponInfo.WeaponAttachInfos.AddZeroed_GetRef();
			AddInfo.WeaponId = Info.WeaponId;
			AddInfo.BoneSocketTag = Info.BoneSocketTag;
		}
	}

	for (const auto& Value : SetupInfo.StartupGameplayAbilityArr)
	{
		ADLPlayerStateBase::FAbilityInfo Info;
		Info.Class = Value.AbilityClass.LoadSynchronous();
		Info.Level = Value.InitLevel;
		Info.IsActiveWhenGive = Value.IsActiveWhenGive;

		Arg.AbilityInfos.Add(Info);
	}

	for (const auto& Value : SetupInfo.StartupGameplayEffects)
	{
		ADLPlayerStateBase::FAbilityInfo Info;
		Info.Class = Value.EffectClass.LoadSynchronous();
		Info.Level = Value.InitLevel;
		Arg.GameplayEffectInfos.Add(Info);
	}

	// 初始化状态数据
	if (!InPlayerState->ServerInitPlayerBaseInfo(Arg))
	{
		ensureAlwaysMsgf(false, TEXT("ServerInitPlayerBaseInfo failed"));
	}

	const UDLAssetManager& AssetManager = UDLAssetManager::Get();
	auto Asset = AssetManager.GetPrimaryAssetObject(InPlayerState->GetCharacterInfoBase().CharacterAssetId);

	//// 初始化角色
	InCharacter->InitCharacter(InPlayerState->GetCharacterInfoBase(), Cast<UDLCharacterAsset>(Asset));
}



ADLCharacterBase* ADLPlayerControllerBase::GetCharacterBase() const
{
	return Cast<ADLCharacterBase>(GetPawn());
}
