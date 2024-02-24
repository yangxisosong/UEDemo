// Fill out your copyright notice in the Description page of Project Settings.


#include "DLCommonBlueprintFunctionLibrary.h"
#include "CoreGlobals.h"
#include "Kismet/KismetMathLibrary.h"

bool UDLCommonBlueprintFunctionLibrary::K2_EnsureMsg(const bool bIsValid, const FString Category, const FString Msg, bool bIsAlways)
{
	const FString& LogStr = FString::Printf(TEXT("<%s> EnsureMsg:%s"), *Category, *Msg);

	if (bIsAlways)
	{
		return ensureAlwaysMsgf(bIsValid, TEXT("%s"), *LogStr);
	}

	return ensureMsgf(bIsValid, TEXT("%s"), *LogStr);
}

bool UDLCommonBlueprintFunctionLibrary::K2_EnsureObjectMsg(const UObject* TargetObject, const FString Category,
	const FString Msg, bool bIsAlways)
{
	return K2_EnsureMsg(TargetObject && IsValid(TargetObject), Category, Msg, bIsAlways);
}

static float Product(const FVector2D& Point1, const FVector2D& Point2, const FVector2D& Point3)
{
	return FVector2D::CrossProduct(Point2 - Point1, Point3 - Point1);
}

bool UDLCommonBlueprintFunctionLibrary::K2_CheckIsPointInTriangle(const FVector2D& TargetPoint, const FVector2D& Point1,
	const FVector2D& Point2, const FVector2D& Point3)
{
	if (Product(Point1, Point2, Point3) < 0.0f)
	{
		//保证p1，p2，p3是逆时针顺序
		return K2_CheckIsPointInTriangle(TargetPoint, Point1, Point3, Point2);
	}

	if (Product(Point1, Point2, TargetPoint) > 0.0f
	   && Product(Point2, Point3, TargetPoint) > 0.0f
	   && Product(Point3, Point1, TargetPoint) > 0.0f
	   )
	{
		return true;
	}
	return false;
}

bool UDLCommonBlueprintFunctionLibrary::K2_CheckIsPointInTriangle_Vector(const FVector& TargetPoint,
	const FVector& Point1, const FVector& Point2, const FVector& Point3)
{
	FVector v0 = Point3 - Point1;
	FVector v1 = Point2 - Point1;
	FVector v2 = TargetPoint - Point1;

	float dot00 = FVector::DotProduct(v0, v0);
	float dot01 = FVector::DotProduct(v0, v1);
	float dot02 = FVector::DotProduct(v0, v2);
	float dot11 = FVector::DotProduct(v1, v1);
	float dot12 = FVector::DotProduct(v1, v2);

	float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01);

	float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
	if (u < 0 || u > 1) // if u out of range, return directly
	{
		return false;
	}

	float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
	if (v < 0 || v > 1) // if v out of range, return directly
	{
		return false;
	}

	return u + v <= 1;
}


void UDLCommonBlueprintFunctionLibrary::K2_PointsCrossCircle(const FVector& LineStart, const FVector& LineEnd,
	const FVector& Center, const float& Radius, FVector& Point1, FVector& Point2)
{
	Point1 = FVector::ZeroVector;
	Point2 = FVector::ZeroVector;


	if (FMath::IsNearlyZero(Radius))
	{
		Point1 = Center;
		Point2 = Center;
		return;
	}

	if (LineStart.Equals(LineEnd))
	{
		Point1 = LineStart;
		Point2 = LineEnd;
		return;
	}

	const FVector& ClosestPoint = UKismetMathLibrary::FindClosestPointOnSegment(Center, LineStart, LineEnd);

	const float Distance = FVector::Dist(Center, ClosestPoint);

	const FVector& Dir = UKismetMathLibrary::GetDirectionUnitVector(LineStart, LineEnd);

	if (FMath::IsNearlyZero(Distance))
	{
		Point1 = Center + Dir * Radius;
		Point2 = Center + (-Dir) * Radius;
		return;
	}
	const float CosB = Distance / Radius;
	const float AngleB = FMath::Acos(CosB);

	const float DisB = Distance * FMath::Tan(AngleB);

	Point1 = ClosestPoint + Dir * DisB;
	Point2 = ClosestPoint + (-Dir) * DisB;
}

