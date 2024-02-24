// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSavingSubSystem.h"

#include "IDLNPCSavingAccessor.h"
#include "NPCSaved/DLNPCSaved.h"
#include "Simple/SimplePlayerSaving.h"


IDLGameSavingSystem* IDLGameSavingSystem::Get(const UObject* WorldContext)
{
	const auto World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	if (World)
	{
		return World->GetSubsystem<UGameSavingSubSystem>();
	}
	return nullptr;
}

UGameSavingSubSystem::UGameSavingSubSystem()
{
	UClass* PlaySavingClass = this->GetClass()->GetDefaultObject<UGameSavingSubSystem>()->PlayerSavingClass.LoadSynchronous();
	checkfSlow(PlaySavingClass, TEXT("忘记配置存档数据"));
	if (PlaySavingClass)
	{
		PlayerSaving = CreateDefaultSubobject(TEXT("SimplePlayerSaving"), PlaySavingClass, PlaySavingClass, true, false);
	}

	NPCSaving = CreateDefaultSubobject<UDLNPCSaved>(TEXT("NPCSaving"));
}

IDLPlayerSavingAccessor* UGameSavingSubSystem::GetPlayerSavingAccessor()
{
	return Cast<IDLPlayerSavingAccessor>(PlayerSaving);
}

IDLNPCSavingAccessor* UGameSavingSubSystem::GetNPCSavingAccessor()
{
	return Cast<IDLNPCSavingAccessor>(NPCSaving);
}
