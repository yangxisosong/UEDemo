// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimDef/DLAnimStruct.h"
#include "Components/ActorComponent.h"
#include "DLAnimComponentBase.generated.h"

class AAnimationTestCharacter;
// class UDLLinkedAnimInstance;
class UDLMainAnimInstance;
class ADLPlayerCharacter;
class UDLCharacterMovementComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UDLAnimComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDLAnimComponentBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame by character.
	virtual void OnTick(float DeltaTime) {};

public:
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) PURE_VIRTUAL(UDLAnimComponentBase::OnMovementModeChanged);

	virtual void OnLanded(const FHitResult& Hit) PURE_VIRTUAL(UDLAnimComponentBase::Landed);
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation | CharacterInfo")
		const FAnimCharacterInfo& GetAnimCharacterInfo() const
	{
		return  AnimCharacterInfo;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation | CharacterInfo")
		FORCEINLINE	FAnimCharacterInfo& GetAnimCharacterInfoRef()
	{
		return  AnimCharacterInfo;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
		UDLMainAnimInstance* GetMainAnimIns() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MovementComp")
		UDLCharacterMovementComponent* GetMovementComponent();

	// UDLLinkedAnimInstance* GetCurrentLinkedInstance() const;
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Animation | CharacterInfo")
		FAnimCharacterInfo AnimCharacterInfo;
protected:
	UFUNCTION(BlueprintPure)
		AAnimationTestCharacter* GetCharacterOwner() const;
};
