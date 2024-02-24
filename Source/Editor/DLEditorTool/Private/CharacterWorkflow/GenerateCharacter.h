// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "UObject/Object.h"
#include "GenerateCharacter.generated.h"


UCLASS(BlueprintType)
class UDLGenerateCharacterBuilder : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Builder)
		FString CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Builder)
		bool bAsNPC = false;

protected:

	UFUNCTION(BlueprintCallable)
		bool Build();
};


UCLASS()
class DLEDITORTOOL_API UGenerateCharacterWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:


public:

	UPROPERTY(BlueprintReadWrite, Transient)
		UDLGenerateCharacterBuilder* Builder;
};




