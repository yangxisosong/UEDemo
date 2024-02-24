// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/DLAnimBaseDef.h"
#include "GameplayTagsManager.h"

FDLAnimSkeletonConfig::FDLAnimSkeletonConfig()
{
	const auto Result = UGameplayTagsManager::Get().FindTagNode(TEXT("BoneName"));
	if (Result.IsValid())
	{
		const auto Children = Result.Get()->GetChildTagNodes();
		for (const auto& Tag : Children)
		{
			Bones.Emplace(Tag->GetCompleteTag(), Tag.Get()->GetSimpleTagName());
		}
	}
}

const FName FDLAnimSkeletonConfig::GetBoneByTag(const FGameplayTag Name) const
{
	if (ensureMsgf(Bones.Contains(Name), TEXT("找不到骨骼%s！！！！！！"), *Name.GetTagName().ToString()))
	{
		return *(Bones.Find(Name));
	}
	return	NAME_None;
}
