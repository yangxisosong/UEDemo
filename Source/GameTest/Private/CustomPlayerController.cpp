#include "CustomPlayerController.h"
#include "EnhancedInputSubsystems.h"
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
		const auto EnhancedInputSubSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if (EnhancedInputSubSys)
		{
			UE_LOG(LogTemp, Log, TEXT("GetEnhancedInput oK"));
			UInputMappingContext* IMC = InputMap.LoadSynchronous();
			if (IMC)
			{
				EnhancedInputSubSys->AddMappingContext(IMC, 0);
			}
			//EnhancedInputSubSys->;
			auto GetMap = IMC->GetMappings();
			for (auto& i : GetMap)
			{
				;
				UE_LOG(LogTemp, Log, TEXT("GetMappings oK"));
			}

			auto ActionMap = EnhancedInputSubSys->GetAllPlayerMappableActionKeyMappings();
			auto setting = EnhancedInputSubSys->GetUserSettings();
			for (auto& i : ActionMap)
			{
				i.Action;
				UE_LOG(LogTemp, Log, TEXT("GetPawn oK"));
			}
			UEnhancedPlayerInput* Input = EnhancedInputSubSys->GetPlayerInput();
			for (auto& i : Input->ActionMappings)
			{
				i.ActionName;
				UE_LOG(LogTemp, Log, TEXT("GetPawn oK"));
			}

		}


		InputComponent->BindKey(EKeys::W, IE_Pressed, this, &ACustomPlayerController::OnKeyDown);
		InputComponent->BindKey(EKeys::W, IE_Released, this, &ACustomPlayerController::OnKeyUp);

		InputComponent->BindKey(EKeys::A, IE_Pressed, this, &ACustomPlayerController::OnKeyDown);
		InputComponent->BindKey(EKeys::A, IE_Released, this, &ACustomPlayerController::OnKeyUp);

		InputComponent->BindKey(EKeys::S, IE_Pressed, this, &ACustomPlayerController::OnKeyDown);
		InputComponent->BindKey(EKeys::S, IE_Released, this, &ACustomPlayerController::OnKeyUp);

		InputComponent->BindKey(EKeys::D, IE_Pressed, this, &ACustomPlayerController::OnKeyDown);
		InputComponent->BindKey(EKeys::D, IE_Released, this, &ACustomPlayerController::OnKeyUp);

		InputComponent->BindAxisKey(EKeys::MouseX, this, &ACustomPlayerController::OnMouseMoveX);
		InputComponent->BindAxisKey(EKeys::MouseY, this, &ACustomPlayerController::OnMouseMoveY);
	}
}

void ACustomPlayerController::Tick(float DeltaSeconds)
{
	MyCharacter->MoveForward(MyCharacter->MoveDir.X);
	MyCharacter->MoveRight(MyCharacter->MoveDir.Y);
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
		MyCharacter->MoveDir.X = 1;
	}
	else if(Key == EKeys::A)
	{
		MyCharacter->MoveDir.Y = -1;
	}
	else if (Key == EKeys::S)
	{
		MyCharacter->MoveDir.X = -1;
	}
	else if (Key == EKeys::D)
	{
		MyCharacter->MoveDir.Y = 1;
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
		MyCharacter->MoveDir.X = 0;
	}
	else if (Key == EKeys::A)
	{
		MyCharacter->MoveDir.Y = 0;
	}
	else if (Key == EKeys::S)
	{
		MyCharacter->MoveDir.X = 0;
	}
	else if (Key == EKeys::D)
	{
		MyCharacter->MoveDir.Y = 0;
	}

	//MyCharacter->MoveDir.Normalize();
}

void ACustomPlayerController::OnMouseMoveX(const float InValue)
{
	//UE_LOG(LogTemp, Warning, TEXT("OnMouseMoveX Key:%f"), InValue);
	AddYawInput(InValue);
}

void ACustomPlayerController::OnMouseMoveY(const float InValue)
{
	//UE_LOG(LogTemp, Warning, TEXT("OnMouseMoveY Key:%f"), InValue);
	AddPitchInput(InValue);
}

