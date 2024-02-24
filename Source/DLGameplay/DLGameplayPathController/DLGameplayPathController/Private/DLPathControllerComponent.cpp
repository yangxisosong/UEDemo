// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameplayPathController/Public/DLPathControllerComponent.h"

#include "ConvertString.h"
#include "DLDrawDebugSubsystem.h"
#include "PathControllerTraceAsset.h"
#include "PCTraceAssetStepper.h"
#include "PCTraceStepperFreeTrack.h"
#include "PCTraceStepperLockV4.h"
#include "Kismet/KismetMathLibrary.h"
#include "VisualLogger/VisualLogger.h"
#include "VisualLogger/VisualLoggerKismetLibrary.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif



// Sets default values for this component's properties
UDLPathControllerComponent::UDLPathControllerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UDLPathControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

static void LogLocation(UObject* WorldContextObject, FVector Location, FString Text, FLinearColor ObjectColor = FLinearColor::Blue, float Radius = 10, FName LogCategory = TEXT("VisLogBP"), bool bAddToMessageLog = false);
void LogLocation(UObject* WorldContextObject, FVector Location, FString Text, FLinearColor Color, float Radius, FName CategoryName, bool bAddToMessageLog)
{
#if ENABLE_VISUAL_LOG
	const ELogVerbosity::Type DefaultVerbosity = ELogVerbosity::Log;
	FVisualLogger::GeometryShapeLogf(WorldContextObject, CategoryName, DefaultVerbosity
		, Location, Radius, Color.ToFColor(true), TEXT("%s"), *Text);
#endif
	if (bAddToMessageLog)
	{
		FMessageLog(CategoryName).Info(FText::FromString(FString::Printf(TEXT("LogLocation: '%s' - Location: (%s)")
			, *Text, *Location.ToString())));
	}
}


// Called every frame
void UDLPathControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
												   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if WITH_EDITOR
	PreviewDeltaSeconds = DeltaTime;
#endif

	if (bIsPlaying && DeltaTime > 0.0f)
	{
		FTransform NewTransform;
		const UObject* PreStepper = PCAssetStepper;
		UPathControllerTraceAsset* CopyAsset = CurLaunchParam.Asset;
		// DL_PATH_CONTROLLER_LOG(Log, TEXT("<UDLPathControllerComponent::TickComponent>  %s[%s] UDLPathControllerComponent::TickComponent DeltaTime:%f")
		// 	, *GetOwner()->GetName(), *ENUM_TO_STRING(GetOwner()->GetLocalRole()), DeltaTime);

		PCAssetStepper->Evaluate(DeltaTime, NewTransform);

#if WITH_EDITOR
		if (CVarPathControllerDebug->GetBool() || ShowDebugLine)
		{
			const float Distance = UKismetMathLibrary::Vector_Distance(NewTransform.GetLocation(), GetOwner()->GetActorLocation());
			const float Speed = FMath::Abs(Distance / DeltaTime);
			ensure(!isinf(Speed));
			DL_PATH_CONTROLLER_LOG(Log, TEXT("<UDLPathControllerComponent::TickComponent>  %s[%s] 已飞行时间：%f,子物体飞行速度为：%f cm/s,当前位置：%s")
				, *GetOwner()->GetName(), *ENUM_TO_STRING(GetOwner()->GetLocalRole()), PCAssetStepper->GetTimePassed(), Speed, *GetOwner()->GetActorLocation().ToString());

			LogLocation(GetWorld(), NewTransform.GetLocation()
				, FString::Printf(TEXT("%s[%s]"), *GetOwner()->GetName(), *ENUM_TO_STRING(GetOwner()->GetLocalRole())));

			UDLDrawDebugSubsystem* DrawDebugSubsystem = GetWorld()->GetSubsystem<UDLDrawDebugSubsystem>();
			if (DrawDebugSubsystem)
			{
				DrawDebugSubsystem->ShowCategory_CustomCategory(TEXT("PathController"));
				DrawDebugSubsystem->DrawDebugLine_CustomCategory(TEXT("PathController"), GetOwner()->GetActorLocation(), NewTransform.GetLocation()
				, FColor::Green, 20.0f, 2.0f);
			}
		}
#endif

		//保持子物体正朝向与飞行方向一致
		{
			const FVector& FlyDirection = UKismetMathLibrary::GetDirectionUnitVector(
				GetOwner()->GetActorLocation(), NewTransform.GetLocation());
			const FVector& SubObjForwardVector = CopyAsset->SubObjectForword;
			const FRotator FinalRot = FlyDirection.Rotation() - SubObjForwardVector.Rotation();
			NewTransform.SetRotation(FinalRot.Quaternion());
		}

		NewTransform.SetScale3D(GetOwner()->GetActorScale3D());
		// ensureAlwaysMsgf(NewTransform.ContainsNaN(), TEXT("包含无效值！！！%s"), *NewTransform.ToString());
		GetOwner()->SetActorTransform(NewTransform, true);

		//防止Evaluate中的通知导致数据销毁或变更
		if (!this || PCAssetStepper != PreStepper)
		{
			TrackComplete(CopyAsset);
			return;
		}
		PCAssetStepper->PostEvaluate(DeltaTime);
	}
}

