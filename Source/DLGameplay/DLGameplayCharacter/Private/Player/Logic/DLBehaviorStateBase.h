// ReSharper disable CppMemberFunctionMayBeConst
#pragma once


#include "CoreMinimal.h"
#include "DLPlayerInputCmd.h"
#include "GameplayTagContainer.h"
#include "IDLPlayerInputCmdSystem.h"
#include "SMStateInstance.h"
#include "DLGamePlayCharacterDef.h"
#include "Player/DLPlayerController.h"
#include "Player/DLPlayerCharacter.h"
#include "DLBehaviorStateBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDLBehaviorStateBase, Log, All);
DEFINE_LOG_CATEGORY(LogDLBehaviorStateBase);



UCLASS()
class UDLBehaviorStateBase
	: public USMStateInstance
{
	GENERATED_BODY()

protected:

	virtual void OnStateBegin_Implementation() override
	{
		Super::OnStateBegin_Implementation();


		if (MarkStateTags.Num() > 0)
		{
			const auto Character = PlayerController->GetPlayerCharacter();
			if (Character)
			{
				Character->AddOwnerTags(MarkStateTags);
			}
		}

		if (InputCmdSys)
		{
			InputCmdSys->AddAllowCmdTags(AllowInputCmdTags,  FString::Printf(TEXT("State %s"), *GetNodeName()));

			if (bTryCheckMoveCmd)
			{
				InputCmdSys->TryCheckMovementCmd();
			}
		}
	}

	virtual void OnStateEnd_Implementation() override
	{
		Super::OnStateEnd_Implementation();

		if (InputCmdSys)
		{
			InputCmdSys->RemoveCmdTags(AllowInputCmdTags, FString::Printf(TEXT("State %s"), *GetNodeName()));

			if (bExecInputCmdWhenEndState)
			{
				const auto Cmd = InputCmdSys->GetCurrentCacheCmd();
				if (Cmd)
				{
					InputCmdSys->ClearInputCmd();
					InputCmdSys->ExecCmd(Cmd, FString::Printf(TEXT("State [%s]  End  Exec Input Cmd "), *this->GetFName().ToString()));
				}
				else
				{
					if (bTryCheckMoveCmd)
					{
						InputCmdSys->TryCheckMovementCmd();
					}
				}
			}
		}


		if (MarkStateTags.Num() > 0)
		{
			const auto Character = PlayerController->GetPlayerCharacter();
			if (Character)
			{
				Character->RemoveOwnerTags(MarkStateTags);
			}
		}
	}

	virtual void OnStateUpdate_Implementation(const float DeltaSeconds) override
	{
		Super::OnStateUpdate_Implementation(DeltaSeconds);
	}

	virtual void OnStateInitialized_Implementation() override
	{
		Super::OnStateInitialized_Implementation();

		PlayerController = Cast<ADLPlayerController>(GetContext());

		if (PlayerController->IsLocalPlayerController())
		{
			InputCmdSys = UDLInputCmdSubsystem::Get(PlayerController);
		}
	}

public:

	UPROPERTY(EditDefaultsOnly)
		bool bExecInputCmdWhenEndState = true;


	UPROPERTY(EditDefaultsOnly)
		bool bTryCheckMoveCmd = false;

	/**
	 * @brief 接受的指令 Tag
	 */
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Gameplay.PlayerInputCmd.Category"))
		FGameplayTagContainer  AllowInputCmdTags;


	UPROPERTY(EditDefaultsOnly)
		FGameplayTagContainer MarkStateTags;

	UPROPERTY(BlueprintReadOnly, meta = (HideOnNode))
		UDLInputCmdSubsystem* InputCmdSys;

	UPROPERTY(BlueprintReadOnly, meta = (HideOnNode))
		ADLPlayerController* PlayerController;
};


// 这个状态目前只是占位置的
UCLASS()
class UDLAbilityState : public UDLBehaviorStateBase
{
	GENERATED_BODY()
public:

	virtual void OnStateBegin_Implementation() override
	{
		Super::OnStateBegin_Implementation();

		if (PlayerController)
		{
			PlayerController->OnMovement.AddDynamic(this, &UDLAbilityState::OnMovement);
		}
	}

	virtual void OnStateEnd_Implementation() override
	{
		if (PlayerController)
		{
			PlayerController->OnMovement.RemoveDynamic(this, &UDLAbilityState::OnMovement);
		}

		Super::OnStateEnd_Implementation();
	}

	virtual void OnStateInitialized_Implementation() override
	{
		Super::OnStateInitialized_Implementation();

		const auto Character = PlayerController->GetPlayerCharacter();
		if (Character)
		{
			ASC = Character->GetAbilitySystemComponent();
		}

		if (ASC)
		{
			ActiveHandle = ASC->AbilityActivatedCallbacks.AddUObject(this, &UDLAbilityState::OnAbilityActive);
			EndHandle = ASC->OnAbilityEnded.AddUObject(this, &UDLAbilityState::OnAbilityEnd);
		}
	}

	virtual void OnStateShutdown_Implementation() override
	{
		if (ASC)
		{
			ASC->AbilityActivatedCallbacks.Remove(ActiveHandle);
			ASC->OnAbilityEnded.Remove(EndHandle);
		}

		Super::OnStateShutdown_Implementation();
	}

protected:

	void OnAbilityActive(UGameplayAbility* Ability)
	{
		
	}

	void OnAbilityEnd(const FAbilityEndedData& EndedData)
	{
		
	}

	UFUNCTION(BlueprintNativeEvent)
		void OnMovement();

	void OnMovement_Implementation()
	{
		if (ASC)
		{
			const auto Ability = ASC->GetAnimatingAbility();
			if (Ability)
			{
				UE_LOG(LogDLBehaviorStateBase, Log, TEXT("UDLAbilityState::OnMovement  CancelAbility %s"), *GetNameSafe(Ability));
				Ability->K2_CancelAbility();
			}
		}
	}


public:

	UPROPERTY()
		UAbilitySystemComponent* ASC;


	FDelegateHandle ActiveHandle;
	FDelegateHandle EndHandle;

};

