// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLLocalPlayer.generated.h"

UCLASS()
class DLAPPFRAMEWORK_API UDLLocalPlayer
	: public ULocalPlayer
{
	GENERATED_BODY()
public:

	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerControllerSetDelegate, UDLLocalPlayer* LocalPlayer, APlayerController* PlayerController);
	FPlayerControllerSetDelegate OnPlayerControllerSet;

	FDelegateHandle CallAndRegister_OnPlayerControllerSet(FPlayerControllerSetDelegate::FDelegate Delegate);

};
