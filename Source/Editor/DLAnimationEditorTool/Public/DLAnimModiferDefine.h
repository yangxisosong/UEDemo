// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLAnimModiferDefine.generated.h"

USTRUCT(BlueprintType)
struct FAnimCurveCreationData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 FrameNumber = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CurveValue = 0.0f;
};

USTRUCT(BlueprintType)
struct FAnimCurveCreationParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName CurveName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool KeyEachFrame = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FAnimCurveCreationData> Keys;
};
