#include "GameplayFramwork/DLGameState.h"

#include "ConvertString.h"
#include "DLAssetManager.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "GameplayFramwork/DLCharacterBase.h"
#include "GameplayFramwork/DLGameModeBase.h"
#include "GameplayFramwork/DLPlayerControllerBase.h"
#include "GameplayFramwork/DLPlayerStateBase.h"
#include "GameplayFramwork/Component/DLPlayerStateExtensionComp.h"
#include "Interface/IDLNPCState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Types/DLCharacterAssetDef.h"
#include "AppFrameworkMessageDef.h"
#include "GameplayCoreLog.h"

ADLGameState::ADLGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADLGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ADLGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ADLGameState::CheckCharacterAssetLoadComplate(ADLPlayerStateBase* PS)
{
	if (LoadComplateCharacterIds.Contains(PS->GetCharacterInfoBase().Id))
	{
		const auto PSExt = PS->FindComponentByClass<UDLPlayerStateExtensionComp>();
		if (PSExt)
		{
			if (!PS->IsABot())
			{
				PSExt->OnPlayerPawnInitReady_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate::CreateUObject(this, &ThisClass::OnPawnInitReady));
			}

			PSExt->LoadPlayerAssetComplate(LoadComplateCharacterIds[PS->GetCharacterInfoBase().Id]);
		}
	}
}

void ADLGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (PlayerState->IsInactive())
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("ADLGameState::AddPlayerState [%s] PS (%s)[%s]"), NET_ROLE_STR(this), TO_STR(PlayerState), NET_ROLE_STR(PlayerState));

	// 架构层 这个一定是可以转的  负责应该直接挂掉
	ADLPlayerStateBase* PS = CastChecked<ADLPlayerStateBase>(PlayerState);

	// PS 加入到游戏中，如果资产已经就绪，那么立刻进行初始化，否则等待资产就绪
	// 客户端；不区分 NPC Or Player, NPC 会动态加载

	if (PS->IsLoadCharacterBaseInfo())
	{
		CheckCharacterAssetLoadComplate(PS);
	}
	else
	{
		const auto PSExt = PS->FindComponentByClass<UDLPlayerStateExtensionComp>();
		if (PSExt)
		{
			PSExt->OnPlayerBaseInfoSet_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate::CreateUObject(this, &ThisClass::CheckCharacterAssetLoadComplate));
		}
	}
}


void ADLGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	if (PlayerState->IsInactive())
	{
		return;
	}

	ServerWaitPlayerLoadingNum++;

	UE_LOG(LogDLGameplayCore, Log, TEXT("ADLGameState::RemovePlayerState PlayerId:%d  ServerWaitPlayerLoadingNum %d"),
						PlayerState->GetPlayerId(),
						ServerWaitPlayerLoadingNum
	);
}

void ADLGameState::MulticastGameplayReady_Implementation()
{
	// 在这里 游戏可以直接开始了

	IsGameplayReady = true;

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(EFrameworkMsgType::LocalPlayerGameplayReady, FDLAppFrameworkMsg{});
}


ADLGameModeBase* ADLGameState::ServerGetGm() const
{
	return Cast<ADLGameModeBase>(AuthorityGameMode);
}

const TArray<FDLGameplayPlayerInfo>& ADLGameState::GetGameplayPlayerInfos() const
{
	return GameplayPlayerInfos;
}

void ADLGameState::SetGameplayPlayerInfos(const TArray<FDLGameplayPlayerInfo>& Infos)
{
	GameplayPlayerInfos = Infos;
	ServerWaitPlayerLoadingNum = GameplayPlayerInfos.Num();
}


void ADLGameState::ServerSetPlayerLoadComplate(int32 PlayerId)
{
	ServerWaitPlayerLoadingNum--;

	UE_LOG(LogDLGameplayCore, Log, TEXT("ADLGameState::ServerSetPlayerLoadComplate PlayerId:%d WaitPlayerNum %d "), PlayerId, ServerWaitPlayerLoadingNum);

	if (ServerWaitPlayerLoadingNum <= 0)
	{
		UE_LOG(LogDLGameplayCore, Log, TEXT("MulticastGameplayReady "));
		this->MulticastGameplayReady();
	}
}

