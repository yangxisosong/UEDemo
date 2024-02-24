// Fill out your copyright notice in the Description page of Project Settings.


#include "PCTraceStepperFreeTrack.h"

#include "DLPathControllerComponent.h"
#include "PathControllerTraceAsset.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPCTraceStepperFreeTrack::OnInitialize(const FPCLaunchParam& InLaunchParam,
	const FTransform& InSubObjectInitialTransform, UDLPathControllerComponent* InPCComp, const bool bEnableDebug)
{
	Super::OnInitialize(InLaunchParam, InSubObjectInitialTransform, InPCComp);
	LastEvaluateTransform = SubObjectInitialTransform;
	LastTransform = SubObjectInitialTransform;

	if (!LaunchParam.TargetActor.IsValid())
	{
		ensureMsgf(false, TEXT("为什么锁定状态下却没有锁定目标！！！！"));
		return;
	}


	if (!LaunchParam.Asset->CustomSpeedCurve)
	{
		ensureMsgf(false, TEXT("没有配置速度曲线！！！！"));
		return;
	}


	if (LaunchParam.bIsHoming && LaunchParam.Asset->HomingTriggerTime >= LaunchParam.PersistTime && LaunchParam.PersistTime > 0.0f)
	{
		ensureMsgf(false, TEXT("为啥一定命中的表现触发时间会大于等于子物体持续时间！！！！"));
		return;
	}

	//初始化时手动进行第一次迭代和方向修正
	Advance(CheckDeltaSeconds(PCComp->GetWorld()->GetDeltaSeconds()) * LaunchParam.Asset->IterateFrequency);
}