FString UDLCommonBlueprintFunctionLibrary::GetLogTimeInfo()
{
	FString Ret;
	switch (GPrintLogTimes)
	{
	case ELogTimes::UTC:
		Ret = FString::Printf(TEXT("[%s][%3llu]"), *FDateTime::UtcNow().ToString(TEXT("%Y.%m.%d-%H.%M.%S:%s")), GFrameCounter % 1000);
		break;

	case ELogTimes::Local:
		Ret = FString::Printf(TEXT("[%s][%3llu]"), *FDateTime::Now().ToString(TEXT("%Y.%m.%d-%H.%M.%S:%s")), GFrameCounter % 1000);
		break;
	default:;
	};
	return Ret;
}

bool UDLCommonBlueprintFunctionLibrary::GetIsDedicatedServer(UWorld* InWorld)
{
	return InWorld->GetNetMode() == ENetMode::NM_DedicatedServer;
}

TArray<UClass*> UDLCommonBlueprintFunctionLibrary::GetAllClass(TArray<AActor*> Actors)
{
	TArray<UClass*> OutClass;
	for (const auto Actor : Actors)
	{
		OutClass.AddUnique(Actor->GetClass());
	}
	return OutClass;
}

TArray<FString> UDLCommonBlueprintFunctionLibrary::GetObjectProperty(UClass* InClass)
{
	TArray<FString> OutProperty;
	for (TFieldIterator<FProperty> i(InClass); i; ++i)
	{
		FProperty* Prop = *i;
		OutProperty.Add(Prop->GetName());
	}
	return OutProperty;
}

TArray<FString> UDLCommonBlueprintFunctionLibrary::GetObjectFunction(UClass* InClass)
{
	TArray<FString> OutProperty;
	for (TFieldIterator<UFunction> i(InClass); i; ++i)
	{
		UFunction* Func = *i;
		OutProperty.Add(Func->GetName());
	}
	return OutProperty;
}

void UDLCommonBlueprintFunctionLibrary::GetTipsArray(FString InTokens, TMap<FString, FString>& OutNames, FString& OutPrevToken, FString& OutLastToken)
{
	OutPrevToken = "";
	OutLastToken = "";
	OutNames.Empty();

	TArray<UClass*> AllActor;
	GetDerivedClasses(AActor::StaticClass(), AllActor);

	TArray<FString> SplitedTokens;
	FString CacheToken = InTokens;
	while (CacheToken.Contains("."))
	{
		FString Left, right;
		CacheToken.Split(".", &Left, &right);
		SplitedTokens.Add(Left);
		OutPrevToken.Append(Left);
		OutPrevToken.Append(".");
		CacheToken = right;
	}
	OutLastToken = CacheToken;


	TArray<FString> OutString;




	if (SplitedTokens.Num() > 0)
	{
		FString CurrentToken;
		UClass* CacheCls = nullptr;
		SplitedTokens.HeapPop(CurrentToken);


		for (auto Cls : AllActor)
		{
			if (Cls->GetName() == CurrentToken)
			{
				CacheCls = Cls;
			}
		}

		// while (SplitedTokens.Num()>0)
		// {
		// 	SplitedTokens.HeapPop(CurrentToken);
		// 	for (TFieldIterator<FProperty> i(CacheCls); i; ++i)
		// 	{
		// 		FProperty* Prop=*i;
		// 		if (CurrentToken == Prop->GetName())
		// 		{
		// 		}
		// 	}
		// }



		if (CacheCls)
		{
			//用最近的 Object 查属性
			for (TFieldIterator<FProperty> i(CacheCls); i; ++i)
			{
				FProperty* Prop = *i;
				OutNames.Add(Prop->GetName(), Prop->GetClass()->GetName());

			}
		}

	}
	else
	{
		for (auto Cls : AllActor)
		{
			FString Name = Cls->GetName();
			if (Name.StartsWith("REINST_")) continue;
			if (Name.StartsWith("SKEL_")) continue;

			OutNames.Add(Name, Cls->GetClass()->GetName());
		}
	}

}

FRotator UDLCommonBlueprintFunctionLibrary::VectorToRotator(const FVector& TargetVector)
{
	return TargetVector.Rotation();
}

FRotator UDLCommonBlueprintFunctionLibrary::AddRotator(const FRotator& RotatorA, const FRotator& RotatorB)
{
	FRotator Total = (RotatorA + RotatorB);
	Total.Normalize();
	return Total;
}

bool UDLCommonBlueprintFunctionLibrary::IsPreview()
{
#if WITH_EDITOR
	if (IsRunningCommandlet() || IsRunningGame())
	{
		return false;
	}
	return true;
#else
	return false;
#endif
}
