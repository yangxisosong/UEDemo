
#include "CoreMinimal.h"
#include "DLPlayerInputCmd.h"
#include "IDLPlayerInputCmdSystem.h"


#if WITH_EDITOR

#include "ISettingsModule.h"
#include "ISettingsSection.h"

#endif // WITH_EDITOR


#include "Engine/ViewportStatsSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Modules/ModuleInterface.h"
#include "VisualLogger/VisualLogger.h"


static TAutoConsoleVariable<int> CVarEnabledInputCmdDebugDisplay(
	TEXT("DL.Debug.EnabledInputCmd"),
	0,
	TEXT("是否启用输入预处理调试显示")
	TEXT(" 1 : 启用")
	TEXT(" 0 : 不启用")
);


struct FDLGameplayInputCmdSystem : public IModuleInterface
{
	virtual void StartupModule() override
	{
#if WITH_EDITOR
		// register settings
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		if (SettingsModule != nullptr)
		{
			ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Game", "PlayerInputCmdFactory",
				FText::FromString(TEXT("PlayerInputCmdFactory")), FText::FromString(TEXT("PlayerInputCmdFactory")),
				GetMutableDefault<UDLPlayerInputCmdFactory>()
			);
		}
#endif // WITH_EDITOR
	}

	virtual void ShutdownModule() override
	{
#if WITH_EDITOR

		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (SettingsModule != nullptr)
		{
			SettingsModule->UnregisterSettings("Project", "Game", "PlayerInputCmdFactory");
		}
#endif
	}
};

IMPLEMENT_MODULE(FDLGameplayInputCmdSystem, DLGameplayInputCmdSystem);

DEFINE_LOG_CATEGORY(LogDLGamePlayInputCmd);


void UDLInputCmdSubsystem::PushInputCmd(UDLPlayerInputCmdBase* InputCmd)
{
	if (!bIsInit)
	{
		return;
	}


	if (!this->ExecCmd(InputCmd, TEXT("PushInputCmd Try Exec")))
	{
		this->ClearInputCmd();

		CacheInputCmd = InputCmd;
	}
}

void UDLInputCmdSubsystem::AddAllowCmdTags(const FGameplayTagContainer& InTags, const FString& DebugStr)
{
	AllowExecInputCmdTags.UpdateTagCount(InTags, 1);

	UE_VLOG_UELOG(GetLocalPlayer<>()->GetPlayerController(GetWorld()),
		LogDLGamePlayInputCmd,
		Verbose,
		TEXT("AddAllowCmdTags InTags %s  DebugStr %s"), *InTags.ToStringSimple(), *DebugStr);
}

void UDLInputCmdSubsystem::RemoveCmdTags(const FGameplayTagContainer& InTags, const FString& DebugStr)
{
	AllowExecInputCmdTags.UpdateTagCount(InTags, -1);

	UE_VLOG_UELOG(GetLocalPlayer<>()->GetPlayerController(GetWorld()),
		LogDLGamePlayInputCmd,
		Verbose,
		TEXT("RemoveCmdTags InTags %s  DebugStr %s"), *InTags.ToStringSimple(), *DebugStr);
}

UDLPlayerInputCmdBase* UDLInputCmdSubsystem::GetCurrentCacheCmd() const
{
	return CacheInputCmd;
}

void UDLInputCmdSubsystem::ClearInputCmd()
{
	if (CacheInputCmd)
	{
		CacheInputCmd->SetActive(false);
	}
	CacheInputCmd = nullptr;
}

UDLInputCmdSubsystem* UDLInputCmdSubsystem::Get(const APlayerController* PC)
{
	UDLInputCmdSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UDLInputCmdSubsystem>(PC->GetLocalPlayer());
	return Subsystem;
}

UDLInputCmdSubsystem* UDLInputCmdSubsystem::Get(const ULocalPlayer* LocalPlayer)
{
	UDLInputCmdSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UDLInputCmdSubsystem>(LocalPlayer);
	return Subsystem;
}

UDLInputCmdSubsystem* UDLInputCmdSubsystem::Get(const UObject* WorldObjectContext)
{
	const auto PC = UGameplayStatics::GetPlayerController(WorldObjectContext, 0);
	UDLInputCmdSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UDLInputCmdSubsystem>(PC->GetLocalPlayer());
	return Subsystem;
}

