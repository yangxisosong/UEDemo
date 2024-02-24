// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLClientSubsystemBase.h"
#include "Components/Widget.h"
#include "DLCFrontEndSubsystem.generated.h"


//Tag对应的widget
USTRUCT()
struct FTagAndWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag Tag;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UUserWidget> Widget;
};



/**
 * 前端系统
 */
UCLASS(Abstract, Blueprintable)
class DLCFRONTENDSUBSYSTEM_API UDLCFrontEndSubsystem
		: public UDLClientSubsystemBase
{
	GENERATED_BODY()
public:

	virtual void InitSubsystem(const FClientSubsystemCollection& Collection) override;

	virtual void UninitSubsystem() override;

private:

	void OnPrimaryLayoutReady();


protected:

	// FrontEnd 的界面
	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<UWidget> WidgetClass;

	UPROPERTY()
		UUserWidget* Widget;
};
