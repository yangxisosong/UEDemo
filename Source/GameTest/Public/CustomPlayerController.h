#pragma once
#include "CoreMinimal.h"
#include "CustomCharacter.h"
#include "InputMappingContext.h"
#include "CustomPlayerController.generated.h"

UCLASS(Blueprintable)
class ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ACustomPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;
public:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UInputMappingContext> InputMap;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UInputAction> MoveAction;
private:
	void OnKeyDown(FKey Key);
	void OnKeyUp(FKey Key);
	void OnMouseMoveX(const float InValue);
	void OnMouseMoveY(const float InValue);
private:
	ACustomCharacter* MyCharacter;
};
