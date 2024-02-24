#pragma once

#include "CoreMinimal.h"
#include "DataRegistryId.h"
#include "DataRegistrySubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "DLGameState.generated.h"

struct FDTNPCCharacterInfoRow;
class ADLPlayerControllerBase;
class ADLCharacterBase;
class UDLCharacterAsset;
struct FDTPlayerCharacterInfoRow;
class ADLGameModeBase;
class ADLPlayerStateBase;


USTRUCT()
struct FDLGameplayPlayerInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
		int32 PlayerID = INDEX_NONE;

	UPROPERTY(EditAnywhere)
		FName CharacterId;
};


UCLASS(Config = Game)
class DLGAMEPLAYCORE_API ADLGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	ADLGameState();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	virtual void RemovePlayerState(APlayerState* PlayerState) override;


protected:

	UFUNCTION()
		virtual void OnRep_GameplayPlayerInfos();

public:

	ADLGameModeBase* ServerGetGm() const;

	const TArray<FDLGameplayPlayerInfo>& GetGameplayPlayerInfos()const;

	void SetGameplayPlayerInfos(const TArray<FDLGameplayPlayerInfo>& Infos);

	template<typename DTRowType>
	static const DTRowType* GetCharacterDataTable(const FDataRegistryId& Id)
	{
		const auto DataRegistry = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>();
		return DataRegistry->GetCachedItem<DTRowType>(Id);
	}

	const FDTPlayerCharacterInfoRow* FindPlayerCharacterInfoEnsure(FName CharacterId) const;
	const FDTNPCCharacterInfoRow* FindNPCCharacterInfoEnsure(FName CharacterId) const;

	ADLPlayerControllerBase* GetLocalPlayerController() const;

	bool ClientIsGameplayReady()const;

	ADLPlayerStateBase* FindNPCPlayerState(FName NpcStartPointId) const;

	void ClientLoadNPCCharacterComplate(FName CharacterId);

	void ServerSetPlayerLoadComplate(int32 PlayerId);

	UFUNCTION(NetMulticast, Reliable)
		void LoadNPCAsset(FName CharacterId);

private:

	UFUNCTION(NetMulticast, Reliable)
		void MulticastGameplayReady();



private:

	void StartLoadPlayerAsset();

	void LoadPlayerAsset(const FDLGameplayPlayerInfo& PlayerInfo);
	void CheckAllPlayerAssetLoadComplate(FName CharacterID);

	void ClientLoadPlayerAssetComplate(FName CharacterID);

	void OnPawnInitReady(ADLPlayerStateBase* PS);

	void CheckCharacterAssetLoadComplate(ADLPlayerStateBase* PS);

public:

	bool IsServer() const;

	bool IsClient() const;

private:

	bool bWaitOtherPlayLoading = false;

	int32 ServerWaitPlayerLoadingNum;

	// 正在加载中的 资产 ID
	TSet<FName> LoadingCharacterID;

	TSet<int32> LoadingPlayerIds;

	// 这里只记录 资产的 ID，这样相同的资产没必要在加载
	// NPC 是没有 PlayerId 的，所以应该使用更加存粹的  AssetId
	UPROPERTY()
		TMap<FName, UDLCharacterAsset*> LoadComplateCharacterIds;

	// 模拟服务器拿到这次游戏的玩家名单
	UPROPERTY(ReplicatedUsing = OnRep_GameplayPlayerInfos, EditDefaultsOnly)
		TArray<FDLGameplayPlayerInfo> GameplayPlayerInfos;


	bool IsGameplayReady = false;
};
