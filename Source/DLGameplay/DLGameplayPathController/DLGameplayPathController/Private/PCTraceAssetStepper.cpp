// Fill out your copyright notice in the Description page of Project Settings.


#include "PCTraceAssetStepper.h"

#include "DLPathControllerComponent.h"
#include "PathControllerLibrary.h"
#include "PathControllerTraceAsset.h"
#include "Kismet/KismetMathLibrary.h"

void UPCTraceAssetStepperUnlock::OnInitialize(const FPCLaunchParam& InLaunchParam,
	const FTransform& InSubObjectInitialTransform, UDLPathControllerComponent* InPCComp, const bool bEnableDebug)
{
	Super::OnInitialize(InLaunchParam, InSubObjectInitialTransform, InPCComp, bEnableDebug);
	LastEvaluateTransform = PCComp->GetOwner()->GetTransform();

	const int32 Num = LaunchParam.Asset->GetNumberOfSplineSegments();
	for (int i = 0; i < Num; ++i)
	{
		const float LocDistance = LaunchParam.Asset->GetDistanceAlongSplineAtSplineInputKey(float(i));
		const float Marker = LaunchParam.Asset->GetInputKeyAtDistanceAlongSpline(LocDistance);
		PointMarkers.Emplace(Marker);
	}

	if (!LaunchParam.Asset->CustomSpeedCurve)
	{
		DefaultSpeed = LaunchParam.Asset->GetSplineLength() / LaunchParam.Asset->Duration;
	}

	//初始化时手动进行第一次迭代
	Advance(CheckDeltaSeconds(PCComp->GetWorld()->GetDeltaSeconds()) * LaunchParam.Asset->IterateFrequency);

}

void UPCTraceAssetStepperUnlock::OnEvaluate(const float DeltaTime, FTransform& OutTransform)
{
	const float PrevDistance = Distance;

	if (CheckIsOverDuration())
	{
		FTransform CurTransform = PCComp->GetOwner()->GetTransform();
		const float DeltaDistance = LaunchParam.Asset->SpeedOverDuration * DeltaTime;
		Distance += DeltaDistance;

		CurTransform.SetLocation(CurTransform.GetLocation()
			+ DirectionWhenOverDuration * DeltaDistance);
		OutTransform = CurTransform;
	}
	else
	{
		//迭代频率优化
		float DeltaMove;
		if (!IterateFrequencyOptimize(DeltaTime, DeltaMove))
		{
			Advance(DeltaMove);
		}

		FVector NewLoc;
		if (FMath::IsNearlyEqual(IterateTime, CurDeltaMove))
		{
			NewLoc = TargetTransform.GetLocation();
		}
		else if (IterateTime > CurDeltaMove)
		{
			const float CurTimePassed = TimePassed + DeltaTime;
			if (LaunchParam.Asset->CustomSpeedCurve)
			{
				const float Begin = FMath::Clamp(TimePassed / LaunchParam.Asset->Duration, 0.0f, 1.0f);
				const float End = FMath::Clamp(CurTimePassed / LaunchParam.Asset->Duration, 0.0f, 1.0f);
				const float Speed = UPathControllerLibrary::CalculateAverageValue(LaunchParam.Asset->CustomSpeedCurve, Begin, End);

				const float LocDistance = Distance + Speed * DeltaTime;

				const FVector Location = PCComp->GetOwner()->GetActorLocation();  /*SubObjectInitialTransform.TransformPosition(
					LaunchParam.Asset->GetLocationAtDistanceAlongSpline(Distance));*/
				const FVector NewLocation = SubObjectInitialTransform.TransformPosition(
					LaunchParam.Asset->GetLocationAtDistanceAlongSpline(LocDistance));
				const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(Location, NewLocation);

				NewLoc = Location + Direction * Speed * DeltaTime;
			}
			else
			{
				const FVector Location = LaunchParam.Asset->GetLocationAtTime(TimePassed + DeltaTime, true);
				NewLoc = SubObjectInitialTransform.TransformPosition(Location);
			}
		}
		else
		{
			NewLoc = UKismetMathLibrary::VLerp(LastTransform.GetLocation(), TargetTransform.GetLocation(), IterateTime / CurDeltaMove);
		}
		OutTransform.SetLocation(NewLoc);
		Distance += UKismetMathLibrary::Vector_Distance(OutTransform.GetLocation(), PCComp->GetOwner()->GetTransform().GetLocation());
	}
	TimePassed += DeltaTime;

	ApplyGravity(DeltaTime, OutTransform);

	LastEvaluateTransform = PCComp->GetOwner()->GetTransform();

	//处理关键帧事件派发
	while (PointMarkers.Num() > CurMarkIndex)
	{
		const float& CurDistanceMarker = PointMarkers[CurMarkIndex];
		if (CurDistanceMarker >= PrevDistance && CurDistanceMarker <= Distance)
		{
			PCComp->OnKeyPoint(LaunchParam.Asset, CurMarkIndex);
			CurMarkIndex++;
		}
		else
		{
			break;
		}
	}
}

