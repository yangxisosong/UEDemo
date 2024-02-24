// Fill out your copyright notice in the Description page of Project Settings.


#include "DLDrawDebugSetting.h"

FName UDLDrawDebugSetting::GetSectionName() const
{
	return TEXT("DLDrawDebugSetting");
}

FName UDLDrawDebugSetting::GetCategoryName() const
{
	return TEXT("Game");
}

FName UDLDrawDebugSetting::GetContainerName() const
{
	return TEXT("Project");
}

#if WITH_EDITOR
FText UDLDrawDebugSetting::GetSectionText() const
{
	return FText::FromString(TEXT("DLDrawDebugSetting"));
}
#endif

UDLDrawDebugSetting* UDLDrawDebugSetting::Get()
{
	return UDLDrawDebugSetting::StaticClass()->GetDefaultObject<UDLDrawDebugSetting>();
}

const UDLDrawDebugSetting* UDLDrawDebugSetting::GetDLDrawDebugSetting()
{
	return UDLDrawDebugSetting::Get();
}
