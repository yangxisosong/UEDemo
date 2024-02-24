// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "NativeGameplayTags.h"
#include "IDLPrimaryPlayerAbilitySystem.generated.h"


USTRUCT(BlueprintType)
struct FDLAbilityDesc
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName AbilityId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText AbilityNameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText AbilityDescText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTag AbilityUISlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CDDuration;

public:

	bool operator==(const FName Id) const
	{
		return AbilityId == Id;
	}
};


UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UDLPrimaryPlayerAbilitySystem : public UInterface
{
	GENERATED_BODY()
};


class DLGAMEPLAYTYPES_API IDLPrimaryPlayerAbilitySystem
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	virtual bool FindActiveAbilityDesc(FName AbilityId, FDLAbilityDesc& OutDesc) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void GetAllActiveAbilityDesc(TArray<FDLAbilityDesc>& AbilityDescArray) = 0;


	virtual FSimpleMulticastDelegate& OnActivateAbilitiesChange() = 0;
};


namespace EDLGameplayMsgType
{
	DLGAMEPLAYTYPES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LocalPlayer_Gameplay_AbilityCD);
}


USTRUCT(BlueprintType, meta=(MessageType = "DLMsgType.LocalPlayer.Gameplay.ApplyAbilityCD"))
struct FDLLocalPlayerAbilityApplyCDMsg
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Duration = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName OwnerAbilityId;
};

