// Fill out your copyright notice in the Description page of Project Settings.


#include "PCTraceStepperLockV4.h"

#include "DLCommonBlueprintFunctionLibrary.h"
#include "DLPathControllerComponent.h"
#include "PathControllerLibrary.h"
#include "PathControllerTraceAsset.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"

void DrawRawDebugLine(const FPCLaunchParam& InLaunchParam,
	const FTransform& InSubObjectInitialTransform, UDLPathControllerComponent* InPCComp, const bool bEnableDebug)
{
	if (!ensure(InLaunchParam.Asset && InPCComp))
	{
		return;
	}

	if (!bEnableDebug)
	{
		return;
	}

	const UWorld* World = InPCComp->GetWorld();
	if (!ensure(World))
	{
		return;
	}

	float Time = 0.0f;
	while (Time <= InLaunchParam.Asset->Duration)
	{
		::DrawDebugLine(World, InSubObjectInitialTransform.TransformPosition(InLaunchParam.Asset->GetLocationAtTime(Time))
			, InSubObjectInitialTransform.TransformPosition(InLaunchParam.Asset->GetLocationAtTime(Time + 0.08f))
			, FColor::Yellow, false, InLaunchParam.PersistTime + 20.f);
		Time += 0.08;
	}
}
#endif

static float IsLeft(const FVector2D& Point1, const FVector2D& Point2, const FVector2D& TargetPoint)
{
	const FVector2D& V1 = Point1 - Point2;
	const FVector2D& V2 = TargetPoint - Point1;
	const float Result = FVector2D::CrossProduct(V1, V2);
	return Result;
}

bool FixOverCorrection(const FVector& InNormal, const FVector& InitialLocation, const FVector& TargetLocation, FVector& CurTraceFixTargetDirection, const FVector& CurLocation, const float CurDeltaDistance)
{
	//处理过度修正

	const FVector& InitialTargetDirection = UKismetMathLibrary::GetDirectionUnitVector(InitialLocation, TargetLocation);

	FVector Normal = UKismetMathLibrary::Cross_VectorVector(InNormal, InitialTargetDirection).GetSafeNormal();
	if (FVector::DotProduct(Normal, CurTraceFixTargetDirection) < 0.0f)
	{
		Normal = -Normal;
	}

	const FVector& TmpTraceFixTargetLocation = CurLocation + CurTraceFixTargetDirection * CurDeltaDistance;

	float T;
	FVector IntersectionPoint;
	const bool IntersectRet = UKismetMathLibrary::LinePlaneIntersection_OriginNormal(CurLocation, TmpTraceFixTargetLocation, InitialLocation, Normal, T, IntersectionPoint);
	if (!IntersectRet || FMath::IsNearlyZero(T))
	{
		return false;
	}

	// FVector Segment1Point;
	// FVector Segment2Point;
	// UKismetMathLibrary::FindNearestPointsOnLineSegments(CurLocation, TmpTraceFixTargetLocation, InitialLocation, TargetLocation
	// 	, Segment1Point, Segment2Point);

	// const FVector Dir1 = UKismetMathLibrary::GetDirectionUnitVector(Segment2Point, TmpTraceFixTargetLocation);

	const FVector Dir1 = UKismetMathLibrary::GetDirectionUnitVector(IntersectionPoint, TmpTraceFixTargetLocation);

	const bool bNeedReAdvance = FVector::DotProduct(Normal, Dir1) >= 0.0f;
	if (bNeedReAdvance)
	{
		FVector Point1, Point2, FinalPoint;
		UDLCommonBlueprintFunctionLibrary::K2_PointsCrossCircle(InitialLocation, TargetLocation, CurLocation, CurDeltaDistance, Point1, Point2);

		if (FVector::DistSquared(TargetLocation, Point1) < FVector::DistSquared(TargetLocation, Point2))
		{
			FinalPoint = Point1;
		}
		else
		{
			FinalPoint = Point2;
		}
		const FVector TmpDir = CurTraceFixTargetDirection;
		CurTraceFixTargetDirection = UKismetMathLibrary::GetDirectionUnitVector(CurLocation, FinalPoint);

		DL_PATH_CONTROLLER_LOG(Log, TEXT("<FixOverCorrection> 原始方向:%s, 新的方向:%s"), *TmpDir.ToString(), *CurTraceFixTargetDirection.ToString());
		return true;
	}
	return false;
}