void UPCTraceAssetStepperUnlock::OnPostEvaluate(const float DeltaTime)
{
	if (TimePassed >= LaunchParam.PersistTime)
	{
		DL_PATH_CONTROLLER_LOG_WITHOUT_VALUE(Log, TEXT("<UPCTraceAssetStepperUnlock::OnPostEvaluate> TimePassed >= LaunchParam.PersistTime"));
		PCComp->TrackComplete(LaunchParam.Asset);
	}
}

void UPCTraceAssetStepperUnlock::Advance(const float DeltaMoveTime)
{
	Super::Advance(DeltaMoveTime);

	float Speed = DefaultSpeed;
	FTransform NewTrans;
	if (LaunchParam.Asset->CustomSpeedCurve)
	{
		const float Begin = FMath::Clamp(TimePassed / LaunchParam.Asset->Duration, 0.0f, 1.0f);
		const float End = FMath::Clamp((TimePassed + DeltaMoveTime) / LaunchParam.Asset->Duration, 0.0f, 1.0f);
		Speed = UPathControllerLibrary::CalculateAverageValue(LaunchParam.Asset->CustomSpeedCurve, Begin, End);

		const float LocDistance = Distance + Speed * DeltaMoveTime;

		const FVector Location = PCComp->GetOwner()->GetActorLocation();  /*SubObjectInitialTransform.TransformPosition(
			LaunchParam.Asset->GetLocationAtDistanceAlongSpline(Distance));*/
		const FVector NewLocation = SubObjectInitialTransform.TransformPosition(
			LaunchParam.Asset->GetLocationAtDistanceAlongSpline(LocDistance));
		const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(Location, NewLocation);

		NewTrans.SetLocation(Location + Direction * Speed * DeltaMoveTime);
	}
	else
	{
		const FVector Location = LaunchParam.Asset->GetLocationAtTime(TimePassed + DeltaMoveTime, true);
		DL_PATH_CONTROLLER_LOG(Log, TEXT("<UPCTraceAssetStepperUnlock::Advance> Duration:%f,TimePassed:%f,DeltaMoveTime:%f")
			, LaunchParam.Asset->Duration, TimePassed, DeltaMoveTime);
		NewTrans.SetLocation(SubObjectInitialTransform.TransformPosition(Location));
	}


	TargetTransform = NewTrans;

	DL_PATH_CONTROLLER_LOG(Log, TEXT("TargetTransform:%s")
			, *TargetTransform.GetLocation().ToString());

	LastTransform = PCComp->GetOwner()->GetTransform();
#if WITH_EDITOR
	const float ActualDistance = UKismetMathLibrary::Vector_Distance(LastTransform.GetLocation(), TargetTransform.GetLocation());
	DL_PATH_CONTROLLER_LOG(Log, TEXT("根据速度得出的距离:%f,实际Advance的距离：%f,TargetTransform:%s,LastTransform:%s")
		, Speed * DeltaMoveTime, ActualDistance, *TargetTransform.GetLocation().ToString(), *LastTransform.GetLocation().ToString());
#endif
}

