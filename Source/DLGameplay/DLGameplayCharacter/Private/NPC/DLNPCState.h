// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayFramwork/DLPlayerStateBase.h"
#include "Interface/IDLNPCState.h"
#include "DLNPCState.generated.h"

class UDLNPCAbilitySysComponent;

UCLASS()
class DLGAMEPLAYCHARACTER_API ADLNPCState
	: public ADLPlayerStateBase
	, public IDLNPCStateInterface
{
	GENERATED_BODY()

public:

	ADLNPCState();

	virtual bool ServerInitPlayerBaseInfo(const FPlayerBaseInfo& Info) override;

	virtual void OnInitPlayerData(const FPlayerBaseInfo& Info) override;

	virtual FName GetNPCStartPointId() const override { return  StartPointId; }

	virtual void SetNPCStartPointId(FName Id) { StartPointId = Id; }

private:

	UPROPERTY()
		UDLNPCAbilitySysComponent* NPCAbilitySys;

	UPROPERTY()
		FName StartPointId;
};