void UDLPathControllerComponent::LaunchToLocation_Implementation(UPathControllerTraceAsset* Asset,
	 FVector TargetLocation, const float PersistTime, const FVector InitialDirection, float MaxDistance)
{
	CurLaunchParam = FPCLaunchParam();
	if (!Asset)
	{
		Asset = DefaultPCAsset;
	}
	CurLaunchParam.Asset = Asset;

	if (!ensureAlwaysMsgf(CurLaunchParam.Asset, TEXT("请传入轨迹资产！！！")))
	{
		return;
	}

	if (MaxDistance < 0)
	{
		MaxDistance = CurLaunchParam.Asset->GetSplineDisplacement();
	}
	const float CurDistance = FVector::Distance(GetOwner()->GetActorLocation(), TargetLocation);
	if (CurDistance > MaxDistance)
	{
		const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(GetOwner()->GetActorLocation(), TargetLocation);
		TargetLocation = GetOwner()->GetActorLocation() + Direction * MaxDistance;
	}

	CurLaunchParam.TargetLocation = TargetLocation;

	CurLaunchParam.PersistTime = (PersistTime < 0 ? Asset->Duration : PersistTime);

	Stop();

	Start();

	PCAssetStepper = NewObject<UPCTraceAssetStepperToLocation>();
	FTransform SubObjectInitialTransform = GetOwner()->GetActorTransform();
	const auto& Quaternion = InitialDirection.GetSafeNormal().Rotation().Quaternion();
	SubObjectInitialTransform.SetRotation(Quaternion);

	SubObjectInitialTransform.SetScale3D(FVector(1.0f));
	PCAssetStepper->Initialize(CurLaunchParam, SubObjectInitialTransform, this, CVarPathControllerDebug->GetBool());
}

void UDLPathControllerComponent::LaunchToTarget_Implementation(UPathControllerTraceAsset* Asset, AActor* TargetActor,
															   const bool bIsTrack, const bool bIsHoming, const float PersistTime, const FVector InitialDirection)
{
	if (!ensureAlwaysMsgf(TargetActor, TEXT("TargetActor is invalid!!!!")))
	{
		return;
	}

	CurLaunchParam = FPCLaunchParam();
	if (!Asset)
	{
		Asset = DefaultPCAsset;
	}
	CurLaunchParam.Asset = Asset;
	CurLaunchParam.TargetActor = TWeakObjectPtr<AActor>(TargetActor);
	CurLaunchParam.bIsTrack = bIsTrack;
	CurLaunchParam.bIsHoming = bIsHoming;

	if (!ensureAlwaysMsgf(CurLaunchParam.Asset, TEXT("请传入轨迹资产！！！")))
	{
		return;
	}
	CurLaunchParam.PersistTime = (PersistTime < 0 ? Asset->Duration : PersistTime);

	Stop();

	Start();

	if (CurLaunchParam.bIsTrack)
	{
		PCAssetStepper = NewObject<UPCTraceStepperLockV4>();
		FTransform SubObjectInitialTransform = GetOwner()->GetActorTransform();

		const auto& Quaternion = InitialDirection.GetSafeNormal().Rotation().Quaternion();
		SubObjectInitialTransform.SetRotation(Quaternion);

		PCAssetStepper->Initialize(CurLaunchParam, SubObjectInitialTransform, this, CVarPathControllerDebug->GetBool());
	}
	else
	{
		LaunchToLocation(Asset, CurLaunchParam.TargetActor->GetActorLocation());
	}
}

