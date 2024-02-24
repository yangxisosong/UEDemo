// Fill out your copyright notice in the Description page of Project Settings.


#include "DLClientSubsystemBase.h"
#include "Kismet/GameplayStatics.h"


ETickableTickType UDLClientSubsystemBase::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool UDLClientSubsystemBase::IsTickable() const
{
	if(this->HasAnyFlags(EObjectFlags::RF_ClassDefaultObject))
	{
		return false;
	}

	return bEnabledTick;
}

TStatId UDLClientSubsystemBase::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UDLClientSubsystemBase, STATGROUP_Tickables);
}

UGameInstance* UDLClientSubsystemBase::GetGameInstance() const
{
	return UGameplayStatics::GetGameInstance(this);
}

APlayerController* UDLClientSubsystemBase::GetPlayerControllerChecked() const
{
	const auto PC = UGameplayStatics::GetPlayerController(this, 0);
	check(PC);
	return PC;
}
