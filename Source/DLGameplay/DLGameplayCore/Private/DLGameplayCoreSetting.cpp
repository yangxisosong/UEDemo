// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameplayCoreSetting.h"

FName UDLGameplayCoreSetting::GetSectionName() const
{
	return TEXT("DLGameplayCore");
}

FName UDLGameplayCoreSetting::GetCategoryName() const
{
	return TEXT("Game");
}

FName UDLGameplayCoreSetting::GetContainerName() const
{
	return TEXT("Project");
}


#if WITH_EDITOR

FText UDLGameplayCoreSetting::GetSectionText() const
{
	return FText::FromString(TEXT("DLGameplayCore"));
}

#endif


const UDLGameplayCoreSetting* UDLGameplayCoreSetting::GetDLGameplayCoreSetting()
{
	return UDLGameplayCoreSetting::Get();
}


const UDLGameplayCoreSetting* UDLGameplayCoreSetting::Get()
{
	return UDLGameplayCoreSetting::StaticClass()->GetDefaultObject<UDLGameplayCoreSetting>();
}

