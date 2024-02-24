// Fill out your copyright notice in the Description page of Project Settings.


#include "IDLClientAppFramework.h"

#include "IDLClientAppInstance.h"
#include "Kismet/GameplayStatics.h"

IDLClientAppInstance* IDLClientAppInstance::Get(const UObject* WorldContext)
{
	return Cast<IDLClientAppInstance>(UGameplayStatics::GetGameInstance(WorldContext)); 
}

IDLClientAppInstance* IDLClientAppInstance::GetChecked(const UObject * WorldContext)
{
	return CastChecked<IDLClientAppInstance>(UGameplayStatics::GetGameInstance(WorldContext)); 
}