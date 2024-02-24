// Copyright Epic Games, Inc. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "GASDemoGameMode.h"

#include "AIController.h"
#include "DLAssetManager.h"
#include "DT/DTHeroInfo.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "GameplayCoreLog.h"
#include "DataRegistry.h"
#include "DataRegistrySubsystem.h"
#include "DLGameplayLaunchSetting.h"
#include "DLPlayerSetupInfo.h"
#include "IDLAbstractLogicServerInterface.h"
#include "IDLGameSavingSystem.h"
#include "IDLPlayerSavingAccessor.h"
#include "GameplayFramwork/DLAIControllerBase.h"
#include "GameplayFramwork/DLPlayerControllerBase.h"
#include "GameplayFramwork/DLPlayerStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Types/DLCharacterAssetDef.h"
#include "GameplayFramwork/DLGameState.h"
#include "Interface/IDLNPCState.h"

AGASDemoGameMode::AGASDemoGameMode()
{
}


void AGASDemoGameMode::CreateAI(class ADLPlayerControllerBase* PlayerController, int32 HeroId, FString StartPointTag)
{
	if (!CharacterDataTable)
	{
		return;
	}

	UDataRegistrySubsystem* DRSys = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>();
	if (!DRSys)
	{
		return;
	}

	const FDTNPCCharacterInfoRow* Info = DRSys->GetCachedItem<FDTNPCCharacterInfoRow>(DRCharacter::CreateNPCDataRegistryId(FName(*FString::FromInt(HeroId))));

	if (Info)
	{
		auto& AssetManager = UDLAssetManager::Get();

		TSet<FSoftObjectPath> PathsToLoad;

		TArray<FName> LoadBundles;
		LoadBundles.Add(TEXT("Default"));
		AssetManager.GetPrimaryAssetLoadSet(PathsToLoad, Info->BaseInfo.CharacterAssetId, LoadBundles, true);

		for (const auto& ObjPath : PathsToLoad)
		{
			if (ObjPath.IsValid())
			{
				auto Obj = ObjPath.TryLoad();
				check(Obj);
				HoldLoadAssets.Add(Obj);
			}
		}

		UDLCharacterAsset* CharacterAsset = Cast<UDLCharacterAsset>(AssetManager.GetPrimaryAssetObject(Info->BaseInfo.CharacterAssetId));
		if (CharacterAsset)
		{
			AActor* StartSpot = FindPlayerStart(PlayerController, StartPointTag);

			if (StartSpot)
			{
				const FVector Location = StartSpot->GetActorLocation();
				FRotator Rotator = FRotator::ZeroRotator;
				Rotator.Yaw = StartSpot->GetActorRotation().Yaw;
				APawn* NewPawn = GetWorld()->SpawnActor<APawn>(CharacterAsset->CharacterClass.LoadSynchronous(), Location,
															   Rotator);
				if (!NewPawn)
				{
					return;
				}

				ADLAIControllerBase* AIController = GetWorld()->SpawnActor<ADLAIControllerBase>(NewPawn->AIControllerClass);

				ADLPlayerStateBase* PS = AIController->GetPlayerState<ADLPlayerStateBase>();
				PS->SetCharacterInfoBase(Info->BaseInfo);

				AIController->ServerSetupCharacter(CharacterAsset, Cast<ADLCharacterBase>(NewPawn), PS);
			}
		}

		GetGameState()->LoadNPCAsset(Info->BaseInfo.Id);
	}
}

void AGASDemoGameMode::RemoveAllAI()
{
	TArray<AActor*> NPCList;
	UGameplayStatics::GetAllActorsOfClass(this, NPCClassBase, NPCList);
	for (const auto& NPC : NPCList)
	{
		NPC->Destroy();
	}
}

void AGASDemoGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	// 目前只有一个角色，一起都简化了

	// 获取存档数据，这里必须是同步的，因为必须再这一帧将数据设置给 PS，客户端大量的依赖  COND_InitOnly
	// 如果接入真正的服务器，需要使用 新增 ServerReady 协议 来确保客户端连接时，已经有存档数据

	IDLGameSavingSystem* GameSavingSystem = IDLGameSavingSystem::Get(this);
	IDLPlayerSavingAccessor* PlayerSavingAccessor = GameSavingSystem->GetPlayerSavingAccessor();

	if (!ensureAlwaysMsgf(PlayerSavingAccessor, TEXT("IDLPlayerSavingAccessor 接口是空的, 玩家丢失数据")))
	{
		ErrorMessage = TEXT("IDLPlayerSavingAccessor 接口是空的, 玩家丢失数据");
	}

	if (!PlayerSavingAccessor->ReadSetupInfo(DefaultSetupInfo))
	{
		ErrorMessage = TEXT("加载存档文件 失败");
	}

	if (!ErrorMessage.IsEmpty())
	{
		return;
	}


	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