void UDLPathControllerComponent::TrackComplete(UPathControllerTraceAsset* Asset)
{
	bIsInterrupted = false;

	if (CurLaunchParam.Asset->EnableTargetRadiusCheck && CurLaunchParam.TargetActor.IsValid())
	{
		//强行设置到目标位置
		GetOwner()->SetActorLocation(CurLaunchParam.TargetActor->GetActorLocation());
	}

	OnStop();
}

void UDLPathControllerComponent::OnKeyPoint(UPathControllerTraceAsset* Asset, const int32 PointIndex)
{
	OnKeyPointChanged.Broadcast(Asset, PointIndex);
}

void UDLPathControllerComponent::TriggerHoming(UPathControllerTraceAsset* Asset)
{
	OnHomingTriggered.Broadcast(Asset);
}

void UDLPathControllerComponent::OnStop()
{
	bIsPlaying = false;
	bIsStopped = true;
	OnStopped.Broadcast(CurLaunchParam.Asset, bIsInterrupted);
	PCAssetStepper = nullptr;
}

void UDLPathControllerComponent::LaunchToDirection_Implementation(UPathControllerTraceAsset* Asset
	, const FVector RelativeDirection, const float PersistTime/*= -1.0f*/)
{
	CurLaunchParam = FPCLaunchParam();
	if (!Asset)
	{
		Asset = DefaultPCAsset;
	}

	CurLaunchParam.Asset = Asset;
	if (!ensureAlwaysMsgf(!RelativeDirection.IsNearlyZero(), TEXT("方向不能全填0！！！")))
	{
		return;
	}
	CurLaunchParam.RelativeRotation = RelativeDirection.Rotation();


	if (!ensureAlwaysMsgf(CurLaunchParam.Asset, TEXT("请传入轨迹资产！！！")))
	{
		return;
	}
	CurLaunchParam.PersistTime = (PersistTime < 0 ? Asset->Duration : PersistTime);
	Stop();

	Start();

	PCAssetStepper = NewObject<UPCTraceAssetStepperUnlock>();
	FTransform SubObjectInitialTransform = GetOwner()->GetActorTransform();
	SubObjectInitialTransform.SetRotation(RelativeDirection.GetSafeNormal().Rotation().Quaternion());
	PCAssetStepper->Initialize(CurLaunchParam, SubObjectInitialTransform, this, CVarPathControllerDebug->GetBool());
}

void UDLPathControllerComponent::LaunchToTargetWithFreeTrack_Implementation(UPathControllerTraceAsset* Asset,
	AActor* TargetActor, const FVector InitialDirection, const bool bIsHoming, const float PersistTime)
{
	CurLaunchParam = FPCLaunchParam();
	if (!Asset)
	{
		Asset = DefaultPCAsset;
	}

	CurLaunchParam.Asset = Asset;
	CurLaunchParam.InitialDirection = InitialDirection.GetSafeNormal();
	CurLaunchParam.TargetActor = TWeakObjectPtr<AActor>(TargetActor);
	CurLaunchParam.bIsHoming = bIsHoming;

	if (!ensureAlwaysMsgf(CurLaunchParam.Asset, TEXT("请传入轨迹资产！！！")))
	{
		return;
	}
	CurLaunchParam.PersistTime = PersistTime;
	Stop();

	Start();

	PCAssetStepper = NewObject<UPCTraceStepperFreeTrack>();
	FTransform SubObjectInitialTransform = GetOwner()->GetActorTransform();
	SubObjectInitialTransform.SetScale3D(FVector(1.0f));

	PCAssetStepper->Initialize(CurLaunchParam, SubObjectInitialTransform, this, CVarPathControllerDebug->GetBool());
}

void UDLPathControllerComponent::Stop()
{
	if (GetIsStopped() || !bIsPlaying)
	{
		return;
	}

	bIsInterrupted = true;
	OnStop();
}

void UDLPathControllerComponent::Start()
{
	bIsInterrupted = false;
	bIsStopped = false;
	bIsPlaying = true;
}

