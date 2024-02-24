// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/DLLinkedAnimInstanceV2.h"

#include "Animation/DLMainAnimInstanceV2.h"

bool UDLLinkedAnimInstanceV2::OnPreInit(const FDLAnimInsInitParams& InitParams)
{
	return ensureMsgf(GetMainAnimInstance(), TEXT("为啥主蓝图是空的！！！！"));
}

void UDLLinkedAnimInstanceV2::OnUpdateAnimation(float DeltaSeconds)
{

}

UDLMainAnimInstanceV2* UDLLinkedAnimInstanceV2::GetMainAnimInstance() const
{
	UDLMainAnimInstanceV2* MainAnimIns = Cast<UDLMainAnimInstanceV2>(GetSkelMeshComponent()->GetAnimInstance());
	ensureMsgf(MainAnimIns, TEXT("没有主动画实例！！！"));
	return MainAnimIns;
}

FAnimCharacterInfoBase& UDLLinkedAnimInstanceV2::GetPrevAnimCharacterInfoRef()
{
	return GetMainAnimInstance()->GetPrevAnimCharacterInfoRef();
}

void UDLLinkedAnimInstanceV2::CopyPrevAnimCharacterInfo(FAnimCharacterInfoBase& NewInfo)
{

}

FAnimConfigBase& UDLLinkedAnimInstanceV2::GetAnimConfigRef()
{
	return GetMainAnimInstance()->GetAnimConfigRef();
}
