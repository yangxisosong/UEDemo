// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "Engine/DataTable.h"
#include "UObject/Object.h"
#include "DLGameplayLaunchSetting.generated.h"

/**
 * Only Editor
 */
UCLASS(Config = UserGameplayConfig)
class DLAPPLICATION_API UDLGameplayLaunchSetting : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditAnywhere, Category = LaunchGameplay, meta = (GetOptions = GetPlayerCharacterNames))
		FString SelectCharacterName;


	UPROPERTY(Config, EditAnywhere, Category = Config, AdvancedDisplay)
		TSoftObjectPtr<UDataTable> CharacterDT;

	UPROPERTY(Config, EditAnywhere, Category= LaunchGameplay)
		int32 ClientNumber = 1;

protected:

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif

public:

	UFUNCTION()
	TArray<FString> GetPlayerCharacterNames() const
	{
		TArray<FString> Ret;
		if (CharacterDT.LoadSynchronous())
		{
			TArray<FDTCharacterInfoBaseRow*> Rows;
			CharacterDT->GetAllRows(TEXT("GetPlayerCharacterNames"), Rows);

			for (const auto& Row : Rows)
			{
				Ret.Add(Row->BaseInfo.Name.ToString());
			}
		}
		return Ret;
	}


	FName GetCurrentSelectCharacterID()const
	{
		if (CharacterDT.LoadSynchronous())
		{
			TArray<FDTCharacterInfoBaseRow*> Rows;
			CharacterDT->GetAllRows(TEXT("GetPlayerCharacterNames"), Rows);

			for (const auto& Row : Rows)
			{
				if (Row->BaseInfo.Name.ToString() == SelectCharacterName)
				{
					return Row->BaseInfo.Id;
				}
			}
		}

		return {};
	}

	UFUNCTION(BlueprintCallable)
	static  UDLGameplayLaunchSetting* GetGameplayLaunchSetting()
	{
		return GetMutableDefault<UDLGameplayLaunchSetting>();
	}
};
