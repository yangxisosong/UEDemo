// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/BTComposite_Probability.h"



UBTComposite_Probability::UBTComposite_Probability(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Probability";
}

int32 UBTComposite_Probability::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
	int32 NextChildIdx = BTSpecialChild::ReturnToParent;
	if (PrevChild == BTSpecialChild::NotInitialized)
	{
		//求随机数最大值
		int32 Sum = 0;
		int32 Num = GetChildrenNum();
		for (int32 i = 0;i<Num;i++)
		{
			if (Probabilitys.Num()>i)
			{
				Sum+=FMath::Abs(Probabilitys[i]);
			}
		}

		//判断随机数落在哪个区间，获得对应的index
		int32 RandVal = FMath::RandRange(0,Sum-1);
		int32 CurrentSum = 0;
		for (int32 i = 0;i<Num;i++)
		{
			if (Probabilitys.Num()>i)
			{
				CurrentSum+=FMath::Abs(Probabilitys[i]);
				if (CurrentSum > RandVal)
				{
					NextChildIdx = i;
					break;
				}
			}
		}
	
	}
	return  NextChildIdx;
}

#if WITH_EDITOR

FName UBTComposite_Probability::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Composite.Selector.Icon");
}

#endif