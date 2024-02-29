#pragma once
#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomPawn.generated.h"

UCLASS(Blueprintable)
class ACustomPawn : public APawn
{
	GENERATED_UCLASS_BODY()
public:
	//ACustomPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;


	//~ Begin APawn Interface.
	virtual void PostInitializeComponents() override;
	virtual UPawnMovementComponent* GetMovementComponent() const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//virtual UPrimitiveComponent* GetMovementBase() const override final { return BasedMovement.MovementBase; }
	/*virtual float GetDefaultHalfHeight() const override;
	virtual void TurnOff() override;
	virtual void Restart() override;
	virtual void PawnClientRestart() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	
	virtual void DisplayDebug(class UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	virtual void RecalculateBaseEyeHeight() override;
	virtual void UpdateNavigationRelevance() override;*/
	//~ End APawn Interface

private:
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* Mesh;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UPawnMovementComponent* CharacterMovement;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USphereComponent* SphereComponent;
private:
	// 处理用于前后移动的输入。
	UFUNCTION()
		void MoveForward(float Value);

	// 处理用于左右移动的输入。
	UFUNCTION()
		void MoveRight(float Value);

	// 按下键时，设置跳跃标记。
	UFUNCTION()
		void StartJump();

	// 释放键时，清除跳跃标记。
	UFUNCTION()
		void StopJump();
};
