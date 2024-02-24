// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BTComposite_Probability.generated.h"

/**
 * 
 */
UCLASS()
class UBTComposite_Probability : public UBTCompositeNode
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Category=Description, EditAnywhere)
	TArray<int32> Probabilitys;
	
	virtual int32 GetNextChildHandler(struct FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif
};
