// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/DLAnimComponentBase.h"
#include "DLGameplayCore/Public/Animation/AnimDebug/DLAnimDebugInterface.h"
#include "GameFramework/Character.h"
#include "AnimationTestCharacter.generated.h"

class UDLLinkedAnimInstance;
class UDLMainAnimInstance;
UCLASS()
class AAnimationTestCharacter : public ACharacter, public IDLAnimDebugInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	explicit AAnimationTestCharacter(const FObjectInitializer& ObjectInitializer);

	// virtual void PostInitProperties() override;
	//
	virtual void PreInitializeComponents() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Landed(const FHitResult& Hit) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual FRotator GetLastVelocityRotation_Debug_Implementation() override;

	virtual FRotator GetLastMovementInputRotation_Debug_Implementation() override;

	virtual FRotator GetTargetRotation_Debug_Implementation()override;

	virtual FString GetCharacterMainState_Debug_Implementation() override;

	virtual FString GetOverlayState_Debug_Implementation() override;

	virtual float GetCurveValue_Debug_Implementation(FName Curve) override;

public:
	UFUNCTION(BlueprintPure, Category = "Character | Animation")
		UDLMainAnimInstance* GetMainAnimInstance() const;

	// UFUNCTION(BlueprintPure, Category = "Character | Animation")
	// 	UDLLinkedAnimInstance* GetCurrentLinkedInstance() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character | Animation")
		const FAnimConfig& GetAnimConfig() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Config")
		const FMovementSettings& GetTargetMovementSettings() const;

	UFUNCTION(BlueprintCallable)
		void ChangeMainState(ECharacterMainState State);


	UFUNCTION(BlueprintCallable)
		void ChangeOverlayState(EDLAnimOverlayState State);

	UFUNCTION(BlueprintCallable)
		void ChangeMovementAction(const EMovementAction NewAction);

	UFUNCTION(BlueprintPure)
		FORCEINLINE	UDLAnimComponentBase* GetAnimComponent() const
	{
		return AnimComponent;
	}

	UFUNCTION(BlueprintCallable)
		void SetHasHorizontalInput(bool NewValue);

protected:
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;


private:
	void LoadAnimConfig();

public:
	// UPROPERTY(EditAnywhere, BlueprintReadOnly)
	// 	TSubclassOf<UDLAnimComponentBase> AnimComponentClass = UDLAnimComponentBase::StaticClass();

	// UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	// 	TSubclassOf<UDLCharacterMovementComponent> MovementComponentClass = UDLCharacterMovementComponent::StaticClass();

	template<typename AnimCompClass>
	AnimCompClass* GetAnimComp()
	{
		return Cast<AnimCompClass>(AnimComponent);
	}
protected:
	UPROPERTY(BlueprintReadWrite)
		UDLAnimComponentBase* AnimComponent = nullptr;

	UPROPERTY(EditAnywhere)
		FString AnimConfigName;

	UPROPERTY(EditAnywhere)
		UDataTable* AnimConfigDataTable = nullptr;
protected:
	FAnimConfig AnimConfig;

public:
	UPROPERTY(BlueprintReadWrite)
		EGaitType ForceGait = EGaitType::None;
};
