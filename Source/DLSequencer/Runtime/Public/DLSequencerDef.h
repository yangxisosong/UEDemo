// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelSequence.h"
#include "MovieSceneSequencePlayer.h"
#include "DLSequencerDef.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDL_Sequencer, Log, All);

USTRUCT(BlueprintType)
struct FDLSequencerPlayOverrideParams
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Override = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector WorldLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator Rotation = FRotator::ZeroRotator;
};


USTRUCT(BlueprintType)
struct FDLSequencerPlayParams
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ULevelSequence* Sequence = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMovieSceneSequencePlaybackSettings PlaybackSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDLSequencerPlayOverrideParams OverrideParams;
};
