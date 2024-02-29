#include "CustomCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

ACustomCharacter::ACustomCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	UE_LOG(LogTemp, Warning, TEXT("ACustomCharacter Construct"));
}

void ACustomCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MoveDir != FVector::ZeroVector)
	{
		//sconst FVector NewLocation = GetActorLocation() + (MoveDir * DeltaSeconds* MoveSpeed);
		//SetActorLocation(NewLocation);

		//UE_LOG(LogTemp, Warning, TEXT("Tick Move : %s"), *NewLocation.ToString());
	}
}

void ACustomCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 设置"移动"绑定。
	PlayerInputComponent->BindAxis("MoveForward", this, &ACustomCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACustomCharacter::MoveRight);

	// 设置"操作"绑定。
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACustomCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACustomCharacter::StopJump);
}

void ACustomCharacter::MoveForward(float Value)
{
	MoveDir.Y = Value;
	//UE_LOG(LogTemp, Warning, TEXT("Directionis Value %f"), Value);
	if (Value != 0)
	{
		FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
		Direction.Normalize();
		UE_LOG(LogTemp, Warning, TEXT("Directionis %s"), *Direction.ToString());

		

		auto InputVec = Direction;//GetMovementComponent()->GetLastInputVector();
		InputVec.Z = 0;
		auto CurrentDir = GetActorForwardVector();

		auto cos = FVector::DotProduct(InputVec, CurrentDir)/(InputVec.Size() * CurrentDir.Size());
		UE_LOG(LogTemp, Warning, TEXT("cos: %f"), cos);
		if(FMath::Abs(cos) >0.95)
		{
			Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed=100;
		}
		else
		{
			Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed = 10;
		}
		AddMovementInput(Direction, Value);

		const FRotator CurrentRotation = GetActorRotation();
		FRotator NewCurrentRotation = Controller->GetControlRotation();
		NewCurrentRotation.Pitch = CurrentRotation.Pitch;
		NewCurrentRotation.Roll = CurrentRotation.Roll;
		//SetActorRotation(NewCurrentRotation);

	}
}

void ACustomCharacter::MoveRight(float Value)
{
	MoveDir.X = Value;
	//UE_LOG(LogTemp, Warning, TEXT("Directionis Value %f"), Value);
	if (Value != 0)
	{
		FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
		Direction.Normalize();

		auto InputVec = Direction;//GetMovementComponent()->GetLastInputVector();
		InputVec.Z = 0;
		auto CurrentDir = GetActorForwardVector();

		auto cos = FVector::DotProduct(InputVec, CurrentDir) / (InputVec.Size() * CurrentDir.Size());
		UE_LOG(LogTemp, Warning, TEXT("cos: %f"), cos);
		if (FMath::Abs(cos) > 0.95)
		{
			Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed = 100;
		}
		else
		{
			Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed = 10;
		}
		AddMovementInput(Direction, Value);

	}
}

void ACustomCharacter::StartJump()
{

}

void ACustomCharacter::StopJump()
{

}
