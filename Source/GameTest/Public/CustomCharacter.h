#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CustomCharacter.generated.h"

UCLASS(Blueprintable)
class ACustomCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	ACustomCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector MoveDir = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
		bool IsMove = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool IsMoveRotation = false;
};
