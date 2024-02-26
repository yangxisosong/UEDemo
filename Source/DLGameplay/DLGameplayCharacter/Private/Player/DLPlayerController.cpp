// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppMemberFunctionMayBeConst

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "DLPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "DataRegistrySubsystem.h"
#include "Blueprint/UserWidget.h"
#include "DLAbilityTagsDef.h"
#include "DLPlayerSetupInfo.h"
#include "IDLGameSavingSystem.h"
#include "IDLPlayerSavingAccessor.h"
#include "ASC/DLPlayerAbilitySysComponent.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/RuntimeErrors.h"
#include "DLGamePlayCharacterDef.h"
#include "DLPlayerCharacter.h"
#include "DLPlayerInputCmd.h"
#include "SMInstance.h"
#include "Engine/ViewportStatsSubsystem.h"
#include "GameFramework/HUD.h"
#include "GameplayFramwork/DLGameModeBase.h"
#include "Interface/IGameplayUISystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logic/DLPlayerLockUnitLogic.h"
#include "GameFramework/PlayerState.h"
#include "GameplayFramwork/DLPlayerStateBase.h"
#include "GameplayFramwork/Component/DLPlayerStateExtensionComp.h"
#include "Interface/ICharacterStateAccessor.h"
#include "Interface/ICharacterStateListener.h"

void ADLPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (PlayerLockUnitLogic)
	{
		PlayerLockUnitLogic->Tick(DeltaSeconds);
	}
}

void ADLPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	this->bShowMouseCursor = true;

	FInputModeGameAndUI NewInputMode;
	NewInputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
	NewInputMode.SetHideCursorDuringCapture(false);
	this->SetInputMode(NewInputMode);

	const auto Ext = GetPlayerState<ADLPlayerStateBase>()->FindComponentByClass<UDLPlayerStateExtensionComp>();
	if (Ext)
	{
		Ext->OnPlayerPawnInitReady_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate::CreateUObject(this, &ThisClass::ClientProcessPawnInit));
		Ext->OnPlayerPawnRemove_CallAndRegister(FOnGeneralPSExtentEvent::FDelegate::CreateUObject(this, &ThisClass::ClientProcessPawnRemove));
	}

	return;
}

void ADLPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	// 确保这个函数中的逻辑 对于同样的 Pawn 只触发一次
	// 目前引擎 对于同样的 Pawn 可能触发多次
	if (CurrentPawn == GetPawn())
	{
		return;
	}
	CurrentPawn = GetPawn();

	this->CheckAndProcessPawnInit();
}

void ADLPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void ADLPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

void ADLPlayerController::GrabDebugSnapshot(FVisualLogEntry* Snapshot) const
{
#if ENABLE_VISUAL_LOG

	if (this->IsLocalPlayerController())
	{
		const UDLInputCmdSubsystem* InputSys = UDLInputCmdSubsystem::Get(this);
		if (InputSys)
		{
			auto& InputCmdStatus = Snapshot->Status.AddZeroed_GetRef();
			InputCmdStatus.Category = TEXT("InputCmd");

			TArray<FString> AllowCmdStatusStrArray;
			InputSys->AllowExecCmdToString().ParseIntoArray(AllowCmdStatusStrArray, TEXT(","));
			for (const auto& Cmd : AllowCmdStatusStrArray)
			{
				InputCmdStatus.Add(TEXT("AllowCmdTag"), Cmd);
			}

			const auto CacheCmd = InputSys->GetCurrentCacheCmd();
			InputCmdStatus.Add(TEXT("CacheCmd"), CacheCmd ? CacheCmd->ToString() : TEXT("None"));
		}
	}

	const auto SMInst = GetPlayerLogicStateMachineIns();
	if (SMInst && SMInst->IsActive())
	{
		auto& LogicStatus = Snapshot->Status.AddZeroed_GetRef();
		LogicStatus.Category = TEXT("PlayerLogicStateMachine");

		TArray<FSMState_Base*> ActiveStates = SMInst->GetAllActiveStates();

		for (const auto& State : ActiveStates)
		{
			LogicStatus.Add(TEXT("ActiveState"), State->GetNodeName());
		}
	}


#endif

}

IGameplayUISystem* ADLPlayerController::GetGameplayUISystem() const
{
	return Cast<IGameplayUISystem>(GetHUD());
}

void ADLPlayerController::OnActiveUnderAttack_Implementation(UDLGameplayPredictTask* Task)
{

}

void ADLPlayerController::OnAbilityActive_Implementation(UGameplayAbility* GameplayAbility)
{
}

void ADLPlayerController::OnAbilityEnd(const FAbilityEndedData& AbilityEndedData)
{
	this->K2_OnAbilityEnd(AbilityEndedData.bWasCancelled, AbilityEndedData.AbilityThatEnded);
}

void ADLPlayerController::K2_OnAbilityEnd_Implementation(bool bWasCancelled, UGameplayAbility* GameplayAbility)
{

}


void ADLPlayerController::OnEndUnderAttack_Implementation(UDLGameplayPredictTask* Task, bool Cancel)
{
}

void ADLPlayerController::SetCurrentMoveSpeedCoefficient(float CurrentMoveSpeedCoefficient)
{
	const auto MyCharacter = Cast<ADLPlayerCharacter>(GetCharacterBase());
	if (MyCharacter)
	{
		MyCharacter->SetCurrentMoveSpeedCoefficient(CurrentMoveSpeedCoefficient);
	}
	else
	{
		UE_LOG(LogDLGamePlayChracter, Warning, TEXT("获取 Controller 所属的 Character 失败，调用时机错误"));
	}
}

