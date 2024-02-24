#pragma once
#include "CoreMinimal.h"
#include "DLGameplayPredictTask.h"
#include "DLPlayerCharacter.h"
#include "IDLPlayerInputCmdSystem.h"
#include "SMStateMachineInstance.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayFramwork/DLPlayerControllerBase.h"
#include "Interface/IDLAbilityContextDataGetter.h"
#include "Interface/IDLPlayerController.h"
#include "VisualLogger/VisualLoggerDebugSnapshotInterface.h"
#include "DLPlayerController.generated.h"


class IGameplayUISystem;
class ULockPointComponent;
class UDLPlayerInputCmdRoll;
class ADLPlayerCharacter;
class AHeroPlayerState;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiveInputCmd, UDLPlayerInputCmdBase*, InputCmd);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchSneakState, bool, IsOpen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovement);


UCLASS()
class ADLPlayerController
	: public ADLPlayerControllerBase
	, public IDLAbilityContextDataGetter
	, public IVisualLoggerDebugSnapshotInterface
	, public IDLPlayerControllerInterface
{

	GENERATED_BODY()
public:

	virtual void Tick(float DeltaSeconds) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Pawn() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginPlay() override;
public:

	virtual void GrabDebugSnapshot(FVisualLogEntry* Snapshot) const override;

protected:

	IGameplayUISystem* GetGameplayUISystem() const;

public:

	UFUNCTION(BlueprintNativeEvent)
		void OnActiveUnderAttack(UDLGameplayPredictTask* Task);

	UFUNCTION(BlueprintNativeEvent)
		void OnEndUnderAttack(UDLGameplayPredictTask* Task, bool Cancel);

	UFUNCTION(BlueprintImplementableEvent)
		USMInstance* GetPlayerLogicStateMachineIns() const;

protected:

	UFUNCTION(BlueprintNativeEvent)
		void OnAbilityActive(UGameplayAbility* GameplayAbility);

	void OnAbilityEnd(const FAbilityEndedData& AbilityEndedData);

	UFUNCTION(BlueprintNativeEvent, DisplayName = OnAbilityEnd)
		void K2_OnAbilityEnd(bool bWasCancelled, UGameplayAbility* GameplayAbility);



public:

	UFUNCTION(BlueprintCallable)
		void SetCurrentMoveSpeedCoefficient(float CurrentMoveSpeedCoefficient);


	UFUNCTION(BlueprintImplementableEvent)
		void OnPlayerCharacterSet(ADLPlayerCharacter* InCharacter);

	UFUNCTION(BlueprintImplementableEvent)
		void OnPlayerCharacterRemove();

	UFUNCTION(BlueprintPure)
		ADLPlayerCharacter* GetPlayerCharacter()const;

public:

	void SyncRollContextData(FAbilityRollContext InAbilityRollContext);

	UFUNCTION(Reliable, Server)
		void ServerSetRollContextData(FAbilityRollContext InAbilityRollContext);

	virtual void GetAudioListenerPosition(FVector& OutLocation, FVector& OutFrontDir, FVector& OutRightDir) const override;

	UFUNCTION(BlueprintPure)
		virtual FVector GetInputVector() const override;


	UPROPERTY(BlueprintAssignable)
		FOnReceiveInputCmd OnReceiveInputCmd;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
		FOnSwitchSneakState OnSwitchSneak;

	UPROPERTY(BlueprintAssignable)
		FOnMovement OnMovement;

#pragma region IDLAbilityContextDataGetter

	virtual FAbilityRollContext GetRollContext() const override;

#pragma endregion


public:

	UFUNCTION(Server, Reliable,BlueprintCallable)
		void ServerCreateAI(int32 HeroId, const FString& StartPointTag);

	UFUNCTION(Server, Reliable)
		void ServerRemoveAllAI();

	/*
	*	本地作弊命令
	*/

	UFUNCTION(Exec)
		void ExecCreateAIById(int32 HeroId, const FString& StartPointTag);

	UFUNCTION(Exec)
		void ExecRemoveAllAI();

private:

	void CheckAndProcessPawnInit();

	void ClientProcessPawnInit(ADLPlayerStateBase* PS);

	void ClientProcessPawnRemove(ADLPlayerStateBase* PS);

public:

	UPROPERTY(BlueprintReadOnly, Transient)
		FGameplayTag PlayerInputStateTag;

	UPROPERTY(BlueprintReadWrite, Transient)
		UDLPlayerInputCmdBase* CacheInputCmd;

	UPROPERTY(EditDefaultsOnly)
		FGameplayTag ActivateRollEventTag;

	UPROPERTY(BlueprintReadWrite, Transient)
		FGameplayTagContainer AllowExecInputCmdTags;

	UPROPERTY(BlueprintReadWrite)
		FRotator RollCmdRotatorCache;

	UPROPERTY()
		FAbilityRollContext AbilityRollContext;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UDLPlayerLockUnitLogic> PlayerLockUnitLogicClass;

	UPROPERTY(BlueprintReadOnly)
		UDLPlayerLockUnitLogic* PlayerLockUnitLogic;

	UPROPERTY(BlueprintReadWrite)
		FVector CachePlayerInputVector;
private:

	UPROPERTY()
		APawn* CurrentPawn = nullptr;

private:

	bool bIsPawnInit = false;
};

