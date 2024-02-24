// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "GameFramework/Actor.h"
#include "GameplayFramwork/DLPlayerStateBase.h"
#include "DLPlayerState.generated.h"

class UDLPlayerAbilitySysComponent;

UCLASS()
class ADLPlayerState
	: public ADLPlayerStateBase
{
private:
	GENERATED_BODY()

public:

	ADLPlayerState();

	void SetNewScene(FName NewSceneId);

	FName GetCurrentScene() const { return CurrentSceneId; }

protected:

	virtual void BeginPlay() override;

#pragma region ADLPlayerStateBase
	virtual bool ServerInitPlayerBaseInfo(const FPlayerBaseInfo& Info) override;

	virtual void OnInitPlayerData(const FPlayerBaseInfo& Info) override;

	virtual void OnRep_CharacterBaseInfo() override;
#pragma endregion


	UFUNCTION(BlueprintNativeEvent)
		bool LoadDataTable(FDTPlayerCharacterInfoRow& OutInfo);

private:

	UPROPERTY()
		UDLPlayerAbilitySysComponent* PlayerAbilitySys;

	UPROPERTY()
		FName CurrentSceneId;
};
