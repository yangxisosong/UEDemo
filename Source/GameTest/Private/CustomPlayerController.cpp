#include "CustomPlayerController.h"
#include "Components/InputComponent.h"

ACustomPlayerController::ACustomPlayerController(const FObjectInitializer& ObjectInitializer)
	:APlayerController(ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("ACustomPlayerController Construct"));
}

void ACustomPlayerController::BeginPlay()
{
	Super::BeginPlay();

	const auto TempPawn = GetPawn();
	if (TempPawn)
	{
		UE_LOG(LogTemp, Log, TEXT("GetPawn oK"));
		MyCharacter = Cast<ACustomCharacter>(TempPawn);
	}

}

void ACustomPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAxis("Turn", this, &ACustomPlayerController::AddYawInput);
		InputComponent->BindAxis("LookUp", this, &ACustomPlayerController::AddPitchInput);

		/*InputComponent->BindKey(EKeys::W, IE_Pressed, this, &ACustomPlayerController::OnKeyDown);
		InputComponent->BindKey(EKeys::W, IE_Released, this, &ACustomPlayerController::OnKeyUp);

		InputComponent->BindKey(EKeys::A, IE_Pressed, this, &ACustomPlayerController::OnKeyDown);
		InputComponent->BindKey(EKeys::A, IE_Released, this, &ACustomPlayerController::OnKeyUp);

		InputComponent->BindKey(EKeys::S, IE_Pressed, this, &ACustomPlayerController::OnKeyDown);
		InputComponent->BindKey(EKeys::S, IE_Released, this, &ACustomPlayerController::OnKeyUp);

		InputComponent->BindKey(EKeys::D, IE_Pressed, this, &ACustomPlayerController::OnKeyDown);
		InputComponent->BindKey(EKeys::D, IE_Released, this, &ACustomPlayerController::OnKeyUp);*/
	}
}

void ACustomPlayerController::OnKeyDown(FKey Key)
{
	UE_LOG(LogTemp, Warning, TEXT("OnKeyDown Key: %s"), *Key.ToString());
	if(!MyCharacter)
	{
		return;
	}
	if (Key == EKeys::W)
	{
		MyCharacter->MoveDir.X += 1;
	}
	else if(Key == EKeys::A)
	{
		MyCharacter->MoveDir.Y += -1;
	}
	else if (Key == EKeys::S)
	{
		MyCharacter->MoveDir.X += -1;
	}
	else if (Key == EKeys::D)
	{
		MyCharacter->MoveDir.Y += 1;
	}

	//MyCharacter->MoveDir.Normalize();
}

void ACustomPlayerController::OnKeyUp(FKey Key)
{
	UE_LOG(LogTemp, Warning, TEXT("OnKeyUp Key: %s"), *Key.ToString());
	if (!MyCharacter)
	{
		return;
	}

	if (Key == EKeys::W)
	{
		MyCharacter->MoveDir.X -= 1;
	}
	else if (Key == EKeys::A)
	{
		MyCharacter->MoveDir.Y -= -1;
	}
	else if (Key == EKeys::S)
	{
		MyCharacter->MoveDir.X -= -1;
	}
	else if (Key == EKeys::D)
	{
		MyCharacter->MoveDir.Y -= 1;
	}

	//MyCharacter->MoveDir.Normalize();
}

