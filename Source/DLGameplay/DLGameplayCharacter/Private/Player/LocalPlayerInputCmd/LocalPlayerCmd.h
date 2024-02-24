#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "DLGameplayAbilityBase.h"
#include "DLPlayerInputCmd.h"
#include "ASC/DLPlayerAbilitySysComponent.h"
#include "Interface/ICharacterStateAccessor.h"
#include "IDLPlayerInputCmdSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/DLPlayerController.h"
#include "LocalPlayerCmd.generated.h"



UCLASS(Config = Game)
class DLGAMEPLAYCHARACTER_API UDLInputCmdRoll : public UDLPlayerInputCmdBase
{
	GENERATED_BODY()
public:

	virtual bool Exec(const FDLPlayerInputCmdExecContext& Context) override
	{
		if (!ensureAlwaysMsgf(Context.LocalCharacter && Context.LocalController, TEXT("Context 不合法")))
		{
			return false;
		}

		// 默认使用 输入向量的 Rotation,  InputRollRotation = ActorTransform * InputVector

		FRotator RollRotation = InputRollRotation;
		UE_LOG(LogTemp, Log, TEXT("<DLInputCmdRoll> InputOrientationRotation: %s    "), *InputOrientationRotation.ToString());

		bool bIsRolling = false;
		if (const auto MyContextObj = Cast<UGameplayAbility>(GetContextObject()))
		{
			bIsRolling = MyContextObj->AbilityTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Category.Special.Roll")));
		}

		const bool IsLock = ICharacterStateAccessor::Execute_K2_IsLockTargetUnit(Context.LocalCharacter);
		if (IsLock)
		{
			// 如果不为零，说明在移动中，否则就是后撤
			if (!InputRollRotation.IsNearlyZero())
			{
				const auto LockTarget = ICharacterStateAccessor::Execute_K2_GetLockTargetUnit(Context.LocalCharacter);
				const FVector CurLocation = Context.LocalCharacter->GetActorLocation();

				RollRotation = UKismetMathLibrary::FindLookAtRotation(
					CurLocation, LockTarget->GetActorLocation());
			}
		}


		UE_LOG(LogTemp, Log, TEXT("<DLInputCmdRoll> UDLInputCmdRoll %s    "), *RollRotation.ToString());

		const IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(Context.LocalCharacter);
		if (!Interface)
		{
			return false;
		}

		const auto ASC = Cast<UDLPlayerAbilitySysComponent>(Interface->GetAbilitySystemComponent());
		if (!ensureAlwaysMsgf(ASC, TEXT("这里一定能够获取到正确的  ASC  DebugSts(%s)"), *Context.ContextDebugStr))
		{
			return false;
		}

		FAbilityRollContext AbilityRollContext;
		AbilityRollContext.bValid = true;
		AbilityRollContext.TurnToRotation = RollRotation;
		AbilityRollContext.bImmediatelyTurn = bIsRolling;
		AbilityRollContext.InputOrientationRotation = InputOrientationRotation;
		AbilityRollContext.InputRollRotation = InputRollRotation;

		if (const auto PC = Cast<ADLPlayerController>(Context.LocalController))
		{
			PC->SyncRollContextData(AbilityRollContext);

			FScopedPredictionWindow NewScopedWindow(ASC, true);
			const FGameplayEventData EventData;
			const int32 Active = ASC->HandleGameplayEvent(ActivateRollEventTag, &EventData);
			return  Active > 0;
		}

		return false;
	}

private:

	UPROPERTY()
		FRotator InputOrientationRotation = FRotator::ZeroRotator;

	UPROPERTY()
		FRotator InputRollRotation = FRotator::ZeroRotator;

	UPROPERTY(Config)
		FGameplayTag ActivateRollEventTag;
};



UCLASS(Config = Game)
class DLGAMEPLAYCHARACTER_API UDLInputCmdSwitchSneakState : public UDLPlayerInputCmdBase
{
	GENERATED_BODY()
public:

	virtual bool Exec(const FDLPlayerInputCmdExecContext& Context) override
	{
		if (!ensureAlwaysMsgf(Context.LocalCharacter && Context.LocalController, TEXT("Context 不合法")))
		{
			return false;
		}

		const FGameplayTagContainer State = ICharacterStateAccessor::Execute_k2_GetCurrentCharacterState(Context.LocalCharacter);
		const bool IsOpenState = !State.HasTag(SneakStateTag);

		if (const auto PC = Cast<ADLPlayerController>(Context.LocalController))
		{
			PC->OnSwitchSneak.Broadcast(IsOpenState);
			return true;
		}

		return false;
	}

private:

	UPROPERTY(Config)
		FGameplayTag SneakStateTag;
};


UCLASS(Config = Game)
class DLGAMEPLAYCHARACTER_API UDLInputCmdMovement : public UDLPlayerInputCmdBase
{
	GENERATED_BODY()
public:

	virtual bool Exec(const FDLPlayerInputCmdExecContext& Context) override
	{
		if (!ensureAlwaysMsgf(Context.LocalCharacter && Context.LocalController, TEXT("Context 不合法")))
		{
			return false;
		}

		if (const auto PC = Cast<ADLPlayerController>(Context.LocalController))
		{
			PC->OnMovement.Broadcast();
			return true;
		}

		return false;
	}

public:

	UPROPERTY(BlueprintReadWrite)
		FVector InputVector;
};