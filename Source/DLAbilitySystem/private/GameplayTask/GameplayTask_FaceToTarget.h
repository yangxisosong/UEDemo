#pragma once


#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "CoreMinimal.h"
#include "AIController.h"
#include "Interface/ICharacterStateAccessor.h"
#include "Interface/IDLCharacterController.h"

#include "GameplayTask_FaceToTarget.generated.h"


UCLASS()
class UGameplayTask_FaceToTarget
	: public UGameplayTask
{
	GENERATED_BODY()

	UGameplayTask_FaceToTarget(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
	{
		bTickingTask = true;
	};	
public:
	
	UFUNCTION(BlueprintCallable, Category = "AI|Tasks", meta = (DefaultToSelf = "Controller", BlueprintInternalUseOnly = "TRUE"))
	static UGameplayTask_FaceToTarget* FaceToTarget(const TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,AAIController* AIController,AActor* TargetActor,bool ImmediatelyTurn = false,bool IsOverrideTurnSpeed = false,float OverrideTurnSpeed = 50)
	{
		if (AIController == nullptr) return nullptr;
		UGameplayTask_FaceToTarget* MyTask = NewTask<UGameplayTask_FaceToTarget>(AIController);
		if (MyTask)
		{
			MyTask->AIController = AIController;
			MyTask->TargetActor = TargetActor;
			MyTask->ImmediatelyTurn = ImmediatelyTurn;
			MyTask->IsOverrideTurnSpeed = IsOverrideTurnSpeed;
			MyTask->OverrideTurnSpeed = OverrideTurnSpeed;
			MyTask->InitTask(*TaskOwner,TaskOwner->GetGameplayTaskDefaultPriority());
			return MyTask;
		}
		return nullptr;
	};
		
protected:
	UPROPERTY()
	AAIController* AIController;
	
	UPROPERTY()
	AActor* TargetActor;
	bool ImmediatelyTurn = false;
	bool IsOverrideTurnSpeed = false;
	float OverrideTurnSpeed = 50;
	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate OnFinished;

	virtual void Activate() override
	{
		//UCharacterMovementComponent* CMC = AIController->GetCharacter()->GetCharacterMovement();
		const FRotator CurrentRot = AIController->GetPawn()->GetActorRotation();
		const FVector FocalPoint = TargetActor->GetActorLocation();
		const FRotator DesiredRot = (FocalPoint - AIController->GetPawn()->GetPawnViewLocation()).Rotation();
		//FRotator DeltaRot = CMC->GetDeltaRotation(DeltaTime);
		
		auto DLCharacterController = Cast<IDLCharacterController>(AIController);
		if (DLCharacterController)
		{
			if (IsOverrideTurnSpeed)
			{
				DLCharacterController->TryTurnToRotation(DesiredRot,ImmediatelyTurn,OverrideTurnSpeed);
			}
			else
			{
				DLCharacterController->TryTurnToRotation(DesiredRot,ImmediatelyTurn,0);
			}
		}
		
	};
public:
	virtual void TickTask(float DeltaTime) override
	{		
		if (AIController == NULL || AIController->GetPawn() == NULL)
		{
			OnFinished.Broadcast();
			EndTask();
			return;
		}
		if (!TargetActor)
		{
			OnFinished.Broadcast();
			EndTask();
			return;
		}
		
		auto CSA = Cast<ICharacterStateAccessor>(AIController->GetPawn());
		if (CSA)
		{
			if (CSA->Execute_K2_IsTurnToComplate(AIController->GetPawn()))
			{
				OnFinished.Broadcast();
				EndTask();
			}
		}
		else
		{
			OnFinished.Broadcast();
			EndTask();
		}
		
	};
};