ADLPlayerCharacter* ADLPlayerController::GetPlayerCharacter() const
{
	return Cast<ADLPlayerCharacter>(GetPawn());
}

void ADLPlayerController::SyncRollContextData(FAbilityRollContext InAbilityRollContext)
{
	AbilityRollContext = InAbilityRollContext;
	this->ServerSetRollContextData(InAbilityRollContext);
}

void ADLPlayerController::ServerSetRollContextData_Implementation(FAbilityRollContext InAbilityRollContext)
{
	AbilityRollContext = InAbilityRollContext;
}

void ADLPlayerController::GetAudioListenerPosition(FVector& OutLocation, FVector& OutFrontDir,
	FVector& OutRightDir) const
{
	if (GetPlayerCharacter() == nullptr)
	{
		Super::GetAudioListenerPosition(OutLocation, OutFrontDir, OutRightDir);
	}
	else
	{
		const auto PlayerCharacter = GetPlayerCharacter();
		const auto ViewLocation = PlayerCharacter->GetActorLocation();
		const auto ViewRotation = PlayerCharacter->GetActorRotation();

		const FRotationTranslationMatrix ViewRotationMatrix(ViewRotation, ViewLocation);

		OutLocation = ViewLocation;
		OutFrontDir = ViewRotationMatrix.GetUnitAxis(EAxis::X);
		OutRightDir = ViewRotationMatrix.GetUnitAxis(EAxis::Y);
	}
}

FVector ADLPlayerController::GetInputVector() const
{
	return CachePlayerInputVector;
}

FAbilityRollContext ADLPlayerController::GetRollContext() const
{
	return AbilityRollContext;
}


void ADLPlayerController::ServerCreateAI_Implementation(int32 HeroId, const FString& StartPointTag)
{
	if (!ensureAlwaysMsgf(GetLocalRole() == ROLE_Authority, TEXT("一定是服务端在执行")))
	{
		return;
	}

	const auto GameMode = Cast<ADLGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->CreateAI(this, HeroId, StartPointTag);
	}
}


void ADLPlayerController::ServerRemoveAllAI_Implementation()
{
	if (!ensureAlwaysMsgf(GetLocalRole() == ROLE_Authority, TEXT("一定是服务端在执行")))
	{
		return;
	}

	const auto GameMode = Cast<ADLGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->RemoveAllAI();
	}
}



void ADLPlayerController::ExecCreateAIById(int32 HeroId, const FString& StartPointTag)
{
	this->ServerCreateAI(HeroId, StartPointTag);
}

void ADLPlayerController::ExecRemoveAllAI()
{
	this->ServerRemoveAllAI();
}

void ADLPlayerController::CheckAndProcessPawnInit()
{
	if (CurrentPawn && bIsPawnInit)
	{
		if (const auto CmdSystem = UDLInputCmdSubsystem::Get(this))
		{
			CmdSystem->OnClientSetup();
		}

		// TODO 考虑变身 附体等等情况  不应该以 Pawn 作为初始化UI的入口
		const auto UISystem = GetGameplayUISystem();
		if (GetPawn() && UISystem)
		{
			UE_LOG(LogTemp, Log, TEXT("Enter Battle"));

			this->bShowMouseCursor = false;

			const FInputModeGameOnly NewInputMode;
			this->SetInputMode(NewInputMode);

			IGameplayUISystem::FInitArg Arg;
			Arg.CharacterStateListener = PlayerState;
			Arg.CharacterStateAccessor = CurrentPawn;
			UISystem->OnGameplayInit(Arg);
		}


		if (GetPlayerCharacter())
		{
			this->OnPlayerCharacterSet(GetPlayerCharacter());

			PlayerLockUnitLogic = NewObject<UDLPlayerLockUnitLogic>(this, PlayerLockUnitLogicClass, TEXT("PlayerLockUnitLogic"));
			PlayerLockUnitLogic->Init(this);
		}

		if (const auto ASC = Cast<UDLPlayerAbilitySysComponent>(GetPlayerStateBase()->GetASC()))
		{
			ASC->AbilityActivatedCallbacks.AddUObject(this, &ADLPlayerController::OnAbilityActive);
			ASC->OnAbilityEnded.AddUObject(this, &ADLPlayerController::OnAbilityEnd);
			ASC->EnabledAbilityInput();
		}
	}
}

void ADLPlayerController::ClientProcessPawnInit(ADLPlayerStateBase* PS)
{
	bIsPawnInit = true;
	this->CheckAndProcessPawnInit();
}

void ADLPlayerController::ClientProcessPawnRemove(ADLPlayerStateBase* PS)
{
	bIsPawnInit = false;

	this->OnPlayerCharacterRemove();

	// 移除预输入系统
	if (const auto CmdSystem = UDLInputCmdSubsystem::Get(this))
	{
		CmdSystem->OnClientClear();
	}

	// 移除 锁定系统
	if (PlayerLockUnitLogic)
	{
		PlayerLockUnitLogic->MarkAsGarbage();
		PlayerLockUnitLogic = nullptr;
	}

	// 移除 ASC 上的 Bind
	if (const auto ASC = Cast<UDLPlayerAbilitySysComponent>(PS->GetASC()))
	{
		ASC->AbilityActivatedCallbacks.RemoveAll(this);
		ASC->OnAbilityEnded.RemoveAll(this);
		ASC->DisabledAbilityInput();
	}
}

