// Fill out your copyright notice in the Description page of Project Settings.


#include "DLAppFrameworkSetting.h"

UDLAppFrameworkSetting::UDLAppFrameworkSetting()
{
	SectionName = TEXT("DLAppFramework");
	CategoryName = TEXT("Game");
}

const UDLAppFrameworkSetting* UDLAppFrameworkSetting::Get()
{
	return GetMutableDefault<UDLAppFrameworkSetting>();
}