void UPCTraceStepperLockV4::OnInitialize(const FPCLaunchParam& InLaunchParam,
	const FTransform& InSubObjectInitialTransform, UDLPathControllerComponent* InPCComp, const bool bEnableDebug)
{
	Super::OnInitialize(InLaunchParam, InSubObjectInitialTransform, InPCComp);
#if WITH_EDITOR
	DrawRawDebugLine(InLaunchParam, InSubObjectInitialTransform, InPCComp, bEnableDebug);
#endif

	LastEvaluateTransform = SubObjectInitialTransform;
	LastTransform = SubObjectInitialTransform;

	if (!LaunchParam.TargetActor.IsValid())
	{
		ensureMsgf(false, TEXT("为什么锁定状态下却没有锁定目标！！！！"));
		return;
	}

	if (LaunchParam.bIsHoming && LaunchParam.Asset->HomingTriggerTime >= LaunchParam.PersistTime && LaunchParam.PersistTime > 0.0f)
	{
		ensureMsgf(false, TEXT("为啥一定命中的表现触发时间会大于等于子物体持续时间！！！！"));
		return;
	}

	const int32 Num = LaunchParam.Asset->GetNumberOfSplineSegments();

	DefaultSpeed = LaunchParam.Asset->GetSplineLength() / LaunchParam.Asset->Duration;

	for (int i = 0; i <= Num; ++i)
	{
		const float LocDistance = LaunchParam.Asset->GetDistanceAlongSplineAtSplineInputKey(float(i));
		PointMarkers.Emplace(LocDistance);
	}

	//初始化时手动进行第一次迭代和方向修正
	if (LaunchParam.bIsTrack)
	{
		//先检测是否需要修正
		const FVector& CurTargetLocation = LaunchParam.TargetActor->GetActorLocation();
		FVector ClosestLocation = LaunchParam.Asset->FindLocationClosestToWorldLocation(
		   CurTargetLocation, SubObjectInitialTransform);
		ClosestLocation = SubObjectInitialTransform.TransformPosition(ClosestLocation);
		LastTargetLocation = CurTargetLocation;

		if (!ClosestLocation.Equals(CurTargetLocation, TARGET_ARRIVE_BIAS))
		{
			bNeedTraceFix = true;
		}
	}

	Advance(CheckDeltaSeconds(PCComp->GetWorld()->GetDeltaSeconds()) * LaunchParam.Asset->IterateFrequency);
}

