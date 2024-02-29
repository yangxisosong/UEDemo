#pragma once
#include "CoreMinimal.h"
#include "CustomCharacter.h"
#include "CustomPlayerController.generated.h"

UCLASS(Blueprintable)
class ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ACustomPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void OnKeyDown(FKey Key);
	void OnKeyUp(FKey Key);

private:
	ACustomCharacter* MyCharacter;
};
