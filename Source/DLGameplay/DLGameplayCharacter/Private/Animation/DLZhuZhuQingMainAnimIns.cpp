// Fill out your copyright notice in the Description page of Project Settings.


#include "DLZhuZhuQingMainAnimIns.h"

void UDLZhuZhuQingMainAnimIns::OnUpdateAnimation(float DeltaSeconds)
{
	if (FMath::IsNearlyZero(DeltaSeconds))
	{
		DL_ANIM_LOG_WITHOUT_VALUE(Warning, TEXT("<UDLZhuZhuQingMainAnimIns::OnUpdateAnimation> Delta seconds is zero"));
		return;
	}
}
