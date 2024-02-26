// ReSharper disable CppMemberFunctionMayBeConst
#include "ASC/DLPlayerAbilitySysComponent.h"

#include "ConvertString.h"
#include "DLGameplayAbilityBase.h"
#include "GameAbilitySysDef.h"
#include "AbilityChain/AbilityChain.h"
#include "Engine/Canvas.h"
#include "DLAbilitySystemGlobal.h"
#include "DLPlayerInputCmd.h"
#include "EnhancedInputSubsystems.h"
#include "IDLPlayerInputCmdSystem.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "DLAbilityLog.h"
#include "Engine/ViewportStatsSubsystem.h"

class UEnhancedInputLocalPlayerSubsystem;

UDLPlayerAbilitySysComponent::UDLPlayerAbilitySysComponent()
{
}

EInputEvent UDLPlayerAbilitySysComponent::QueryAbilityInputState(const int32 InputId) const
{
	if (AbilityInputMap.Contains(InputId))
	{
		return AbilityInputMap[InputId];
	}
	return EInputEvent::IE_Released;
}

bool UDLPlayerAbilitySysComponent::InitAbilityChain(UAbilityChainAsset* Asset)
{
	AbilityChain = NewObject<UAbilityChain>();

	AbilityChain->SetEnabledAcceptInput(true);

	const bool BuildOk = AbilityChain->Init(Asset);

	this->OnAbilityEnded.AddUObject(this, &UDLPlayerAbilitySysComponent::OnClientAbilityEnd);
	this->AbilityActivatedCallbacks.AddUObject(this, &UDLPlayerAbilitySysComponent::OnClientActiveAbility);

	return BuildOk;
}

namespace PlayerASC
{
	FString ToString(const TMap<FGameplayAbilitySpecHandle, FGameplayTagContainer>& Data)
	{
		FString Ret;
		for (const auto& V : Data)
		{
			Ret += V.Value.ToStringSimple() += TEXT(" ");
		}
		return Ret;
	}
}

static TAutoConsoleVariable<int> CVarEnabledAbilityCancelByTagDisplay(
	TEXT("DL.Ability.EnabledAbilityCancelByTagDisplay"),
	0,
	TEXT("是否启用左上角的调试输入显示")
	TEXT(" 1 : 启用")
	TEXT(" 0 : 不启用")
);

static TAutoConsoleVariable<int> CVarEnabledAbilityDebugInput(
	TEXT("DL.Ability.EnableDebugInput"),
	0,
	TEXT("是否启用调试输入")
	TEXT(" 1 : 启用")
	TEXT(" 0 : 不启用")
);


void UDLPlayerAbilitySysComponent::BeginPlay()
{
	Super::BeginPlay();

	const bool IsDebug = CVarEnabledAbilityDebugInput.GetValueOnGameThread() == 1;
	bIsDebugInput = IsDebug;
	this->RefreshDebugInput();


#if WITH_EDITOR
	if (UViewportStatsSubsystem* ViewportSubsystem = GetWorld()->GetSubsystem<UViewportStatsSubsystem>())
	{
		if (ViewportHandle == INDEX_NONE)
		{
			ViewportHandle = ViewportSubsystem->AddDisplayDelegate(
				[this](FText& OutText, FLinearColor& OutColor)
				{
					if (CVarEnabledAbilityCancelByTagDisplay.GetValueOnGameThread() == 1)
					{
						FGameplayTagContainer Tags;
						GetOwnedGameplayTags(Tags);
						const FString Text = FString::Printf(
							TEXT("AbilityCanCancelByAbilityTag : %s \nPlayerState : %s"),
							*PlayerASC::ToString(AbilityBeCancelByAbilityTag),
							*Tags.ToStringSimple(true)
						);



						OutText = FText::FromString(Text);
						OutColor = FLinearColor::Blue;
					}
					return true;
				});
		}
	}
#endif
}

void UDLPlayerAbilitySysComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UViewportStatsSubsystem* ViewportSubsystem = GetWorld()->GetSubsystem<UViewportStatsSubsystem>())
	{
		ViewportSubsystem->RemoveDisplayDelegate(ViewportHandle);
		ViewportHandle = INDEX_NONE;
	}

	Super::EndPlay(EndPlayReason);
}


void UDLPlayerAbilitySysComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if !UE_BUILD_SHIPPING

	const bool IsDebug = CVarEnabledAbilityDebugInput.GetValueOnGameThread() == 1;

	if (IsDebug != bIsDebugInput)
	{
		bIsDebugInput = IsDebug;
		this->RefreshDebugInput();
	}

#endif

}




