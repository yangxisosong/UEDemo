// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/Object.h"
#include "DLAppFrameworkSetting.generated.h"

/**
 * 
 */
UCLASS(Config = DLAppFramework)
class DLAPPFRAMEWORK_API UDLAppFrameworkSetting
	: public UDeveloperSettings
{
	GENERATED_BODY()
public:

	UDLAppFrameworkSetting();

	static const UDLAppFrameworkSetting* Get();

public:

	UPROPERTY(Config, EditAnywhere, Category = FrameworkMsgTag)
		FGameplayTag MsgTypeGameInstanceInitPost; 
};
