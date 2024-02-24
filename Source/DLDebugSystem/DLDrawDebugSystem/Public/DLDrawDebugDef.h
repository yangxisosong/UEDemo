// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLDrawDebugDef.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDL_DrawDebugSystem, Log, All);

UENUM(BlueprintType)
enum class EDrawDebugCategory : uint8
{
	Temp,
};