void ADLGameState::LoadNPCAsset_Implementation(FName CharacterId)
{
	if (LoadComplateCharacterIds.Contains(CharacterId))
	{
		return;
	}


	const FDTNPCCharacterInfoRow* Info = FindNPCCharacterInfoEnsure(CharacterId);

	if (Info)
	{
		auto& AssetManager = UDLAssetManager::Get();

		TSet<FSoftObjectPath> PathsToLoad;

		TArray<FName> LoadBundles;
		LoadBundles.Add(TEXT("Default"));
		auto Handle = AssetManager.LoadPrimaryAsset(Info->BaseInfo.CharacterAssetId, LoadBundles);
		if (!Handle.IsValid() || Handle->HasLoadCompleted())
		{
			this->ClientLoadNPCCharacterComplate(CharacterId);
		}
		else
		{
			Handle->BindCompleteDelegate(FStreamableDelegate::CreateUObject(this, &ThisClass::ClientLoadNPCCharacterComplate, CharacterId));
		}
	}
}

void ADLGameState::StartLoadPlayerAsset()
{
	for (const auto& PlayerInfo : GetGameplayPlayerInfos())
	{
		this->LoadPlayerAsset(PlayerInfo);
	}
}

const FDTPlayerCharacterInfoRow* ADLGameState::FindPlayerCharacterInfoEnsure(FName CharacterId) const
{
	const auto DT = GetCharacterDataTable<FDTPlayerCharacterInfoRow>(DRCharacter::CreatePlayerDataRegistryId(CharacterId));
	ensureAlwaysMsgf(DT, TEXT("FindCharacterInfo Is None  CharacterId: %s"), *CharacterId.ToString());
	return DT;
}

const FDTNPCCharacterInfoRow* ADLGameState::FindNPCCharacterInfoEnsure(FName CharacterId) const
{
	const auto DT = GetCharacterDataTable<FDTNPCCharacterInfoRow>(DRCharacter::CreateNPCDataRegistryId(CharacterId));
	ensureAlwaysMsgf(DT, TEXT("FindCharacterInfo Is None  CharacterId: %s"), *CharacterId.ToString());
	return DT;
}

ADLPlayerControllerBase* ADLGameState::GetLocalPlayerController() const
{
	return Cast<ADLPlayerControllerBase>(UGameplayStatics::GetGameInstance(this)->GetFirstLocalPlayerController(this->GetWorld()));
}

bool ADLGameState::ClientIsGameplayReady() const
{
	return IsGameplayReady;
}

ADLPlayerStateBase* ADLGameState::FindNPCPlayerState(FName NpcStartPointId) const
{
	for (const auto Player : PlayerArray)
	{
		if (const auto INpc = Cast<IDLNPCStateInterface>(Player))
		{
			if (INpc->GetNPCStartPointId() == NpcStartPointId)
			{
				return Cast<ADLPlayerStateBase>(Player);
			}
		}
	}
	return nullptr;
}

void ADLGameState::ClientLoadNPCCharacterComplate(FName CharacterID)
{
	UE_LOG(LogTemp, Log, TEXT("ADLGameState::ClientLoadNPCCharacterComplate [%s] PlayerID:(%s)"), NET_ROLE_STR(this), *CharacterID.ToString());

	const auto DT = FindNPCCharacterInfoEnsure(CharacterID);
	if (!DT)
	{
		return;
	}

	UDLCharacterAsset* Asset = Cast<UDLCharacterAsset>(UDLAssetManager::Get().GetPrimaryAssetObject(DT->BaseInfo.CharacterAssetId));

	LoadComplateCharacterIds.Add(CharacterID, Asset);

	// 资产加载完毕，检查是否能够初始化 PS Pawn
	this->CheckAllPlayerAssetLoadComplate(CharacterID);
}

