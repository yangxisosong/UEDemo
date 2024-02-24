// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DLPlayerSetupInfo.h"
#include "IDLAbstractLogicServerInterface.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayFramwork/DLGameModeBase.h"
#include "GASDemoGameMode.generated.h"

class ADLCharacterBase;
class ADLAIControllerBase;
class UEntityInitSpec;

UCLASS()
class DLAPPLICATION_API AGASDemoGameMode
	: public ADLGameModeBase
{
	GENERATED_BODY()

public:

	AGASDemoGameMode();

	virtual void CreateAI(class ADLPlayerControllerBase* PlayerController, int32 HeroId, FString StartPointTag) override;

	virtual void RemoveAllAI() override;

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	virtual ADLCharacterBase* CreateNPC(const FDLCreateNPCArg& Arg) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	virtual bool SetupPlayerCharacter(APlayerController* NewPlayer) override;

protected:
	void ServerPreLoadAsset(const TArray<FDLGameplayPlayerInfo>& Array);
	void OnRecvLogicServerGameplayInfo(const FDLGameplayInfo& GameplayInfo);	
	virtual void BeginPlay() override;
	void SetupGameplayPlayerInfo(TArray<FDLGameplayPlayerInfo> MyInfo);

public:

	UPROPERTY(EditAnywhere)
		TSubclassOf<APawn> NPCClassBase;

	UPROPERTY(EditAnywhere)
		class UDataTable* CharacterDataTable;

	UPROPERTY(Transient)
		FDLPlayerSetupInfo DefaultSetupInfo;

	UPROPERTY(Transient)
		TArray<UObject*> HoldLoadAssets;

private:

	FSimpleDelegate OnLoadAssetComplate;
};


