// Fill out your copyright notice in the Description page of Project Settings.


#include "DLSequencerSubSystem.h"

#include "DefaultLevelSequenceInstanceData.h"
#include "LevelSequencePlayer.h"
#include "Chaos/PBDSuspensionConstraintData.h"
#include "LevelSequence/Public/LevelSequenceActor.h"

ULevelSequencePlayer* UDLSequencerSubSystem::CreateSequence(const FDLSequencerPlayParams& PlayParams, float& SequenceLength)
{
	if (!PlayParams.Sequence)
	{
		ensureMsgf(false, TEXT("Sequence 为空！！"));
		SequenceLength = 0.0f;
		return nullptr;
	}

	if (GetCurLevelSequencePlayer())
	{
		GetCurLevelSequencePlayer()->Stop();
		RemoveDelegates(GetCurLevelSequencePlayer());
		LevelSequenceActor = nullptr;
		LevelSequencePlayer = nullptr;
	}

	ALevelSequenceActor* LocLevelSequenceActor = nullptr;
	ULevelSequencePlayer* LocLevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(), PlayParams.Sequence, PlayParams.PlaybackSettings, LocLevelSequenceActor);

	if (!LocLevelSequenceActor || !LocLevelSequencePlayer)
	{
		return nullptr;
	}

	LevelSequenceActor = TWeakObjectPtr<ALevelSequenceActor>(LocLevelSequenceActor);
	LevelSequencePlayer = TWeakObjectPtr<ULevelSequencePlayer>(LocLevelSequencePlayer);

	if (PlayParams.OverrideParams.Override)
	{
		LevelSequenceActor->bOverrideInstanceData = true;
		UDefaultLevelSequenceInstanceData* InstanceObj = Cast<UDefaultLevelSequenceInstanceData>(LevelSequenceActor->DefaultInstanceData);
		InstanceObj->TransformOrigin.SetLocation(PlayParams.OverrideParams.WorldLocation);
		InstanceObj->TransformOrigin.SetRotation(FQuat::MakeFromEuler(PlayParams.OverrideParams.Rotation.Vector()));
	}

	AddDelegates(LocLevelSequencePlayer);
	SequenceLength = LevelSequencePlayer->GetDuration().AsSeconds();
	return LocLevelSequencePlayer;
}

bool UDLSequencerSubSystem::IsPlaying()
{
	if (GetCurLevelSequencePlayer())
	{
		return GetCurLevelSequencePlayer()->IsPlaying();
	}
	return false;
}

bool UDLSequencerSubSystem::IsPaused()
{
	if (GetCurLevelSequencePlayer())
	{
		return GetCurLevelSequencePlayer()->IsPaused();
	}
	return false;
}

bool UDLSequencerSubSystem::Play()
{
	if (GetCurLevelSequencePlayer())
	{
		GetCurLevelSequencePlayer()->Play();
		return true;
	}
	return false;
}

bool UDLSequencerSubSystem::Pause()
{
	if (GetCurLevelSequencePlayer())
	{
		GetCurLevelSequencePlayer()->Pause();
		return true;
	}
	return false;
}

bool UDLSequencerSubSystem::Stop()
{
	if (GetCurLevelSequencePlayer())
	{
		GetCurLevelSequencePlayer()->Stop();
		return true;
	}
	return false;
}

void UDLSequencerSubSystem::TriggerPlay()
{
	OnPlay.Broadcast();
}

void UDLSequencerSubSystem::TriggerPlayReverse()
{
	OnPlayReverse.Broadcast();
}

void UDLSequencerSubSystem::TriggerFinished()
{
	OnFinished.Broadcast();
}

void UDLSequencerSubSystem::TriggerStop()
{
	OnStop.Broadcast();
}

void UDLSequencerSubSystem::TriggerPause()
{
	OnPause.Broadcast();
}

void UDLSequencerSubSystem::TriggerCameraCut(UCameraComponent* CameraComponent)
{
	OnCameraCut.Broadcast(CameraComponent);
}

void UDLSequencerSubSystem::AddDelegates(ULevelSequencePlayer* InLevelSequencePlayer)
{
	InLevelSequencePlayer->OnPlay.AddDynamic(this, &UDLSequencerSubSystem::TriggerPlay);
	InLevelSequencePlayer->OnPlayReverse.AddDynamic(this, &UDLSequencerSubSystem::TriggerPlayReverse);
	InLevelSequencePlayer->OnFinished.AddDynamic(this, &UDLSequencerSubSystem::TriggerFinished);
	InLevelSequencePlayer->OnStop.AddDynamic(this, &UDLSequencerSubSystem::TriggerStop);
	InLevelSequencePlayer->OnPause.AddDynamic(this, &UDLSequencerSubSystem::TriggerPause);
	InLevelSequencePlayer->OnCameraCut.AddDynamic(this, &UDLSequencerSubSystem::TriggerCameraCut);
}

void UDLSequencerSubSystem::RemoveDelegates(ULevelSequencePlayer* InLevelSequencePlayer)
{
	InLevelSequencePlayer->OnPlay.RemoveDynamic(this, &UDLSequencerSubSystem::TriggerPlay);
	InLevelSequencePlayer->OnPlayReverse.RemoveDynamic(this, &UDLSequencerSubSystem::TriggerPlayReverse);
	InLevelSequencePlayer->OnFinished.RemoveDynamic(this, &UDLSequencerSubSystem::TriggerFinished);
	InLevelSequencePlayer->OnStop.RemoveDynamic(this, &UDLSequencerSubSystem::TriggerStop);
	InLevelSequencePlayer->OnPause.RemoveDynamic(this, &UDLSequencerSubSystem::TriggerPause);
	InLevelSequencePlayer->OnCameraCut.RemoveDynamic(this, &UDLSequencerSubSystem::TriggerCameraCut);
}
