// ReSharper disable CppParameterMayBeConst

#include "TestAlgoLib.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

static TArray<FVector> LinesHitTest(const UObject* WorldContext,
                                    FVector OriginLocation,
                                    FVector Orientation,
                                    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType,
                                    float Distance,
                                    int32 IterationCount,
                                    float IterationInterval)
{
	const FVector TargetVec = Orientation * (Distance + 100);

	TArray<FVector> HitPointList; 

	FVector StartPotion = OriginLocation;
	FVector EndPotion = TargetVec + OriginLocation;

	for (int32 i = 0; i <= IterationCount; i++)
	{
		FHitResult Ret;

		const bool HasHit = UKismetSystemLibrary::LineTraceSingleForObjects(
			WorldContext,
			StartPotion,
			EndPotion,
			ObjectType,
			false,
			{},
			EDrawDebugTrace::Type::ForDuration,
			Ret,
			true);

		UE_LOG(LogTemp, Log, TEXT("Line Trace  Start : %s, End : %s HasHit : %d"), *StartPotion.ToString(), *EndPotion.ToString(), HasHit);

		if (HasHit)
		{
			HitPointList.Add(Ret.ImpactPoint);
		}
		else
		{
			break;
		}

		EndPotion.Z += IterationInterval;
		StartPotion.Z = EndPotion.Z;
	}

	return HitPointList;
}


void UTestAlgoLib::ProbeForwardLandforms(UObject* WorldContext, FVector OriginLocation, FVector Orientation, TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType, float Distance, int32 IterationCount,
										 float IterationInterval, FVector& RetPoint, bool& Sweep)
{
	const FVector TargetVec = Orientation * Distance;

	TArray<FVector> HitPointList;

	HitPointList = LinesHitTest(WorldContext, OriginLocation, Orientation, ObjectType, Distance, IterationCount, IterationInterval);


	// 没有打到任何物体，说明在平地上
	if (HitPointList.Num() ==0 )
	{
		RetPoint = TargetVec + OriginLocation + FVector(0,0, 100);
		Sweep = true;
		UE_LOG(LogTemp, Log, TEXT("在平地上的情况  Sweep : %d"), Sweep);
	}
	else
	{
		bool IsOk = false;

		if (HitPointList.Num() > 1)
		{
			float FirstX = HitPointList[0].X;
			for (int32 index = 1; index < HitPointList.Num(); index ++)
			{
				float Adjacent = HitPointList[index].X - FirstX;
				float Tan = IterationInterval / FMath::Abs(Adjacent);
				if (Tan > 1.5f)
				{
					UE_LOG(LogTemp, Log, TEXT("Has Hit tan(%f) > 1.5, Index : %d"), Tan, index);

					const auto& HitPoint = HitPointList[index];
					if (HitPoint.Z > OriginLocation.Z + 100)
					{
						auto TempPotion = HitPointList[FMath::Clamp(index-1, 0, HitPointList.Num())];
						
						UE_LOG(LogTemp, Log, TEXT("Hit Break"));

						FHitResult Ret;

						const bool HasHit = UKismetSystemLibrary::LineTraceSingleForObjects(
							WorldContext,
							OriginLocation + FVector(0,0, TempPotion.Z),
							TargetVec + (OriginLocation + FVector(0,0, TempPotion.Z)),
							ObjectType,
							false,
							{},
							EDrawDebugTrace::Type::ForDuration,
							Ret,
							true, FLinearColor::Yellow);


						if (HasHit)
						{
							RetPoint = Ret.ImpactPoint;
							UE_LOG(LogTemp, Log, TEXT("二次检测，有 Hit "));
						}
						else
						{
							RetPoint = TargetVec + OriginLocation + FVector(0,0, TempPotion.Z);
							UE_LOG(LogTemp, Log, TEXT("二次检测，无 Hit "));
						}

						Sweep = true;

						IsOk = true;
						break;
					}
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Has Hit tan(%f) <= 1.5, Index : %d"), Tan, index);
				}
				FirstX = HitPointList[index].X;
			}
		}

		if (!IsOk)
		{
			FVector Potion = TargetVec + OriginLocation;

			FVector PotionEnd = Potion;
			PotionEnd.Z += Distance * 1; // tan(45) * Distance

			FHitResult Ret;

			const bool HasHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
				WorldContext,
				PotionEnd,
				Potion,
				50.f,
				ObjectType,
				false,
				{},
				EDrawDebugTrace::Type::ForDuration,Ret, true);

			UE_LOG(LogTemp, Log, TEXT("SphereTrace  Start : %s, End : %s HasHit : %d"), *Potion.ToString(), *PotionEnd.ToString(), HasHit);

			if (HasHit)
			{
				RetPoint = Ret.ImpactPoint+ FVector(0,0, 100);
				Sweep = false;
			}
			else
			{
				RetPoint = TargetVec + OriginLocation + FVector(0,0, 100);
				Sweep = true;
			}
		}
	}
}

TTuple<bool, float> UTestAlgoLib::CalculateActorInFront(AActor* Origin, AActor* Target, float FontAngle)
{
	if (!(IsValid(Origin) && IsValid(Target)))
	{
		return MakeTuple(false, 0.f);
	}

	const auto OriginLocation = Origin->GetActorLocation();
	const auto TargetLocation = Target->GetActorLocation();

	const auto Rotation = UKismetMathLibrary::FindLookAtRotation(OriginLocation, TargetLocation);
	const auto DtRotation = UKismetMathLibrary::NormalizedDeltaRotator(Rotation, Origin->GetActorRotation());

	FontAngle = UKismetMathLibrary::Abs(FontAngle);

	if (DtRotation.Yaw >-FontAngle && DtRotation.Yaw < FontAngle)
	{
		return MakeTuple(true, DtRotation.Yaw);
	}

	return MakeTuple(false, DtRotation.Yaw);
}

void UTestAlgoLib::CalculateActorInFront_BP(AActor* Origin, AActor* Target, float FontAngle, bool& InFront,
	float& Angle)
{
	auto Ret = CalculateActorInFront(Origin, Target, FontAngle);
	InFront = Ret.Get<0>();
	Angle = Ret.Get<1>();
}

