// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/DLMainAnimInstanceV2.h"

#include "Animation/DLLinkedAnimInstanceV2.h"

bool UDLMainAnimInstanceV2::OnPreInit(const FDLAnimInsInitParams& InitParams)
{
	Super::OnPreInit(InitParams);
	//在这里先初始化链接动画实例
	InitLinkedAnimInstance(InitParams);
	return true;
}

void UDLMainAnimInstanceV2::InitLinkedAnimInstance(const FDLAnimInsInitParams& InitParams) const
{
	const TArray<UAnimInstance*>& LinkedAnimIns = const_cast<const USkeletalMeshComponent*>(GetSkelMeshComponent())->GetLinkedAnimInstances();
	for (const auto Ins : LinkedAnimIns)
	{
		UDLLinkedAnimInstanceV2* LinkIns = Cast<UDLLinkedAnimInstanceV2>(Ins);
		if (LinkIns)
		{
			LinkIns->InitAnimIns(InitParams);
		}
	}
}
