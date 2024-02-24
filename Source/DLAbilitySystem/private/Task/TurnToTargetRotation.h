#pragma once

#include "CoreMinimal.h"
#include "DLAbilityActorInfo.h"
#include "DLGameplayAbilityBase.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Interface/ICharacterStateAccessor.h"
#include "Interface/IDLCharacterController.h"
#include "TurnToTargetRotation.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnToTotationEvent);


UCLASS()
class DLABILITYSYSTEM_API UAbilityTask_TurnToTargetRotation
	: public UAbilityTask
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_TurnToTargetRotation* TurnToRotation(UGameplayAbility* OwningAbility, const FRotator InRotation, const bool ImmediatelyTurn)
	{
		UAbilityTask_TurnToTargetRotation* Task = NewAbilityTask<UAbilityTask_TurnToTargetRotation>(OwningAbility);
		Task->Rotation = InRotation;
		Task->bTickingTask = true;
		Task->bImmediatelyTurn = ImmediatelyTurn;
		return Task;
	}


	UPROPERTY(BlueprintAssignable)
	FOnTurnToTotationEvent OnComplate;

protected:

	virtual void Activate() override
	{
		if (const auto MyAbility = Cast<UDLGameplayAbilityBase>(Ability))
		{
			if(const auto AbilityActorInfo = MyAbility->GetAbilityActorInfo())
			{
				if (AbilityActorInfo && AbilityActorInfo->GetCharacterController())
				{
					AbilityActorInfo->GetCharacterController()->TryTurnToRotation(Rotation, bImmediatelyTurn);
				}
			}
		}
	
		this->CheckComplate();
	}

	virtual void TickTask(float DeltaTime) override
	{
		this->CheckComplate();
	}

	void CheckComplate()
	{
		const bool IsComplate = ICharacterStateAccessor::Execute_K2_IsTurnToComplate(GetAvatarActor());
		if (IsComplate)
		{
			OnComplate.Broadcast();
			this->EndTask();
		}
	}

private:
	bool bImmediatelyTurn = false;
	FRotator Rotation;
};


UCLASS()
class DLABILITYSYSTEM_API UAbilityTask_WaitTurnComplate
	: public UAbilityTask
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAbilityTask_WaitTurnComplate* WaitTurnComplate(UGameplayAbility* OwningAbility, const bool IfTurnImmediatelyComplate = true, float OverrideYawSpeed = 0.f)
	{
		UAbilityTask_WaitTurnComplate* Task = NewAbilityTask<UAbilityTask_WaitTurnComplate>(OwningAbility);
		Task->bImmediatelyTurn = IfTurnImmediatelyComplate;
		Task->OverrideYawSpeed = OverrideYawSpeed;
		return Task;
	}


	UPROPERTY(BlueprintAssignable)
		FOnTurnToTotationEvent OnComplate;

protected:

	virtual void Activate() override
	{
		bTickingTask = true;

		const auto Controller = Cast<IDLCharacterController>(Ability->GetActorInfo().PlayerController.Get());
		if (Controller)
		{
			if (!ICharacterStateAccessor::Execute_K2_IsTurnToComplate(GetAvatarActor()))
			{
				FRotator Target = FRotator::ZeroRotator;
				ICharacterStateAccessor::Execute_K2_GetTurnToTargetRotation(GetAvatarActor(), Target);
				Controller->TryTurnToRotation(Target, bImmediatelyTurn);
			}
		}

		this->CheckComplate();
	}

	virtual void TickTask(float DeltaTime) override
	{
		this->CheckComplate();
	}

	void CheckComplate()
	{
		const bool IsComplate = ICharacterStateAccessor::Execute_K2_IsTurnToComplate(GetAvatarActor());
		if (IsComplate)
		{
			OnComplate.Broadcast();
			this->EndTask();
		}
	}

private:

	float OverrideYawSpeed = 0.f;

	bool bImmediatelyTurn = false;
};
