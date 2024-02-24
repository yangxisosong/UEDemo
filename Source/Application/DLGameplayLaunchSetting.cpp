// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameplayLaunchSetting.h"

#if WITH_EDITOR

void UDLGameplayLaunchSetting::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	this->SaveConfig();
}

#endif