void UPCTraceStepperLockV4::OnEvaluate(const float DeltaTime, FTransform& OutTransform)
{
	Super::OnEvaluate(DeltaTime, OutTransform);
	DL_PATH_CONTROLLER_LOG(Log, TEXT("<UPCTraceStepperLockV4::OnEvaluate> DeltaTime:%f"), DeltaTime);
	//记录一下Distance
	const float PrevDistance = Distance;

	FVector TargetLocation;
	if (!LaunchParam.TargetActor.IsValid())
	{
		TargetLocation = LastTargetLocation;
	}
	else
	{
		TargetLocation = LaunchParam.TargetActor->GetActorLocation();
	}

	if (CheckIsArriveTargetRadius())
	{
		//子物体已到达目标范围内
		LastTargetLocation = TargetLocation;
		const float CurDistance = UKismetMathLibrary::Vector_Distance(PCComp->GetOwner()->GetActorLocation(), TargetLocation);
		const float NeedMoveDistance = DeltaTime * ArrivedTargetBaseSpeed * LaunchParam.Asset->SpeedRateAfterArrivedTarget;

		if (CurDistance < NeedMoveDistance)
		{
			bArrivedTarget = true;
			Distance += CurDistance;
			OutTransform.SetLocation(TargetLocation);
		}
		else
		{
			Distance += NeedMoveDistance;
			const FVector& CurLocation = PCComp->GetOwner()->GetActorLocation();
			OutTransform.SetLocation(UKismetMathLibrary::VLerp(CurLocation, TargetLocation, NeedMoveDistance / CurDistance));
		}
		//TODO 考虑需不需要设置旋转
		OutTransform.SetScale3D(PCComp->GetOwner()->GetActorScale3D());
	}
	else if (CheckShouldHoming())
	{
		//一定命中表现处理

		LastTargetLocation = TargetLocation;

		const float CurDistance = UKismetMathLibrary::Vector_Distance(PCComp->GetOwner()->GetActorLocation(), TargetLocation);
		HomingCount++;
		const float NeedMoveDistance = DeltaTime * HomingBaseSpeed * (HomingCount * LaunchParam.Asset->HomingFixAmplitude + 1);
		const FVector& CurLocation = PCComp->GetOwner()->GetActorLocation();
		if (CurDistance < NeedMoveDistance)
		{
			bArrivedTarget = true;
			Distance += CurDistance;
			OutTransform.SetLocation(TargetLocation);
		}
		else
		{
			Distance += NeedMoveDistance;

			OutTransform.SetLocation(UKismetMathLibrary::VLerp(CurLocation, TargetLocation, NeedMoveDistance / CurDistance));
		}

	}
	else if (CheckIsOverDuration())
	{
		FTransform CurTransform = PCComp->GetOwner()->GetTransform();
		const float DeltaDistance = LaunchParam.Asset->SpeedOverDuration * DeltaTime;
		Distance += DeltaDistance;

		CurTransform.SetLocation(CurTransform.GetLocation()
			+ DirectionWhenOverDuration * DeltaDistance);
		OutTransform = CurTransform;
	}
	else if (CheckIsOverFixDuration() && bNeedTraceFix)
	{
		FTransform CurTransform = PCComp->GetOwner()->GetTransform();
		const float DeltaDistance = OverFixBaseSpeed * DeltaTime;
		Distance += DeltaDistance;

		CurTransform.SetLocation(CurTransform.GetLocation() + DirectionWhenOverFixDuration * DeltaDistance);
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

		//插值处理
		// const FVector& CurLocation = PCComp->GetOwner()->GetActorLocation();

		FVector NewLoc;
		FVector CurDirection;
		float DeltaMoveDistance;

		DL_PATH_CONTROLLER_LOG(Log, TEXT("<UPCTraceStepperLockV4::OnEvaluate> IterateTime:%f CurDeltaMove:%f"), IterateTime, CurDeltaMove);

		if (FMath::IsNearlyEqual(IterateTime, CurDeltaMove))
		{
			NewLoc = TargetTransform.GetLocation();
		}
		else if (IterateTime > CurDeltaMove)
		{
			float TmpCurDeltaDistance;
			FVector TmpTraceFixTargetDirection;
			int32 TmpTraceFixCount;
			FVector TmpTraceFixTargetLoc;
			AdvanceInternal(DeltaTime, TmpCurDeltaDistance, TmpTraceFixTargetDirection, TmpTraceFixCount, TmpTraceFixTargetLoc, TargetTransform);
			NewLoc = TargetTransform.GetLocation();
		}
		else
		{
			const float Alpha = FMath::Clamp(IterateTime / CurDeltaMove, 0.0f, 1.0f);
			CurDirection = UKismetMathLibrary::GetDirectionUnitVector(LastTransform.GetLocation(), TargetTransform.GetLocation());
			DeltaMoveDistance = UKismetMathLibrary::Lerp(0.0f, CurDeltaDistance, Alpha);
			NewLoc = LastTransform.GetLocation() + CurDirection * DeltaMoveDistance;
		}


		OutTransform.SetLocation(NewLoc);
		DL_PATH_CONTROLLER_LOG(Log, TEXT("PcStepper OutTransform:%s"), *OutTransform.GetLocation().ToString());

		CurDirection = UKismetMathLibrary::GetDirectionUnitVector(PCComp->GetOwner()->GetActorLocation(), NewLoc);

		DeltaMoveDistance = FVector::Distance(PCComp->GetOwner()->GetActorLocation(), NewLoc);
		Distance += DeltaMoveDistance;

		ApplyGravity(DeltaTime, OutTransform);

		DL_PATH_CONTROLLER_LOG(Log, TEXT("Normal OutTransform:%s"), *OutTransform.ToString());

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

	{
		//检测轨迹是否经过了目标位置
		const FVector& CurSubObjPos = PCComp->GetOwner()->GetActorLocation();
		const FVector ClosestPoint = FMath::ClosestPointOnSegment(TargetLocation, CurSubObjPos, OutTransform.GetLocation());

		const bool bIsOnTheTrace = ClosestPoint.Equals(TargetLocation, 0.1);

		if (bIsOnTheTrace)
		{
			OutTransform.SetLocation(TargetLocation);
		}
	}

	LastEvaluateTransform = PCComp->GetOwner()->GetActorTransform();

	//设置已运行时间
	TimePassed += DeltaTime;

}

void UPCTraceStepperLockV4::OnPostEvaluate(const float DeltaTime)
{
	//分步骤判断对性能更加友好,因为这里有些计算比较耗
	if (bArrivedTarget)
	{
		FinishTrace();
		return;
	}

	//需要判断是否已到达目标位置
	const FVector& CurLocation = PCComp->GetOwner()->GetActorLocation();
	if (CurLocation.Equals(LastTargetLocation, TARGET_ARRIVE_BIAS))
	{
		FinishTrace();
		return;
	}

	if (LaunchParam.bIsHoming || bArrivedTargetRadius)
	{
		return;
	}

	if (TimePassed >= LaunchParam.PersistTime)
	{
		FinishTrace();
		return;
	}

	// const float MaxDisplacement = LaunchParam.Asset->GetSplineDisplacementSquared();
	// const float CurDisplacement = FVector::DistSquared(CurLocation, SubObjectInitialTransform.GetLocation());
	// if (CurDisplacement >= MaxDisplacement - TARGET_ARRIVE_BIAS)
	// {
	// 	FinishTrace();
	// 	return;
	// }
}



void UPCTraceStepperLockV4::Advance(const float DeltaMoveTime)
{
	Super::Advance(DeltaMoveTime);
	FVector TargetLocation;
	if (!LaunchParam.TargetActor.IsValid())
	{
		TargetLocation = LastTargetLocation;
	}
	else
	{
		TargetLocation = LaunchParam.TargetActor->GetActorLocation();
	}

	AdvanceInternal(DeltaMoveTime, CurDeltaDistance, TraceFixTargetDirection, TraceFixCount, TraceFixTargetLocation, TargetTransform);

	LastTransform = PCComp->GetOwner()->GetActorTransform();
	LastTargetLocation = TargetLocation;
	//记录迭代次数
	IterateCount++;
}

bool UPCTraceStepperLockV4::CheckIsOverDuration()
{
	if (!bIsOverDuration)
	{
		bIsOverDuration = (LaunchParam.Asset->Duration <= TimePassed);
		if (bIsOverDuration)
		{
			DirectionWhenOverDuration = UKismetMathLibrary::GetDirectionUnitVector(LastEvaluateTransform.GetLocation(), PCComp->GetOwner()->GetActorLocation());
		}
	}
	return bIsOverDuration;

}

bool UPCTraceStepperLockV4::CheckIsArriveTargetRadius()
{
	if (LaunchParam.Asset->EnableTargetRadiusCheck && !bArrivedTargetRadius && LaunchParam.TargetActor.IsValid())
	{
		const auto CapsuleComp = Cast<UCapsuleComponent>(
			LaunchParam.TargetActor->GetComponentByClass(UCapsuleComponent::StaticClass()));

		if (CapsuleComp)
		{
			const float CurDistance = UKismetMathLibrary::Vector_Distance(PCComp->GetOwner()->GetActorLocation()
				, LaunchParam.TargetActor->GetActorLocation());

			bArrivedTargetRadius = CurDistance <= (CapsuleComp->GetScaledCapsuleRadius() + LaunchParam.Asset->TargetRadius);
			if (bArrivedTargetRadius)
			{
				//记录一下速度
				ArrivedTargetBaseSpeed = FVector::DistSquared(LastEvaluateTransform.GetLocation(), PCComp->GetOwner()->GetActorLocation())
					/ FMath::Square(CheckDeltaSeconds(0.0f));
				ArrivedTargetBaseSpeed = FMath::Sqrt(ArrivedTargetBaseSpeed);
				//派发到达目标范围内的通知
				PCComp->OnArriveTargetRadius.Broadcast(LaunchParam.Asset);
			}
			return bArrivedTargetRadius;
		}
	}
	return bArrivedTargetRadius;

}

bool UPCTraceStepperLockV4::CheckShouldHoming()
{
	if (LaunchParam.bIsHoming && !bIsHoming && (TimePassed >= LaunchParam.Asset->HomingTriggerTime))
	{
		bIsHoming = true;
		if (bIsHoming)
		{
			//记录一下速度
			const float DistanceSquared = FVector::DistSquared(LastEvaluateTransform.GetLocation(), PCComp->GetOwner()->GetActorLocation());
			HomingBaseSpeed = FMath::Sqrt(DistanceSquared / FMath::Square(CheckDeltaSeconds(0.0f)));
			//派发触发一定命的表现通知
			PCComp->TriggerHoming(LaunchParam.Asset);
		}
		return bIsHoming;
	}
	return bIsHoming;

}

bool UPCTraceStepperLockV4::CheckIsOverFixDuration()
{
	if (!bOverTraceFixOverDuration && (TimePassed >= LaunchParam.Asset->MaxFixDuration))
	{
		bOverTraceFixOverDuration = true;
		if (bOverTraceFixOverDuration)
		{
			//记录一下速度
			const float DistanceSquared = FVector::DistSquared(LastEvaluateTransform.GetLocation(), PCComp->GetOwner()->GetActorLocation());
			OverFixBaseSpeed = FMath::Sqrt(DistanceSquared / FMath::Square(CheckDeltaSeconds(0.0f)));
			DirectionWhenOverFixDuration = UKismetMathLibrary::GetDirectionUnitVector(LastEvaluateTransform.GetLocation(), PCComp->GetOwner()->GetActorLocation());
			//TODO:派发通知
			// PCComp->TriggerHoming(LaunchParam.Asset);
		}
	}
	return bOverTraceFixOverDuration;
}

bool UPCTraceStepperLockV4::CheckIsSplineEnd()
{
	if (!bIsSplineEnd)
	{
		bIsSplineEnd = Distance >= LaunchParam.Asset->GetSplineLength();
		if (bIsSplineEnd)
		{
			//记录一下方向
			SplineEndDirection = UKismetMathLibrary::GetDirectionUnitVector(LastEvaluateTransform.GetLocation(), PCComp->GetOwner()->GetActorLocation());
			//TODO:派发通知
			// PCComp->TriggerHoming(LaunchParam.Asset);
		}
	}
	return bIsSplineEnd;
}

void UPCTraceStepperLockV4::AdvanceInternal(const float DeltaTime, float& DeltaDistance, FVector& OutTraceFixTargetDirection
	, int32& OutTraceFixCount, FVector& OutTraceFixTargetLocation, FTransform& OutTargetTransform)
{
	FVector TargetLocation;
	if (!LaunchParam.TargetActor.IsValid())
	{
		TargetLocation = LastTargetLocation;
	}
	else
	{
		TargetLocation = LaunchParam.TargetActor->GetActorLocation();
	}

	const FTransform& CurTransform = PCComp->GetOwner()->GetTransform();

	//检查是否需要修正
	if (!LastTargetLocation.Equals(TargetLocation, 1.0f))
	{
		bNeedTraceFix = true;
	}

	float Speed = DefaultSpeed;
	if (LaunchParam.Asset->CustomSpeedCurve)
	{
		const float Begin = FMath::Clamp(TimePassed / LaunchParam.Asset->Duration, 0.0f, 1.0f);
		const float End = FMath::Clamp((TimePassed + DeltaTime) / LaunchParam.Asset->Duration, 0.0f, 1.0f);
		Speed = UPathControllerLibrary::CalculateAverageValue(LaunchParam.Asset->CustomSpeedCurve, Begin, End);
	}

	DeltaDistance = DeltaTime * Speed;

	if (Speed > 0.0f)
	{
		if (bNeedTraceFix)
		{

			const FVector& CurLocation = CurTransform.GetLocation();
			const FVector& TargetDirection = UKismetMathLibrary::GetDirectionUnitVector(CurLocation, TargetLocation);

			FVector RawDirection;
			if (CheckIsSplineEnd())
			{
				RawDirection = SplineEndDirection;
			}
			else
			{
				const FVector& PrevSplineLocation = SubObjectInitialTransform.TransformPosition(
				LaunchParam.Asset->GetLocationAtDistanceAlongSpline(Distance));

				const FVector& NextSplineLocation = SubObjectInitialTransform.TransformPosition(
					LaunchParam.Asset->GetLocationAtDistanceAlongSpline(Distance + DeltaDistance));

				RawDirection = UKismetMathLibrary::GetDirectionUnitVector(PrevSplineLocation, NextSplineLocation);

				ensureAlways(!RawDirection.IsNearlyZero());

			}

			FVector CurDirection = FVector::ZeroVector;
			if (TraceFixCount == 0)
			{
				CurDirection = RawDirection;
			}
			else
			{
				CurDirection = UKismetMathLibrary::GetDirectionUnitVector(LastEvaluateTransform.GetLocation(), CurLocation);
			}

			OutTraceFixTargetDirection = RawDirection;
			if (!RawDirection.Equals(TargetDirection, 0.01f))
			{
				float Angle;

				FVector Normal = UKismetMathLibrary::Cross_VectorVector(RawDirection, TargetDirection).GetSafeNormal();
				//处理法线为0的情况
				if (Normal.IsNearlyZero(0.01f))
				{
					Normal = FVector(0.0f, 0.0f, 1.0f);
				}

				if (Normal.Z < 0.0f)
				{
					Normal = -Normal;
				}

				DL_PATH_CONTROLLER_LOG(Log, TEXT("<UPCTraceStepperLockV4::AdvanceInternal> TraceFix RawDirection:%s,TargetDirection:%s, Normal:%s")
						, *RawDirection.ToString(), *TargetDirection.ToString(), *Normal.ToString());

				const float Dot = FVector::DotProduct(CurDirection, TargetDirection);
				Angle = FMath::RadiansToDegrees(acosf(Dot));

				DL_PATH_CONTROLLER_LOG(Log, TEXT("<UPCTraceStepperLockV4::AdvanceInternal> TraceFix Angle:%f"), Angle);

				//如果大于修正角度时采用最大修正角度
				Angle = FMath::Clamp(Angle, -LaunchParam.Asset->FixAngle, LaunchParam.Asset->FixAngle);

				// FMath::FindDeltaAngle()

				{
					const FVector FixedPoint1 = CurLocation + UKismetMathLibrary::RotateAngleAxis(RawDirection, Angle, Normal) * DeltaDistance;
					// const FVector FixedPoint2 = CurLocation + UKismetMathLibrary::RotateAngleAxis(RawDirection, -Angle, Normal) * CurDeltaDistance;
					const FVector TargetPoint = CurLocation + TargetDirection * DeltaDistance;

					const FVector InitialLocation = SubObjectInitialTransform.GetLocation();

					const FVector RawFixedPoint1 = CurLocation + RawDirection * DeltaDistance;

					const FVector ProjectedPoint1 = UKismetMathLibrary::ProjectVectorOnToPlane(FixedPoint1, Normal);
					// const FVector ProjectedPoint2 = UKismetMathLibrary::ProjectVectorOnToPlane(FixedPoint2, Normal);
					const FVector ProjectedTargetPoint = UKismetMathLibrary::ProjectVectorOnToPlane(TargetPoint, Normal);
					const FVector ProjectedInitialLocation = UKismetMathLibrary::ProjectVectorOnToPlane(InitialLocation, Normal);
					const FVector ProjectedCurLocation = UKismetMathLibrary::ProjectVectorOnToPlane(CurLocation, Normal);
					const FVector ProjectedRawFixedPoint1 = UKismetMathLibrary::ProjectVectorOnToPlane(RawFixedPoint1, Normal);

					const float Left0 = IsLeft(FVector2D(ProjectedInitialLocation.X, ProjectedInitialLocation.Y)
						, FVector2D(ProjectedCurLocation.X, ProjectedCurLocation.Y)
						, FVector2D(ProjectedTargetPoint.X, ProjectedTargetPoint.Y));

					const float Left1 = IsLeft(FVector2D(ProjectedCurLocation.X, ProjectedCurLocation.Y)
						, FVector2D(ProjectedRawFixedPoint1.X, ProjectedRawFixedPoint1.Y)
						, FVector2D(ProjectedPoint1.X, ProjectedPoint1.Y));

					if ((Left0 > 0 && Left1 < 0) || (Left0 < 0 && Left1 > 0))
					{
						Angle = -Angle;
					}
				}

				DL_PATH_CONTROLLER_LOG(Log, TEXT("<UPCTraceStepperLockV4::AdvanceInternal> TraceFix Angle:%f,Normal:%s,TargetDirection:%s")
					, Angle, *Normal.ToString(), *TargetDirection.ToString());

				OutTraceFixTargetDirection = UKismetMathLibrary::RotateAngleAxis(RawDirection, Angle, Normal).GetSafeNormal();

				//处理过度修正
				FixOverCorrection(Normal, SubObjectInitialTransform.GetLocation(), TargetLocation, OutTraceFixTargetDirection, CurLocation, DeltaDistance);
			}

			OutTraceFixCount++;

			OutTraceFixTargetLocation = CurLocation + OutTraceFixTargetDirection * DeltaDistance;

			const FVector TmpDir = UKismetMathLibrary::GetDirectionUnitVector(CurLocation, OutTraceFixTargetLocation);
			DL_PATH_CONTROLLER_LOG(Log, TEXT("<UPCTraceStepperLockV4::AdvanceInternal> TmpDir:%s"), *TmpDir.ToString());

			OutTargetTransform.SetLocation(OutTraceFixTargetLocation);

			DL_PATH_CONTROLLER_LOG(Log, TEXT("<UPCTraceStepperLockV4::AdvanceInternal> PcStepper TargetTransform:%s"), *OutTraceFixTargetLocation.ToString());
		}
		else
		{
			FTransform NewTrans;
			if (LaunchParam.Asset->CustomSpeedCurve)
			{
				const float LocDistance = Distance + CurDeltaDistance;
				NewTrans = LaunchParam.Asset->GetTransformAtDistanceAlongSpline(LocDistance);
			}
			else
			{
				NewTrans = LaunchParam.Asset->GetTransformAtTime(TimePassed + DeltaTime, true);
			}
			NewTrans = NewTrans * SubObjectInitialTransform;
			OutTargetTransform = NewTrans;
		}
	}
	else
	{
		DL_PATH_CONTROLLER_LOG_WITHOUT_VALUE(Warning, TEXT("<UPCTraceStepperLockV4::AdvanceInternal> 速度等于0！！！"));
	}
}


