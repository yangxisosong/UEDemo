#pragma once

#include "CoreMinimal.h"
#include "DLPlayerControllerCommon.h"
#include "GameFramework/PlayerController.h"
#include "Interface/IDLCharacterController.h"

#include "DLPlayerControllerBase.generated.h"

class UDLLocalPlayerAbilitySysComp;
class ADLPlayerStateBase;
struct FDLPlayerSetupInfo;
class ADLCharacterBase;


UCLASS()
class DLGAMEPLAYCORE_API ADLPlayerControllerBase
	: public ADLPlayerControllerCommon
	, public IDLCharacterController
{
	GENERATED_BODY()
public:

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	virtual void TryTurnToRotation(FRotator Rotator, bool ImmediatelyTurn = false, float OverrideYawSpeed = 0.f) override;

	virtual void ServerRestartPlayer_Implementation() override;

	virtual void InitPlayerState() override;

	virtual void CleanupPlayerState() override;
	/**
	 * @brief 子类可以重写这个方法 已控制角色的初始化数据获取方式
	 * @param OutSetup 
	 * @return 
	 */
	virtual bool ReadPlayerSetupInfo(FDLPlayerSetupInfo& OutSetup);

	virtual void OnRep_PlayerState() override;

public:

	ADLPlayerStateBase* GetPlayerStateBase() const;

	// 任何身份的对象都会被触发
	void OnCharacterDied();

	void ServerSetupCharacter(const FDLPlayerSetupInfo* InSetupInfo, ADLCharacterBase* InCharacter, ADLPlayerStateBase* InPlayerState);

public:

	UFUNCTION(BlueprintPure)
	ADLCharacterBase* GetCharacterBase() const;


protected:

	
};