UDLPlayerInputCmdBase* UDLInputCmdSubsystem::AllocateCmd(TSubclassOf<UDLPlayerInputCmdBase> Class,
														 const FDLPlayerInputCmdConstructBase* Data)
{
	if (!Class)
	{
		return nullptr;
	}

	UDLPlayerInputCmdBase* Ret = nullptr;
	for (const auto Cmd : CmdPool)
	{
		if (Cmd->GetClass() == Class && Cmd->IsIdle())
		{
			Cmd->SetActive(true);
			Ret = Cmd;
			break;
		}
	}

	if (!Ret)
	{
		Ret = NewObject<UDLPlayerInputCmdBase>(this, Class);
		CmdPool.Add(Ret);
	}

	if (Data)
	{
		const UScriptStruct* Struct = Data->GetScriptStruct();
		for (TFieldIterator<FProperty> It(Struct); It; ++It)
		{
			const FName FieldName = It->GetFName();
			const auto InsField = Class->FindPropertyByName(FieldName);
			if (InsField)
			{
				void* DesPtr = InsField->ContainerPtrToValuePtr<void>(Ret);
				const void* SrcPtr = It->ContainerPtrToValuePtr<void>(Data);
				InsField->CopySingleValue(DesPtr, SrcPtr);
			}
		}
	}

	Ret->InitFinish();

	return Ret;
}

void UDLInputCmdSubsystem::Deinitialize()
{
	if (UViewportStatsSubsystem* ViewportSubsystem = GetWorld()->GetSubsystem<UViewportStatsSubsystem>())
	{
		ViewportSubsystem->RemoveDisplayDelegate(ViewportHandle);
		ViewportHandle = INDEX_NONE;
	}

	Super::Deinitialize();
}

void UDLInputCmdSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
bool UDLInputCmdSubsystem::ExecCmd(UDLPlayerInputCmdBase* InputCmd, const FString& ContextDebugStr)
{
	if (!bIsInit)
	{
		return false;
	}

	if (!InputCmd)
	{
		UE_LOG(LogDLGamePlayInputCmd, Warning, TEXT("UDLInputCmdSubsystem::ExecCmd InputCmd Is None"));
		return false;
	}

	auto&& Tags = InputCmd->CmdTags;
	if (!Tags.HasAny(AllowExecInputCmdTags.GetExplicitGameplayTags()))
	{
		UE_LOG(LogDLGamePlayInputCmd, Verbose, TEXT("不允许执行这条命令 %s DebugSts(%s)"), *InputCmd->ToString(), *ContextDebugStr);
		return false;
	}

	FDLPlayerInputCmdExecContext Context;
	Context.ContextDebugStr = ContextDebugStr;

	Context.LocalController = GetLocalPlayerChecked<>()->GetPlayerController(nullptr);
	if (Context.LocalController)
	{
		Context.LocalCharacter = Context.LocalController->GetPawn();
	}

	UE_VLOG_UELOG(GetLocalPlayer<>()->GetPlayerController(GetWorld()), LogDLGamePlayInputCmd,
		Verbose,
		TEXT("===> Exec Cmd -> %s DebugStr : %s"),
		*InputCmd->ToString(),
		*ContextDebugStr
	);

	const bool Ok = InputCmd->Exec(Context);
	InputCmd->SetActive(false);

	UE_VLOG_UELOG(GetLocalPlayer<>()->GetPlayerController(GetWorld()),
				LogDLGamePlayInputCmd,
				Verbose,
				TEXT("<=== Exec Cmd -> %s Ret -> %s, DebugStr : %s"),
				*InputCmd->ToString(),
				Ok ? TEXT("True") : TEXT("False"),
				*ContextDebugStr
	);

	return Ok;
}

void UDLInputCmdSubsystem::OnClientSetup()
{
#if WITH_EDITOR
	if (UViewportStatsSubsystem* ViewportSubsystem = GetWorld()->GetSubsystem<UViewportStatsSubsystem>())
	{
		if (ViewportHandle == INDEX_NONE)
		{
			ViewportHandle = ViewportSubsystem->AddDisplayDelegate(
				[this](FText& OutText, FLinearColor& OutColor)
				{
					if (CVarEnabledInputCmdDebugDisplay.GetValueOnGameThread() == 1)
					{
						const FString Text = FString::Printf(
							TEXT("CacheInputCmd : %s \nAllowExecInputCmdTags : %s"),
							CacheInputCmd ? *CacheInputCmd->ToString() : TEXT("None"),
							*AllowExecCmdToString()
						);

						OutText = FText::FromString(Text);
						OutColor = FLinearColor::Red;
					}
					return true;
				});
		}
	}
#endif

	bIsInit = true;
	AllowExecInputCmdTags.Reset();
	this->ClearInputCmd();
}

