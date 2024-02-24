// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLSequencerDef.h"
#include "LevelSequencePlayer.h"
#include "DLSequencerSubSystem.generated.h"

class UCameraComponent;
class ULevelSequencePlayer;
class ALevelSequenceActor;
class ULevelSequence;
/**
 *
 */
UCLASS(BlueprintType)
class DLSEQUENCERRUNTIME_API UDLSequencerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		ULevelSequencePlayer* CreateSequence(const FDLSequencerPlayParams& PlayParams, float& SequenceLength);

	/** Check whether the sequence is actively playing. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sequencer|Player")
		bool IsPlaying();

	/** Check whether the sequence is paused. */
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Sequencer|Player")
		bool IsPaused();

	/** Start playback forwards from the current time cursor position, using the current play rate. */
	UFUNCTION(BlueprintCallable, Category = "Sequencer|Player")
		bool Play();

	/** Pause playback. */
	UFUNCTION(BlueprintCallable, Category = "Sequencer|Player")
		bool Pause();

	/** Stop playback and move the cursor to the end (or start, for reversed playback) of the sequence. */
	UFUNCTION(BlueprintCallable, Category = "Sequencer|Player")
		bool Stop();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE ALevelSequenceActor* GetCurLevelSequenceActor() { return LevelSequenceActor.Get(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE ULevelSequencePlayer* GetCurLevelSequencePlayer() { return LevelSequencePlayer.Get(); }
public:
	/** Event triggered when the level sequence player is played */
	UPROPERTY(BlueprintAssignable, Category = "Sequencer|Player")
		FOnMovieSceneSequencePlayerEvent OnPlay;

	/** Event triggered when the level sequence player is played in reverse */
	UPROPERTY(BlueprintAssignable, Category = "Sequencer|Player")
		FOnMovieSceneSequencePlayerEvent OnPlayReverse;

	/** Event triggered when the level sequence player is stopped */
	UPROPERTY(BlueprintAssignable, Category = "Sequencer|Player")
		FOnMovieSceneSequencePlayerEvent OnStop;

	/** Event triggered when the level sequence player is paused */
	UPROPERTY(BlueprintAssignable, Category = "Sequencer|Player")
		FOnMovieSceneSequencePlayerEvent OnPause;

	/** Event triggered when the level sequence player finishes naturally (without explicitly calling stop) */
	UPROPERTY(BlueprintAssignable, Category = "Sequencer|Player")
		FOnMovieSceneSequencePlayerEvent OnFinished;

	/** Event triggered when there is a camera cut */
	UPROPERTY(BlueprintAssignable, Category = "Sequencer|Player")
		FOnLevelSequencePlayerCameraCutEvent OnCameraCut;

private:
	UFUNCTION()
		void TriggerPlay();

	UFUNCTION()
		void TriggerPlayReverse();

	UFUNCTION()
		void TriggerFinished();

	UFUNCTION()
		void TriggerStop();

	UFUNCTION()
		void TriggerPause();

	UFUNCTION()
		void TriggerCameraCut(UCameraComponent* CameraComponent);
private:
	void AddDelegates(ULevelSequencePlayer* InLevelSequencePlayer);

	void RemoveDelegates(ULevelSequencePlayer* InLevelSequencePlayer);
private:

	UPROPERTY()
		TWeakObjectPtr<ALevelSequenceActor> LevelSequenceActor = nullptr;

	UPROPERTY()
		TWeakObjectPtr<ULevelSequencePlayer> LevelSequencePlayer = nullptr;
};
