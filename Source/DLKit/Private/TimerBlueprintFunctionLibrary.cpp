// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerBlueprintFunctionLibrary.h"

#include "DelayAction.h"

void UTimerBlueprintFunctionLibrary::DelayOneFrame(const UObject* WorldContextObject, FLatentActionInfo LatentInfo )
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		if (LatentActionManager.FindExistingAction<FDelayAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == NULL)
		{
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, new FDelayAction(0, LatentInfo));
		}
	}
}