void UDLPlayerAbilitySysComponent::OnClientActiveAbility(UGameplayAbility* Ability) const
{
	const auto AbilityBase = Cast<UDLGameplayAbilityBase>(Ability);
	if (AbilityBase && AbilityActorInfo && AbilityActorInfo->IsLocallyControlledPlayer())
	{
		auto InputId = EPrimaryInputID::None;

		if (CVarEnabledAbilityDebugInput.GetValueOnGameThread() != 0)
		{
			if (static_cast<int32>(AbilityBase->GetDebugInputID()) > static_cast<int32>(EPrimaryInputID::DebugKeyBegin))
			{
				InputId = AbilityBase->GetDebugInputID();
			}

			ensureAlwaysMsgf(InputId != EPrimaryInputID::None, TEXT("Ability Input 一定有一个 InputKey "));
			AbilityBase->SetActiveInputKey(InputId);
		}
		else
		{
			InputId = AbilityChain ?
				static_cast<EPrimaryInputID>(AbilityChain->GetLastInputId().KeyIndex) : EPrimaryInputID::None;

			if (CurrentAbilityChainCheckout == Ability->GetClass())
			{
				ensureAlwaysMsgf(InputId != EPrimaryInputID::None, TEXT("Ability Input 一定有一个 InputKey "));
				AbilityBase->SetActiveInputKey(InputId);
			}
			else
			{
				// 这个技能不是用按键触发的  可以忽略
			}
		}


	}
}



void UDLPlayerAbilitySysComponent::AbilityLocalInputReleased(const int32 InputId)
{
	AbilityInputMap.FindOrAdd(InputId) = EInputEvent::IE_Released;

	this->OnAbilityInputChange().Broadcast(EInputEvent::IE_Released, InputId);

	if (InputId > static_cast<int32>(EPrimaryInputID::DebugKeyBegin))
	{
		Super::AbilityLocalInputReleased(InputId);
	}
}


void UDLPlayerAbilitySysComponent::DisplayDebugInfo(
	UCanvas* Canvas,
	const FDebugDisplayInfo& DebugDisplay,
	float& YL,
	float& YPos) const
{
	Super::DisplayDebugInfo(Canvas, DebugDisplay, YL, YPos);

	FString DebugStr = TEXT("None");
	if (AbilityChain)
	{
		DebugStr = AbilityChain->GetDebugString();
	}

	Canvas->SetDrawColor(FColor::Red);
	YL = Canvas->DrawText(GEngine->GetLargeFont(), DebugStr, 4.0f, YPos, 1.5f, 1.5f);
	YPos += YL;
}


bool UDLPlayerAbilitySysComponent::InjectInputActiveAbility(int32 InputKey, UObject* AbilityContext)
{
	bool Ret = false;

	if (AbilityChain)
	{
		FAbilityChainInputKey Key;
		Key.KeyIndex = static_cast<EPrimaryInputID>(InputKey);

		AbilityChain->InjectInput(Key);
		UAbilityChain::FCheckOutAbilityArg Arg;
		Arg.ASC = this;
		Arg.CurrentActiveAbility = Cast<UDLGameplayAbilityBase>(AbilityContext);

		const auto AbilityClass = AbilityChain->CheckoutAlternateAbility(Arg);
		if (AbilityClass)
		{
			AbilityChain->CommitCheckOutAbility(AbilityClass);

			CurrentAbilityChainCheckout = AbilityClass;

			const bool IsOK = this->TryActivateAbilityByClass(AbilityClass);
			Ret = IsOK;

			if (!IsOK)
			{
				AbilityChain->AckCheckOutAbility(AbilityClass);
			}
		}
	}

	return  Ret;
}

void UDLPlayerAbilitySysComponent::AbilityLocalInputPressed(const int32 InputId)
{
	AbilityInputMap.FindOrAdd(InputId) = EInputEvent::IE_Pressed;
	this->OnAbilityInputChange().Broadcast(EInputEvent::IE_Pressed, InputId);

	const bool IsDebug = CVarEnabledAbilityDebugInput.GetValueOnGameThread() != 0;

	UE_LOG(LogDLAbilitySystem, Log, TEXT("IsDebug[%s] AbilityLocalInputPressed %s"), TO_STR(IsDebug), ENUM_TO_STR(EPrimaryInputID, InputId));

	if (IsDebug)
	{
		Super::AbilityLocalInputPressed(InputId);
	}
	else
	{
		if (InputId < static_cast<int32>(EPrimaryInputID::DebugKeyBegin))
		{
			FDLPlayerInputCmdConstructAbility Arg;
			Arg.AbstractKeyID = InputId;
			const auto Cmd = UDLPlayerInputCmdFactory::CreateActivateAbilityCmd(this, Arg);

			if (Cmd)
			{
				if (const auto CmdSystem = UDLInputCmdSubsystem::Get(this))
				{
					CmdSystem->PushInputCmd(Cmd);
				}
			}
		}
	}
}