bool UPCTraceAssetStepperUnlock::CheckIsOverDuration()
{
	if (!bIsOverDuration)
	{
		//时间和路程都要检测
		bIsOverDuration = (LaunchParam.Asset->Duration <= TimePassed) || LaunchParam.Asset->GetSplineLength() <= Distance;
		if (bIsOverDuration)
		{
			DirectionWhenOverDuration = UKismetMathLibrary::GetDirectionUnitVector(LastEvaluateTransform.GetLocation(), PCComp->GetOwner()->GetActorLocation());
		}
	}
	return bIsOverDuration;
}

void UPCTraceAssetStepperToLocation::OnInitialize(const FPCLaunchParam& InLaunchParam,
	const FTransform& InSubObjectInitialTransform, UDLPathControllerComponent* InPCComp, const bool bEnableDebug)
{
	Super::OnInitialize(InLaunchParam, InSubObjectInitialTransform, InPCComp, bEnableDebug);
	UPathControllerTraceAsset* AssetCopy = UPathControllerTraceAsset::CopyAsset(LaunchParam.Asset);
	LaunchParam.Asset = AssetCopy;

	if (!LaunchParam.Asset->CustomSpeedCurve)
	{
		DefaultSpeed = LaunchParam.Asset->GetSplineLength() / LaunchParam.Asset->Duration;
	}

	//找到最近的点，然后修改至目标位置,并删除后续的点
	float Key = LaunchParam.Asset->FindInputKeyClosestToWorldLocation(LaunchParam.TargetLocation, SubObjectInitialTransform);
	if (Key == 0)
	{
		LaunchParam.Asset->AddSplinePointAtIndex(LaunchParam.Asset->GetLocationAtDistanceAlongSpline(50.0f), 1, false);
		Key = 2;
	}
	const int32 PointIndex = FMath::CeilToInt(Key);

	for (int i = PointIndex + 1; i < LaunchParam.Asset->GetNumberOfSplinePoints(); ++i)
	{
		LaunchParam.Asset->RemoveSplinePoint(i, false);
	}

	LaunchParam.Asset->SetLocationAtSplinePoint(PointIndex, UKismetMathLibrary::InverseTransformLocation(SubObjectInitialTransform, LaunchParam.TargetLocation));

	const int32 Num = LaunchParam.Asset->GetNumberOfSplinePoints();

	for (int i = 0; i < Num; ++i)
	{
		const float LocDistance = LaunchParam.Asset->GetDistanceAlongSplineAtSplineInputKey(float(i));
		PointMarkers.Emplace(LocDistance);
	}

	//初始化时手动进行第一次迭代
	Advance(CheckDeltaSeconds(PCComp->GetWorld()->GetDeltaSeconds()) * LaunchParam.Asset->IterateFrequency);
}

void UPCTraceAssetStepperToLocation::OnPostEvaluate(const float DeltaTime)
{
	//分步骤判断对性能更加友好,因为这里有些计算比较耗

	if (TimePassed >= LaunchParam.PersistTime)
	{
		FinishTrace();
		return;
	}

	//需要判断是否已到达目标位置
	const FVector& CurLocation = PCComp->GetOwner()->GetActorLocation();
	if (CurLocation.Equals(LaunchParam.TargetLocation, TARGET_ARRIVE_BIAS))
	{
		FinishTrace();
		return;
	}

	// const float MaxDistance = LaunchParam.Asset->GetSplineDisplacementSquared();
	// const float CurDistance = FVector::DistSquared(CurLocation, SubObjectInitialTransform.GetLocation());
	// if (CurDistance >= MaxDistance - FMath::Square(TARGET_ARRIVE_BIAS))
	// {
	// 	FinishTrace();
	// 	return;
	// }
}