FString AGASDemoGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
	const FString& Options, const FString& Portal)
{
	if (!NewPlayerController->IsA<ADLPlayerControllerBase>())
	{
		return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	}


	auto ErrMsg = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	if (!ErrMsg.IsEmpty())
	{
		return ErrMsg;
	}

	// 架构强制保证，一定需要一个正确的 PC
	const ADLPlayerControllerBase* PC = CastChecked<ADLPlayerControllerBase>(NewPlayerController);
	ADLPlayerStateBase* PS = PC->GetPlayerStateBase();

	FName CharacterId = TEXT("1");
	// 解析 Options
	//const FString CharacterId = UGameplayStatics::ParseOption(Options, TEXT("CharacterId"));
	//if (CharacterId.IsEmpty())
	//{
	//	return TEXT("CharacterId 不能为 None");
	//}

	if (GIsEditor)
	{
		const auto TempCharacterID = UDLGameplayLaunchSetting::GetGameplayLaunchSetting()->GetCurrentSelectCharacterID();
		if (!TempCharacterID.IsNone())
		{
			CharacterId = TempCharacterID;
		}
	}

	const UDataRegistrySubsystem* DRSys = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>();
	const FDTCharacterInfoBaseRow* Info = DRSys->GetCachedItem<FDTCharacterInfoBaseRow>(DRCharacter::CreatePlayerDataRegistryId(CharacterId));
	if (!Info)
	{
		return TEXT("无法查询到 角色信息");
	}

	PS->SetCharacterInfoBase(Info->BaseInfo);

	return {};
}

ADLCharacterBase* AGASDemoGameMode::CreateNPC(const FDLCreateNPCArg& Arg)
{
	const FVector Location = Arg.SpawnTransform.GetLocation();
	FRotator Rotator = FRotator::ZeroRotator;
	Rotator.Yaw = Arg.SpawnTransform.Rotator().Yaw;

	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(Arg.Asset->CharacterClass.Get(), Location, Rotator);
	if (!ensureAlwaysMsgf(NewPawn, TEXT("无法创建 NPC Character")))
	{
		return nullptr;
	}


	ADLAIControllerBase* AIController = nullptr;

	for (auto Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		ADLAIControllerBase* FindAIController = Cast<ADLAIControllerBase>(Iterator->Get());
		if (FindAIController)
		{
			if (FindAIController->GetNpcSpawnPointId() == Arg.StartPointId)
			{
				AIController = FindAIController;
				break;
			}
		}
	}

	if (!AIController)
	{
		AIController = GetWorld()->SpawnActor<ADLAIControllerBase>(NewPawn->AIControllerClass);
		AIController->SetNpcSpawnPointId(Arg.StartPointId);

		IDLNPCStateInterface* PStateInterface = AIController->GetPlayerState<IDLNPCStateInterface>();
		if (PStateInterface)
		{
			PStateInterface->SetNPCStartPointId(Arg.StartPointId);
		}
	}

	// ReSharper disable once CppLocalVariableMayBeConst
	auto GS = GetGameState();

	const auto PS = AIController->GetPlayerState<ADLPlayerStateBase>();

	const auto NPCDT = GS->FindNPCCharacterInfoEnsure(Arg.NPCCharacterId);
	if (!NPCDT)
	{
		return nullptr;
	}

	ADLCharacterBase* Character = CastChecked<ADLCharacterBase>(NewPawn);

	PS->SetCharacterInfoBase(NPCDT->BaseInfo);
	AIController->ServerSetupCharacter(Arg.Asset, Character, PS);

	UE_LOG(LogDLGameplayCoreAI, Log, TEXT("CreateNPC Complate PointId:%s"), *Arg.StartPointId.ToString());

	return Character;
}

bool AGASDemoGameMode::SetupPlayerCharacter(APlayerController* NewPlayer)
{
	if (!NewPlayer->IsA<ADLPlayerControllerBase>())
	{
		return false;
	}

	// 架构强制保证，一定需要一个正确的 PC
	ADLPlayerControllerBase* PC = CastChecked<ADLPlayerControllerBase>(NewPlayer);
	ADLPlayerStateBase* PS = PC->GetPlayerStateBase();
	ADLCharacterBase* CharacterBase = PC->GetCharacterBase();

	if (!ensureAlwaysMsgf(PS && CharacterBase, TEXT("创建失败")))
	{
		return false;
	}

	// 服务端初始化 玩家
	PC->ServerSetupCharacter(&DefaultSetupInfo, CharacterBase, PS);
	return true;
}

void AGASDemoGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	this->SetupPlayerCharacter(NewPlayer);
}

