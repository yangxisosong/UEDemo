// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/EnvQueryTest_FindNearestItems.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"

#define ENVQUERYTEST_DISTANCE_NAN_DETECTION 1

namespace
{
	FORCEINLINE float CalcDistance3D(const FVector& PosA, const FVector& PosB)
	{
		return (PosB - PosA).Size();
	}

	FORCEINLINE float CalcDistance2D(const FVector& PosA, const FVector& PosB)
	{
		return (PosB - PosA).Size2D();
	}

	FORCEINLINE float CalcDistanceZ(const FVector& PosA, const FVector& PosB)
	{
		return PosB.Z - PosA.Z;
	}

	FORCEINLINE float CalcDistanceAbsoluteZ(const FVector& PosA, const FVector& PosB)
	{
		return FMath::Abs(PosB.Z - PosA.Z);
	}

	FORCEINLINE void CheckItemLocationForNaN(const FVector& ItemLocation, UObject* QueryOwner, int32 Index, uint8 TestMode)
	{
#if ENVQUERYTEST_DISTANCE_NAN_DETECTION
		ensureMsgf(!ItemLocation.ContainsNaN(), TEXT("EnvQueryTest_Distance NaN in ItemLocation with owner %s. X=%f,Y=%f,Z=%f. Index:%d, TesMode:%d"), *GetPathNameSafe(QueryOwner), ItemLocation.X, ItemLocation.Y, ItemLocation.Z, Index, TestMode);
#endif
	}

	FORCEINLINE void CheckContextLocationForNaN(const FVector& ContextLocation, UObject* QueryOwner, int32 Index, uint8 TestMode)
	{
#if ENVQUERYTEST_DISTANCE_NAN_DETECTION
		ensureMsgf(!ContextLocation.ContainsNaN(), TEXT("EnvQueryTest_Distance NaN in ContextLocations with owner %s. X=%f,Y=%f,Z=%f. Index:%d, TesMode:%d"), *GetPathNameSafe(QueryOwner), ContextLocation.X, ContextLocation.Y, ContextLocation.Z, Index, TestMode);
#endif
	}
}


UEnvQueryTest_FindNearestItems::UEnvQueryTest_FindNearestItems(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DistanceTo = UEnvQueryContext_Querier::StaticClass();
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	FilterNum = 2;
}

void UEnvQueryTest_FindNearestItems::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr)
	{
		return;
	}
	TArray<FVector> ContextLocations;
	if (!QueryInstance.PrepareContext(DistanceTo, ContextLocations))
	{
		return;
	}

	FloatValueMin.BindData(QueryOwner, QueryInstance.QueryID);
	float MinThresholdValue = FloatValueMin.GetValue();

	FloatValueMax.BindData(QueryOwner, QueryInstance.QueryID);
	float MaxThresholdValue = FloatValueMax.GetValue();

	TMap<int32,float> Scores;
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance,0); It; ++It)
	{
		const FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());
		CheckItemLocationForNaN(ItemLocation, QueryOwner, It.GetIndex(), 1);
		for (int32 ContextIndex = 0; ContextIndex < ContextLocations.Num(); ContextIndex++)
		{
			CheckContextLocationForNaN(ContextLocations[ContextIndex], QueryOwner, ContextIndex, 1);
			const float Distance = CalcDistance2D(ItemLocation, ContextLocations[ContextIndex]);
			Scores.Add(It.GetIndex(),Distance);
			It.SetScore(TestPurpose, FilterType, 1, 0, 2);
		}
	}

	

	Scores.ValueSort([]( const float A, const float B ) {return B > A;});
	TArray<int32> Index;
	Scores.GenerateKeyArray(Index);
	int32 CenterIndex = 0;
	if (Index.Num()>0)
	{
		CenterIndex = Index[0];
	}
	
	QueryInstance.CurrentTestStartingItem = 0;
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance,0); It; ++It)
	{
		int32 absindex = FMath::Abs(It.GetIndex() - CenterIndex);
		if (  absindex< FilterNum || absindex> Index.Num()-FilterNum )
		{
			It.SetScore(TestPurpose, FilterType, 1, 0, 2);
		}
		else
		{
			It.SetScore(TestPurpose, FilterType, 0, 1, 1);
		}		
	}
	
}