void ADLGameState::LoadPlayerAsset(const FDLGameplayPlayerInfo& PlayerInfo)
{
	UE_LOG(LogTemp, Log, TEXT("ADLGameState::LoadPlayerAsset [%s] PlayerId:(%d)"), NET_ROLE_STR(this), PlayerInfo.PlayerID);

	// 正在加载 就可以坐享其成了，没必要再加载
	if (LoadingCharacterID.Contains(PlayerInfo.CharacterId))
	{
		return;
	}

	const auto DT = FindPlayerCharacterInfoEnsure(PlayerInfo.CharacterId);
	if (!DT)
	{
		return;
	}


	LoadingCharacterID.Add(PlayerInfo.CharacterId);

	UDLAssetManager& AssetManager = UDLAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	TSet<FSoftObjectPath> RawAssetList;


	BundleAssetList.Add(DT->BaseInfo.CharacterAssetId);

	TArray<FName> BundlesToLoad;

	const ENetMode OwnerNetMode = GetNetMode();
	const bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
	const bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);

	if (bLoadClient)
	{
		BundlesToLoad.Add("Client");
	}

	if (bLoadServer)
	{
		BundlesToLoad.Add("Server");
	}

	BundlesToLoad.Add("Default");

	const TSharedPtr<FStreamableHandle> BundleLoadHandle = AssetManager.ChangeBundleStateForPrimaryAssets(
		BundleAssetList.Array(),
		BundlesToLoad,
		{},
		false,
		FStreamableDelegate(),
		FStreamableManager::AsyncLoadHighPriority
	);


	const TSharedPtr<FStreamableHandle> RawLoadHandle = AssetManager.LoadAssetList(RawAssetList.Array(), FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority, TEXT("LoadPlayerAsset()"));

	TSharedPtr<FStreamableHandle> Handle = nullptr;
	if (BundleLoadHandle.IsValid() && RawLoadHandle.IsValid())
	{
		Handle = AssetManager.GetStreamableManager().CreateCombinedHandle({ BundleLoadHandle, RawLoadHandle });
	}
	else
	{
		Handle = BundleLoadHandle.IsValid() ? BundleLoadHandle : RawLoadHandle;
	}

	const FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::ClientLoadPlayerAssetComplate, PlayerInfo.CharacterId);
	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);
	}
}

void ADLGameState::CheckAllPlayerAssetLoadComplate(FName CharacterID)
{
	auto CopyPsArray = PlayerArray;
	for (const auto PlayerSate : CopyPsArray)
	{
		const auto PS = Cast<ADLPlayerStateBase>(PlayerSate);
		if (PS)
		{
			if (CharacterID == PS->GetCharacterInfoBase().Id)
			{
				CheckCharacterAssetLoadComplate(PS);
			}
		}
	}
}

void ADLGameState::ClientLoadPlayerAssetComplate(FName CharacterID)
{
	UE_LOG(LogTemp, Log, TEXT("ADLGameState::ClientLoadPlayerAssetComplate [%s] PlayerID:(%s)"), NET_ROLE_STR(this), *CharacterID.ToString());

	const auto DT = FindPlayerCharacterInfoEnsure(CharacterID);
	if (!DT)
	{
		return;
	}

	UDLCharacterAsset* Asset = Cast<UDLCharacterAsset>(UDLAssetManager::Get().GetPrimaryAssetObject(DT->BaseInfo.CharacterAssetId));

	LoadComplateCharacterIds.Add(CharacterID, Asset);

	// 资产加载完毕，检查是否能够初始化 PS Pawn
	this->CheckAllPlayerAssetLoadComplate(CharacterID);
}

void ADLGameState::OnPawnInitReady(ADLPlayerStateBase* PS)
{
	if (!PS->IsABot())
	{
		LoadingPlayerIds.Remove(PS->GetPlayerId());
	}

	// 加载完成
	if (LoadingPlayerIds.Num() == 0)
	{
		const auto PC = GetLocalPlayerController();
		PS->ClientLoadComplate();
		UE_LOG(LogDLGameplayCore, Log, TEXT("Load Player Complate "));
	}
}


bool ADLGameState::IsServer() const
{
	return GetLocalRole() == ENetRole::ROLE_Authority;
}

bool ADLGameState::IsClient() const
{
	return  !IsServer();
}

void ADLGameState::OnRep_GameplayPlayerInfos()
{
	this->StartLoadPlayerAsset();
}

void ADLGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ADLGameState, GameplayPlayerInfos, COND_InitialOnly, REPNOTIFY_Always);
}