UClass* AGASDemoGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// TODO 这里需要区分 是 Client 还是 模拟的情况
	if (!InController->IsA<ADLPlayerControllerBase>())
	{
		return Super::GetDefaultPawnClassForController_Implementation(InController);
	}

	// 架构强制保证，一定需要一个正确的 PC
	const ADLPlayerControllerBase* PC = CastChecked<ADLPlayerControllerBase>(InController);
	const ADLPlayerStateBase* PS = PC->GetPlayerStateBase();
	if (PS && PS->IsLoadCharacterBaseInfo())
	{
		const UDLAssetManager& AssetManager = UDLAssetManager::Get();
		const UDLCharacterAsset* Asset = Cast<UDLCharacterAsset>(AssetManager.GetPrimaryAssetObject(PS->GetCharacterInfoBase().CharacterAssetId));
		if (ensureAlwaysMsgf(Asset && Asset->CharacterClass.Get(), TEXT("应该已经加载了")))
		{
			return Asset->CharacterClass.Get();
		}
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AGASDemoGameMode::ServerPreLoadAsset(const TArray<FDLGameplayPlayerInfo>& Array)
{
	const UDLAssetManager& AssetManager = UDLAssetManager::Get();

	TSet<FSoftObjectPath> PathsToLoad;

	for (const auto& PlayerInfo : Array)
	{
		const auto DT = ADLGameState::GetCharacterDataTable<FDTPlayerCharacterInfoRow>(DRCharacter::CreatePlayerDataRegistryId(PlayerInfo.CharacterId));
		if (!ensureAlwaysMsgf(DT, TEXT("数据表一定能加载到")))
		{
			continue;
		}

		auto AssetId = DT->BaseInfo.CharacterAssetId;

		TArray<FName> LoadBundles;
		LoadBundles.Add(TEXT("Default"));
		LoadBundles.Add(TEXT("Server"));
		AssetManager.GetPrimaryAssetLoadSet(PathsToLoad, AssetId, LoadBundles, true);
	}

	// 同步加载 ...

	for (const auto& ObjPath : PathsToLoad)
	{
		if (ObjPath.IsValid())
		{
			auto Obj = ObjPath.TryLoad();
			check(Obj);
			HoldLoadAssets.Add(Obj);
		}
	}

	// ReSharper disable once CppExpressionWithoutSideEffects
	OnLoadAssetComplate.ExecuteIfBound();
}

void AGASDemoGameMode::SetupGameplayPlayerInfo(TArray<FDLGameplayPlayerInfo> MyInfo)
{
	const auto GS = GetGameState();
	GS->SetGameplayPlayerInfos(MyInfo);
	this->ServerPreLoadAsset(MyInfo);
}

void AGASDemoGameMode::OnRecvLogicServerGameplayInfo(const FDLGameplayInfo& GameplayInfo)
{
	TArray<FDLGameplayPlayerInfo> MyInfo;

	for (const auto& Value : GameplayInfo.PlayerInfo)
	{
		auto& Ref = MyInfo.AddDefaulted_GetRef();
		Ref.CharacterId = *Value.CharacterId;
		Ref.PlayerID = Value.PlayerId;
	}

	UE_LOG(LogTemp, Log, TEXT("AGASDemoGameMode::OnRecvLogicServerGameplayInfo  %s"), *GameplayInfo.ToJson());

	this->SetupGameplayPlayerInfo(MyInfo);
}


enum ELogicServerMode
{
	AbsLogicServer,
	Setting
};

static ELogicServerMode GetLogicServerMode()
{
	const auto CmdArg = FCommandLine::Get();
	if (FParse::Param(CmdArg, TEXT("UsedAbstractLogicServer")))
	{
		return AbsLogicServer;
	}

#if WITH_EDITOR
	return Setting;
#else
	return AbsLogicServer;
#endif
}


void AGASDemoGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GetLogicServerMode() == AbsLogicServer)
	{
		const auto AbsLogicServer = IDLAbstractLogicServerInterface::Get(this);
		if (AbsLogicServer)
		{
			AbsLogicServer->OnRecvGameplayInfo().BindUObject(this, &ThisClass::OnRecvLogicServerGameplayInfo);

			OnLoadAssetComplate.BindWeakLambda(this, [this, AbsLogicServer]()
				{
					AbsLogicServer->GameplayServerLoadComplate(TEXT("Succeed"));
				});

			AbsLogicServer->GameplayServerSetupReady();
		}
	}
	else
	{
		OnLoadAssetComplate.BindWeakLambda(this, [this]()
		{
			UE_LOG(LogTemp, Log, TEXT("========== Load Complaet ===== 玩家可以加入了"));
		});


		TArray<FDLGameplayPlayerInfo> Infos;

		const auto Setting = UDLGameplayLaunchSetting::GetGameplayLaunchSetting();

		for (int32 Index = 0; Index < Setting->ClientNumber; Index ++)
		{
			FDLGameplayPlayerInfo Info;
			Info.PlayerID = 256 + Index;
			Info.CharacterId = UDLGameplayLaunchSetting::GetGameplayLaunchSetting()->GetCurrentSelectCharacterID();
			Infos.Add(Info);
		}

		this->SetupGameplayPlayerInfo(Infos);
	}
}