void UDLInputCmdSubsystem::OnClientClear()
{
	bIsInit = false;
}

void UDLInputCmdSubsystem::TryCheckMovementCmd()
{
	if (!bIsInit)
	{
		return;
	}

	const auto PC = GetLocalPlayer<>()->GetPlayerController(GetWorld());
	static FName GetInputVectorName(TEXT("GetInputVector"));
	FVector RetVector;
	const auto Func = PC->FindFunction(GetInputVectorName);
	if (ensureAlwaysMsgf(Func, TEXT("应该实现了 GetInputVectorName ")))
	{
		PC->ProcessEvent(Func, &RetVector);
	}

	if (!RetVector.IsNearlyZero())
	{
		FDLPlayerInputCmdConstructMovement Arg;
		Arg.InputVector = RetVector;
		const auto Cmd = UDLPlayerInputCmdFactory::CreateMovementCmd(this, Arg);
		UE_LOG(LogDLGamePlayInputCmd, Verbose, TEXT("UDLInputCmdSubsystem::TryCheckMovementCmd  Create Movement Cmd"));
		this->PushInputCmd(Cmd);
	}
}

FString UDLInputCmdSubsystem::AllowExecCmdToString()const
{
	FString RetStrings;
	if (AllowExecInputCmdTags.GetExplicitGameplayTags().Num() > 0)
	{
		for (FGameplayTag Tag : AllowExecInputCmdTags.GetExplicitGameplayTags())
		{
			RetStrings.Append(FString::Printf(TEXT("%s (%d)"), *Tag.ToString(), AllowExecInputCmdTags.GetTagCount(Tag)));
			RetStrings += TEXT(", ");
		}
	}
	return RetStrings;
}


UDLPlayerInputCmdBase* UDLPlayerInputCmdFactory::CreateMovementCmd(UObject* WorldContextObject, const FDLPlayerInputCmdConstructMovement& Arg)
{
	const FName FunctionName = TEXT("CreateMovementCmd");
	return UDLPlayerInputCmdFactory::AllocateCmd(WorldContextObject, FunctionName, &Arg);
}

UDLPlayerInputCmdBase* UDLPlayerInputCmdFactory::CreateActivateAbilityCmd(UObject* WorldContextObject,
																		  const FDLPlayerInputCmdConstructAbility& Arg)
{
	const FName FunctionName = TEXT("CreateActivateAbilityCmd");
	return UDLPlayerInputCmdFactory::AllocateCmd(WorldContextObject, FunctionName, &Arg);
}

UDLPlayerInputCmdBase* UDLPlayerInputCmdFactory::CreateRollCmd(UObject* WorldContextObject,
	const FDLPlayerInputCmdConstructRoll& Arg)
{
	const FName FunctionName = TEXT("CreateRollCmd");
	return UDLPlayerInputCmdFactory::AllocateCmd(WorldContextObject, FunctionName, &Arg);
}

UDLPlayerInputCmdBase* UDLPlayerInputCmdFactory::CreateJumpCmd(UObject* WorldContextObject,
	const FDLPlayerInputCmdConstructJump& Arg)
{
	const FName FunctionName = TEXT("CreateJumpCmd");
	return UDLPlayerInputCmdFactory::AllocateCmd(WorldContextObject, FunctionName, &Arg);
}

UDLPlayerInputCmdBase* UDLPlayerInputCmdFactory::CreateSwitchSneakStateCmd(UObject* WorldContextObject)
{
	const FName FunctionName = TEXT("CreateSwitchSneakStateCmd");
	return UDLPlayerInputCmdFactory::AllocateCmd(WorldContextObject, FunctionName, nullptr);
}

UDLPlayerInputCmdBase* UDLPlayerInputCmdFactory::AllocateCmd(const UObject* WorldContextObject, const FName FunctionName,
															 const FDLPlayerInputCmdConstructBase* Data)
{
	const auto DefaultFactory = GetMutableDefault<UDLPlayerInputCmdFactory>();

	if (!DefaultFactory->CmdClassConfig.Contains(FunctionName))
	{
		return nullptr;
	}

	const auto PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (PC && PC->GetLocalPlayer())
	{
		UDLInputCmdSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UDLInputCmdSubsystem>(PC->GetLocalPlayer());
		if (Subsystem)
		{
			return Subsystem->AllocateCmd(DefaultFactory->CmdClassConfig[FunctionName].LoadSynchronous(), Data);
		}
	}

	return nullptr;
}


