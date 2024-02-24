#pragma once

#include "CoreMinimal.h"
#include "DLCharacterBase.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayFramwork/DLGameState.h"
#include "DLGameModeBase.generated.h"

class UDLCharacterAsset;

USTRUCT()
struct FDLCreateNPCArg
{
	GENERATED_BODY()
public:

	UPROPERTY()
		FTransform SpawnTransform;

	UPROPERTY()
		FName StartPointId;

	UPROPERTY()
		FName NPCCharacterId;

	UPROPERTY()
		UDLCharacterAsset* Asset = nullptr;
};

UCLASS()
class DLGAMEPLAYCORE_API ADLGameModeBase
	: public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void CreateAI(class ADLPlayerControllerBase* PlayerController, int32 HeroId, FString StartPointTag) {}

	virtual void RemoveAllAI() {}

	virtual ADLCharacterBase* CreateNPC(const FDLCreateNPCArg& Arg) { return nullptr; }

	virtual bool SetupPlayerCharacter(APlayerController* NewPlayer) { return false; };

	ADLGameState* GetGameState() const { return  Cast<ADLGameState>(GameState); }
};