void UDLPlayerAbilitySysComponent::OnClientAbilityEnd(const FAbilityEndedData& Data) const
{
	if (AbilityChain)
	{
		AbilityChain->AckCheckOutAbility(Data.AbilityThatEnded->GetClass());
	}
}

void UDLPlayerAbilitySysComponent::DispatchAbilityLocalInput(EInputEvent Event, int32 InputId)
{
	if (Event == EInputEvent::IE_Pressed)
	{
		this->AbilityLocalInputPressed(InputId);
	}
	else if (Event == EInputEvent::IE_Released)
	{
		this->AbilityLocalInputReleased(InputId);
	}
}

void UDLPlayerAbilitySysComponent::RefreshDebugInput()
{
#if !UE_BUILD_SHIPPING
	if (const APlayerController* PC = AbilityActorInfo->PlayerController.Get())
	{
		const auto SubSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (SubSys)
		{
			if (const auto IMC = UDLAbilitySystemGlobal::Get().IMCAbilityDebug.LoadSynchronous())
			{
				if (bIsDebugInput && bIsEnabledInput)
				{
					SubSys->AddMappingContext(IMC, 0);
				}
				else
				{
					SubSys->RemoveMappingContext(IMC);
				}
			}
		}
	}
#endif
}

void UDLPlayerAbilitySysComponent::OnPlayerControllerSet()
{
	Super::OnPlayerControllerSet();

	FTopLevelAssetPath temp;
	//const FGameplayAbilityInputBinds BindsInfo({}, {}, TEXT("EPrimaryInputID"));
	const FGameplayAbilityInputBinds BindsInfo({}, {}, temp);
	if (AbilityActorInfo->IsLocallyControlledPlayer())
	{
		if (AbilityActorInfo->PlayerController.IsValid())
		{
			this->BindAbilityActivationToInputComponent(AbilityActorInfo->PlayerController->InputComponent, BindsInfo);
		}
	}

}

bool UDLPlayerAbilitySysComponent::GetShouldTick() const
{
#if !UE_BUILD_SHIPPING
	return true;
#else
	return Super::GetShouldTick();
#endif
}

void UDLPlayerAbilitySysComponent::DisabledAbilityInput()
{
	if (AbilityActorInfo && AbilityActorInfo->IsLocallyControlledPlayer())
	{
		if (const auto PC = AbilityActorInfo->PlayerController.Get())
		{
			const auto SubSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
			if (SubSys)
			{
				if (const auto IMC = UDLAbilitySystemGlobal::Get().IMCAbilityBase.LoadSynchronous())
				{
					SubSys->RemoveMappingContext(IMC);
				}
			}
		}
	}

	bIsEnabledInput = false;

	this->RefreshDebugInput();
}


void UDLPlayerAbilitySysComponent::EnabledAbilityInput()
{
	if (const APlayerController* PC = AbilityActorInfo->PlayerController.Get())
	{
		const auto SubSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (SubSys)
		{
			if (const auto IMC = UDLAbilitySystemGlobal::Get().IMCAbilityBase.LoadSynchronous())
			{
				SubSys->AddMappingContext(IMC, 0);
			}
		}
	}

	bIsEnabledInput = true;

	this->RefreshDebugInput();
}

void UDLPlayerAbilitySysComponent::BindAbilityActivationToInputComponent(
	UInputComponent* InputComponent,
	FGameplayAbilityInputBinds BindInfo)
{
	const auto& InputActionMap = UDLAbilitySystemGlobal::Get().AbilityInputActions;

	this->EnabledAbilityInput();

	if (const auto NewInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		const UEnum* EnumBinds = BindInfo.GetBindEnum();

		SetBlockAbilityBindingsArray(BindInfo);

		for (int32 Idx = 0; Idx < EnumBinds->NumEnums(); ++Idx)
		{

			if (const auto ActionData = InputActionMap.Find(static_cast<EPrimaryInputID>(Idx)))
			{
				if (const UInputAction* Action = ActionData->LoadSynchronous())
				{
					NewInput->BindAction(Action, ETriggerEvent::Triggered, this, &UDLPlayerAbilitySysComponent::DispatchAbilityLocalInput, EInputEvent::IE_Pressed, Idx);
					NewInput->BindAction(Action, ETriggerEvent::Canceled, this, &UDLPlayerAbilitySysComponent::DispatchAbilityLocalInput, EInputEvent::IE_Released, Idx);
				}
			}
		}
	}
	else
	{
		if (InputComponent)
		{
			Super::BindAbilityActivationToInputComponent(InputComponent, BindInfo);
		}
	}
}