void UPCTraceStepperFreeTrack::OnEvaluate(const float DeltaTime, FTransform& OutTransform)
{
	Super::OnEvaluate(DeltaTime, OutTransform);
	//记录一下Distance
	// const float PrevDistance = Distance;

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
	// else if (CheckIsOverDuration())
	// {
	// 	FTransform CurTransform = PCComp->GetOwner()->GetTransform();
	// 	const float DeltaDistance = LaunchParam.Asset->SpeedOverDuration * DeltaTime;
	// 	Distance += DeltaDistance;
	//
	// 	CurTransform.SetLocation(CurTransform.GetLocation()
	// 		+ DirectionWhenOverDuration * DeltaDistance);
	// 	OutTransform = CurTransform;
	// }
	else if (!CheckIsNeedFix())
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
		const FVector& CurLocation = PCComp->GetOwner()->GetActorLocation();

		FVector NewLoc;

		if (FMath::IsNearlyEqual(IterateTime, CurDeltaMove))
		{
			NewLoc = TargetTransform.GetLocation();
		}
		else if (IterateTime > CurDeltaMove)
		{
			float Speed = 0.0f;
			if (LaunchParam.Asset->CustomSpeedCurve)
			{
				Speed = LaunchParam.Asset->CustomSpeedCurve->GetFloatValue(FMath::Clamp(TimePassed / LaunchParam.Asset->Duration, 0.0f, 1.0f));
			}

			const float DeltaDistance = DeltaTime * Speed;
			FVector TmpTraceFixTargetDirection = FVector(1.0f, 0.0f, 0.0f);
			if (Speed > 0.0f)
			{
				FVector RawDirection;
				if (TraceFixCount == 0)
				{
					RawDirection = LaunchParam.InitialDirection;
				}
				else
				{
					RawDirection = TraceFixTargetDirection;
				}

				const FVector& TargetDirection = UKismetMathLibrary::GetDirectionUnitVector(CurLocation, TargetLocation);

				TmpTraceFixTargetDirection = RawDirection;

				if (!RawDirection.Equals(TargetDirection))
				{
					FVector Normal = UKismetMathLibrary::Cross_VectorVector(RawDirection, TargetDirection).GetSafeNormal();

					//处理法线为0的情况
					if (Normal.IsNearlyZero())
					{
						Normal = FVector(0.0f, 0.0f, 1.0f);
					}

					FVector CurDirection;
					if (TraceFixCount == 0)
					{
						CurDirection = LaunchParam.InitialDirection;
					}
					else
					{
						CurDirection = UKismetMathLibrary::GetDirectionUnitVector(LastEvaluateTransform.GetLocation(), CurLocation);
					}

					float Angle = FMath::RadiansToDegrees(acosf(
						FVector::DotProduct(CurDirection, TargetDirection)));

					//如果大于修正角度时采用最大修正角度
					Angle = FMath::ClampAngle(Angle
						, -LaunchParam.Asset->FixAngle, LaunchParam.Asset->FixAngle);


					TmpTraceFixTargetDirection = UKismetMathLibrary::RotateAngleAxis(RawDirection, Angle, Normal);
				}
			}

			NewLoc = CurLocation + TmpTraceFixTargetDirection * DeltaDistance;
		}
		else
		{
			//插值处理
			const float Alpha = FMath::Clamp(IterateTime / CurDeltaMove, 0.0f, 1.0f);
			NewLoc = UKismetMathLibrary::VLerp(LastTransform.GetLocation()
						, TargetTransform.GetLocation(), Alpha);
		}

		const FVector CurDirection = UKismetMathLibrary::GetDirectionUnitVector(LastEvaluateTransform.GetLocation(), NewLoc);
		OutTransform.SetLocation(NewLoc);
		const float DeltaMoveDistance = UKismetMathLibrary::Vector_Distance(CurLocation, OutTransform.GetLocation());

		// DL_PATH_CONTROLLER_LOG(Log, TEXT("InterppedDirection:%s,Alpha:%f,DeltaMoveDistance:%f")
		// 	, *CurTargetLocation.ToString(), Alpha, DeltaMoveDistance);

		Distance += DeltaMoveDistance;
		ApplyGravity(DeltaTime, OutTransform);
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

void UPCTraceStepperFreeTrack::OnPostEvaluate(const float DeltaTime)
{
	Super::OnPostEvaluate(DeltaTime);

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

	if (TimePassed >= LaunchParam.PersistTime && LaunchParam.PersistTime > 0.0f)
	{
		FinishTrace();
		return;
	}
	//
	// const float MaxDisplacement = LaunchParam.Asset->GetSplineDisplacementSquared();
	// const float CurDisplacement = FVector::DistSquared(CurLocation, SubObjectInitialTransform.GetLocation());
	// if (CurDisplacement >= MaxDisplacement - TARGET_ARRIVE_BIAS)
	// {
	// 	FinishTrace(LaunchParam.Asset, PCComp);
	// 	return;
	// }
}

void UPCTraceStepperFreeTrack::Advance(const float DeltaMoveTime)
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

	const FTransform& CurTransform = PCComp->GetOwner()->GetTransform();
	float Speed = 0.0f;
	if (LaunchParam.Asset->CustomSpeedCurve)
	{
		Speed = LaunchParam.Asset->CustomSpeedCurve->GetFloatValue(FMath::Clamp(TimePassed / LaunchParam.Asset->Duration, 0.0f, 1.0f));
	}

	const float DeltaDistance = DeltaMoveTime * Speed;

	const FVector& CurLocation = CurTransform.GetLocation();

	if (Speed > 0.0f)
	{
		FVector RawDirection;
		if (TraceFixCount == 0)
		{
			RawDirection = LaunchParam.InitialDirection;
		}
		else
		{
			RawDirection = TraceFixTargetDirection;
		}

		const FVector& TargetDirection = UKismetMathLibrary::GetDirectionUnitVector(CurLocation, TargetLocation);

		TraceFixTargetDirection = RawDirection;

		if (!RawDirection.Equals(TargetDirection))
		{
			FVector Normal = UKismetMathLibrary::Cross_VectorVector(RawDirection, TargetDirection).GetSafeNormal();

			//处理法线为0的情况
			if (Normal.IsNearlyZero())
			{
				Normal = FVector(0.0f, 0.0f, 1.0f);
			}

			FVector CurDirection;
			if (TraceFixCount == 0)
			{
				CurDirection = LaunchParam.InitialDirection;
			}
			else
			{
				CurDirection = UKismetMathLibrary::GetDirectionUnitVector(LastEvaluateTransform.GetLocation(), CurLocation);
			}

			float Angle = FMath::RadiansToDegrees(acosf(
				FVector::DotProduct(CurDirection, TargetDirection)));

			//如果大于修正角度时采用最大修正角度
			Angle = FMath::ClampAngle(Angle
				, -LaunchParam.Asset->FixAngle, LaunchParam.Asset->FixAngle);


			TraceFixTargetDirection = UKismetMathLibrary::RotateAngleAxis(RawDirection, Angle, Normal);

			// DL_PATH_CONTROLLER_LOG(Log, TEXT("Angle:%f,Normal:%s,TraceFixTargetDirection:%s"), Angle, *Normal.ToString()
			// 	, *TraceFixTargetDirection.ToString());
		}
	}

	TraceFixTargetLocation = CurLocation + TraceFixTargetDirection * DeltaDistance;
	TraceFixCount++;

	TargetTransform.SetLocation(TraceFixTargetLocation);
	LastTransform = CurTransform;
	LastTargetLocation = TargetLocation;

	//记录迭代次数
	IterateCount++;
}

bool UPCTraceStepperFreeTrack::CheckIsOverDuration()
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

bool UPCTraceStepperFreeTrack::CheckIsArriveTargetRadius()
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
		}
	}
	return bArrivedTargetRadius;
}

bool UPCTraceStepperFreeTrack::CheckShouldHoming()
{
	DL_PATH_CONTROLLER_LOG(Log, TEXT("<CheckShouldHoming>   TimePassed:%f,LaunchParam.Asset->HomingTriggerTime:%f")
	, TimePassed, LaunchParam.Asset->HomingTriggerTime);

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
	}
	return bIsHoming;
}

bool UPCTraceStepperFreeTrack::CheckIsNeedFix()
{
	if (bNeedTraceFix && (TimePassed >= LaunchParam.Asset->MaxFixDuration))
	{
		bNeedTraceFix = false;
		if (!bNeedTraceFix)
		{
			//记录一下速度
			const float DistanceSquared = FVector::DistSquared(LastEvaluateTransform.GetLocation(), PCComp->GetOwner()->GetActorLocation());
			OverFixBaseSpeed = FMath::Sqrt(DistanceSquared / FMath::Square(CheckDeltaSeconds(0.0f)));
			DirectionWhenOverFixDuration = UKismetMathLibrary::GetDirectionUnitVector(LastEvaluateTransform.GetLocation(), PCComp->GetOwner()->GetActorLocation());
			//TODO:派发触发通知
			// PCComp->TriggerHoming(LaunchParam.Asset);
		}
	}
	return bNeedTraceFix;
}
