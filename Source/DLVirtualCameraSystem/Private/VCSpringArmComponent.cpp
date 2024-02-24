// Fill out your copyright notice in the Description page of Project Settings.


#include "VCSpringArmComponent.h"
#include "CollisionQueryParams.h"
#include "WorldCollision.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "VirtualCam.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

//PRAGMA_DISABLE_OPTIMIZATION


#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
int32 DebugVCPos = 0;
static FAutoConsoleVariableRef CVarDebugVCPos(
TEXT("VC.DebugVCPos"),
	DebugVCPos,
	TEXT("Show debug info for VC"),
	ECVF_Default
	);

int32 DebugVCLerpAngle = 0;
static FAutoConsoleVariableRef CVarDebugVCLerpAngle(
TEXT("VC.DebugVCLerpAngle"),
	DebugVCLerpAngle,
	TEXT("Show debug info for VC"),
	ECVF_Default
	);

int32 DebugVCLerpPos = 0;
static FAutoConsoleVariableRef CVarDebugVCLerpPos(
TEXT("VC.DebugVCLerpPos"),
	DebugVCLerpPos,
	TEXT("Show debug info for VC"),
	ECVF_Default
	);

#endif


UVCSpringArmComponent::UVCSpringArmComponent()
{
	bUsePawnControlRotation = true;
}

void UVCSpringArmComponent::BeginPlay()
{
	Super::BeginPlay();
	TArray<UActorComponent*> Comps = GetOwner()->GetComponentsByInterface(UMyInterface_CameraData::StaticClass());
	for (const auto Comp : Comps)
	{
		VCams.AddUnique(Comp);
		IMyInterface_CameraData::Execute_RegisterArmComponent(Comp, this);
	}
	SortVCams();

	LastCamData.LookAtTarget.Value = GetComponentLocation() + BaseTargetOffset;
	LastCamData.ArmLength.Value = BaseArmLength;
	LastCamData.ArmRot.Value = RotToVec(GetTargetRotation());
	LastCamPos = LastCamData.LookAtTarget.Value - GetTargetRotation().Vector() * LastCamData.ArmLength.Value;

	//修改镜头的FOV
	if (GetChildComponent(0))
	{
		UCameraComponent* Cam = Cast<UCameraComponent>(GetChildComponent(0));
		if (Cam) Cam->SetFieldOfView(BaseCamFOV);
	}
}


void UVCSpringArmComponent::AddActiveCam(TScriptInterface<IMyInterface_CameraData> CamDataInterface)
{
	if (!IsValid(CamDataInterface.GetObject())) return;

	VCams.AddUnique(CamDataInterface.GetObject());
	SortVCams();
}

void UVCSpringArmComponent::RemoveActiveCam(TScriptInterface<IMyInterface_CameraData> CamDataInterface)
{
	if (!IsValid(CamDataInterface.GetObject())) return;
	VCams.Remove(CamDataInterface.GetObject());
	SortVCams();
}

FVCCamBlendData UVCSpringArmComponent::VCGetLastCamData()
{
	return LastCamData;
}

void UVCSpringArmComponent::LockTarget(USceneComponent* Target)
{
	LockedComponent = Target;
	bIsLockMode = IsValid(Target);
}

void UVCSpringArmComponent::LockTargetArmLengthOffset(float offset)
{
	TargetArmLengthOffset = offset;
}

void UVCSpringArmComponent::BeginArmLength(float Duration,UAnimSequenceBase* Anim)
{
	ANS_ArmLength_MaxTime = Duration;
	ANS_ArmLength_CurrentTime = 0;
	ANS_ArmLength_ValueOffset = 0;
#if UE_SERVER
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("该虚拟相机功能不应该在服务器运行，请取消勾选服务器：%s"),*Anim->GetFullName());
	}
#endif
}

void UVCSpringArmComponent::TickArmLength(const UCurveFloat* Curve, float TickTime)
{
	ANS_ArmLength_CurrentTime+= TickTime;
	ANS_ArmLength_ValueOffset = Curve->GetFloatValue(ANS_ArmLength_CurrentTime/ANS_ArmLength_MaxTime);
}


void UVCSpringArmComponent::EndArmLength()
{
	ANS_ArmLength_ValueOffset = 0;
}

void UVCSpringArmComponent::SortVCams()
{
	VCams.Sort([](UObject& LHS, UObject& RHS)
	{
		return IMyInterface_CameraData::Execute_GetOrder(&LHS) < IMyInterface_CameraData::Execute_GetOrder(&RHS);
	});
}

void UVCSpringArmComponent::GetIterationDesiredCamData(const FTransform& ActorLocalToWorld,
                                                       const FTransform& ArmLocalToWorld,
                                                       const float DeltaTime,
                                                       FVCCamBlendData& MyCamData)
{
	FVector NewTargetPos = GetComponentLocation();
	if (GetAttachSocketName() != NAME_None)
	{
		const FVector ParentPos = GetAttachParent()->GetComponentLocation();
		FVector LocPos = ActorLocalToWorld.InverseTransformPosition(NewTargetPos);
		const FVector LocParentPos = ActorLocalToWorld.InverseTransformPosition(ParentPos);

		if (SnapSocketIgnoreX)LocPos.X = LocParentPos.X;
		if (SnapSocketIgnoreY)LocPos.Y = LocParentPos.Y;
		if (SnapSocketIgnoreZ)LocPos.Z = LocParentPos.Z;

		NewTargetPos = ActorLocalToWorld.TransformPosition(LocPos);
	}
	if (BaseTargetWithArmRot)
	{
		MyCamData.LookAtTarget.Value = NewTargetPos + ArmLocalToWorld.TransformVector(BaseTargetOffset);
	}
	else
	{
		MyCamData.LookAtTarget.Value = NewTargetPos + ActorLocalToWorld.TransformVector(BaseTargetOffset);
	}

	MyCamData.EnableTargetLag = BaseEnableTargetLag ? EVCBlendBool::BoolTrue : EVCBlendBool::BoolFalse;
	MyCamData.TargetLagSpeed.Value = BasePosLagSpeed;
	MyCamData.LagMaxDistance.Value = BaseLagMaxDistance;

	const FRotator Rot = GetTargetRotation();
	MyCamData.ArmRot.Value = FVector(Rot.Roll, Rot.Pitch, Rot.Yaw);
	// MyCamData.EnableMoveRotFix = BaseEnableMoveRotFix ? EVCBlendBool::BoolTrue : EVCBlendBool::BoolFalse;
	// MyCamData.MoveRotFixSpeed.Value = BaseMoveRotFixSpeed;
	MyCamData.EnableArmRotLag = BaseEnableArmRotLag ? EVCBlendBool::BoolTrue : EVCBlendBool::BoolFalse;
	MyCamData.ArmRotLagSpeed.Value = BaseArmRotSpeed;

	MyCamData.ArmLength.Value = BaseArmLength;
	MyCamData.CamFOV.Value = BaseCamFOV;


	FTransform LTTrans = FTransform::Identity;
	if (IsValid(LockedComponent))
	{
		LTTrans = LockedComponent->GetComponentToWorld();
	}
	for (const auto VCam : VCams)
	{
		IMyInterface_CameraData::Execute_TryFixVCamData(VCam, ActorLocalToWorld, ArmLocalToWorld, LTTrans,DeltaTime, MyCamData);
	}
}

void UVCSpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag,
                                                     float DeltaTime)
{
	bool bSkip = false;
	if (GetOwnerRole() != ENetRole::ROLE_AutonomousProxy) bSkip = true;
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (GetWorld()->GetNetMode() == ENetMode::NM_Standalone) bSkip = false;
#endif
	if (bSkip) return;;
	
	//迭代当前虚拟相机队列
	FTransform ArmLocalToWorld = GetOwner()->GetActorTransform();
	ArmLocalToWorld.SetRotation(GetTargetRotation().Quaternion());
	GetIterationDesiredCamData(GetOwner()->GetActorTransform(), ArmLocalToWorld,DeltaTime, CurrentCamData);

	//控制器的旋转角度
	FRotator CurrentTempRot = FRotator(CurrentCamData.ArmRot.Value.Y, CurrentCamData.ArmRot.Value.Z, 0);
	FRotator LastRot = VecToRot(LastCamData.ArmRot.Value);

	//是否是锁定模式
	if (!IsValid(LockedComponent)) bIsLockMode = false;
	if (bIsLockMode)
	{
		if ((LockedComponent->K2_GetComponentLocation() - GetOwner()->GetActorLocation()).Size2D() > 10)
		{
			//看向的点
			const FVector LerpedTarget = FMath::Lerp(GetOwner()->GetActorLocation(),
			                                         LockedComponent->K2_GetComponentLocation(), LookAtRotationLerpVar);
			//计算出来的角度
			FRotator TargetArmRot = UKismetMathLibrary::FindLookAtRotation(
				GetOwner()->GetActorLocation(), LockedComponent->K2_GetComponentLocation());

			//缩放计算出来的俯仰角
			//TargetArmRot.Pitch *= LockZSizeScale;
			
			//两点的距离
			float Dist = (GetOwner()->GetActorLocation() - LockedComponent->GetOwner()->GetActorLocation()).Size2D();

			//曲线处理插值方式
			float YawLockCameraRotationLagSpeed = LockCameraRotationLagSpeed;
			float PitchLockCameraRotationLagSpeed = LockCameraRotationLagSpeed;
			FRotator CacheTargetArmRot = TargetArmRot;
			if (LockYawOffsetCurve) CacheTargetArmRot.Yaw += LockYawOffsetCurve->GetFloatValue(Dist);
			if (LockPitchOffsetCurve) CacheTargetArmRot.Pitch += LockPitchOffsetCurve->GetFloatValue(Dist);
			CacheTargetArmRot.Pitch = FMath::ClampAngle(CacheTargetArmRot.Pitch, LockMinPitch, LockMaxPitch);
			
			FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(CacheTargetArmRot,LastRot);
			float YawValue = FMath::Abs(DeltaRot.Yaw);
			float PitchValue = FMath::Abs(DeltaRot.Pitch);
			if (YawLockAngleScaleCurve) YawValue *= YawLockAngleScaleCurve->GetFloatValue(Dist);
			if (PitchLockAngleScaleCurve) PitchValue *= PitchLockAngleScaleCurve->GetFloatValue(Dist);

			if (YawLockCameraRotationLagSpeedCurve) YawLockCameraRotationLagSpeed = YawLockCameraRotationLagSpeedCurve->GetFloatValue(YawValue);
			if (PitchLockCameraRotationLagSpeedCurve) PitchLockCameraRotationLagSpeed = PitchLockCameraRotationLagSpeedCurve->GetFloatValue(PitchValue);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			if (DebugVCLerpAngle)
			{
				UE_LOG(LogTemp, Log, TEXT("RotYawBase --- Angle: %f"), DeltaRot.Yaw);
				UE_LOG(LogTemp, Log, TEXT("RotYaw --- value: %f"), YawValue);
				UE_LOG(LogTemp, Log, TEXT("RotYaw --- Speed: %f"), YawLockCameraRotationLagSpeed);
				UE_LOG(LogTemp, Log, TEXT("RotPitchBase --- Angle: %f"), DeltaRot.Pitch);
				UE_LOG(LogTemp, Log, TEXT("RotPitch --- value: %f"), PitchValue);
				UE_LOG(LogTemp, Log, TEXT("RotPitch --- Speed: %f"), PitchLockCameraRotationLagSpeed);
			}
#endif
			
			//曲线偏移
			if (LockYawOffsetCurve) TargetArmRot.Yaw += LockYawOffsetCurve->GetFloatValue(Dist);
			if (LockPitchOffsetCurve) TargetArmRot.Pitch += LockPitchOffsetCurve->GetFloatValue(Dist);
			
			FRotator TempRotYaw = FRotator(FMath::QInterpTo(FQuat(LastRot), FQuat(TargetArmRot), DeltaTime,
														 YawLockCameraRotationLagSpeed));
			FRotator TempRotPitch = FRotator(FMath::QInterpTo(FQuat(LastRot), FQuat(TargetArmRot), DeltaTime,
														 PitchLockCameraRotationLagSpeed));
			FRotator TempRot = TempRotYaw;
			TempRot.Pitch = TempRotPitch.Pitch;
			CurrentTempRot = TempRot;
			CurrentTempRot.Roll = 0;
			if (LockMaxPitchCurve)
			{
				LockMaxPitch = LockMaxPitchCurve->GetFloatValue(Dist);
			}
			CurrentTempRot.Pitch = FMath::ClampAngle(CurrentTempRot.Pitch, LockMinPitch, LockMaxPitch);
			
		}
	}
	else
	{
		// //做相机角度的跟随 - 移动修正镜头角度
		// if (CurrentCamData.EnableMoveRotFix == EVCBlendBool::BoolTrue)
		// {
		// 	//计算出来的一个旋转角度
		// 	FRotator TargetArmRot = UKismetMathLibrary::FindLookAtRotation(
		// 		LastCamPos, CurrentCamData.LookAtTarget.Value);
		// 	FRotator TempRot = FRotator(FMath::QInterpTo(FQuat(CurrentTempRot), FQuat(TargetArmRot), DeltaTime,
		// 	                                             CurrentCamData.MoveRotFixSpeed.Value));
		// 	TempRot.Pitch = CurrentTempRot.Pitch;
		// 	CurrentTempRot = TempRot;
		// }

		//相机镜头延迟靠近目标角度
		if (CurrentCamData.EnableArmRotLag == EVCBlendBool::BoolTrue)
		{
			FRotator TempRot = FRotator(FMath::QInterpTo(FQuat(LastRot), FQuat(CurrentTempRot), DeltaTime,
			                                             CurrentCamData.ArmRotLagSpeed.Value));
			CurrentTempRot = TempRot;
		}
		CurrentTempRot.Roll = 0;
		CurrentTempRot.Pitch = FMath::ClampAngle(CurrentTempRot.Pitch, MinPitch, MaxPitch);
	}

	//把新的旋转值给到控制器
	auto* pc = Cast<APawn>(GetOwner())->GetController();
	if (Cast<APlayerController>(pc))
	{
		pc->SetControlRotation(CurrentTempRot);
	}

	//镜头的看向目标点
	FVector TempLookAtTarget = CurrentCamData.LookAtTarget.Value;


	//摇臂的长度
	float TempArmLength = CurrentCamData.ArmLength.Value;
	if (CurrentCamData.EnableTargetLag == EVCBlendBool::BoolTrue)
	{
		float Lagspeed = CurrentCamData.TargetLagSpeed.Value;

// 		if (BaseCurve_PosLagLerp) 
// 		{
// 			float dist = FVector::Distance(LastCamData.LookAtTarget.Value,CurrentCamData.LookAtTarget.Value);
// 			Lagspeed = BaseCurve_PosLagLerp->GetFloatValue(dist);
// 			
// #if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
// 			if (DebugVCLerpPos)
// 			{
// 				UE_LOG(LogTemp, Log, TEXT("Pos --- dist: %f"), dist);
// 				UE_LOG(LogTemp, Log, TEXT("Pos --- speed: %f"), Lagspeed);
// 			}
// #endif
// 		}

		TempLookAtTarget = FMath::VInterpTo(LastCamData.LookAtTarget.Value, CurrentCamData.LookAtTarget.Value, DeltaTime,
		                                    Lagspeed);

		float ArmLengthLagSpeed = CurrentCamData.TargetLagSpeed.Value;
		if (BaseCurve_ArmLengthLerp_Near && BaseCurve_ArmLengthLerp_Far)
		{
			if (CurrentCamData.ArmLength.Value < LastCamData.ArmLength.Value)
			{
				ArmLengthLagSpeed = BaseCurve_ArmLengthLerp_Near->GetFloatValue(LastCamData.ArmLength.Value);
			}
			else
			{
				ArmLengthLagSpeed = BaseCurve_ArmLengthLerp_Far->GetFloatValue(LastCamData.ArmLength.Value);
			}
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			if (DebugVCLerpPos)
			{
				UE_LOG(LogTemp, Log, TEXT("ArmLength --- dist: %f"), LastCamData.ArmLength.Value);
				UE_LOG(LogTemp, Log, TEXT("ArmLength --- speed: %f"), ArmLengthLagSpeed);
				if(CurrentCamData.ArmLength.Value < LastCamData.ArmLength.Value)
				{
					UE_LOG(LogTemp, Log, TEXT("ArmLength is Near"));
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("ArmLength is Far"));
				}
			}
#endif
		}
		TempArmLength = FMath::FInterpConstantTo(LastCamData.ArmLength.Value, CurrentCamData.ArmLength.Value, DeltaTime,
		                                 ArmLengthLagSpeed);
		
		if (CurrentCamData.LagMaxDistance.Value > 0.f)
		{

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			bool bClampedDist = false;
#endif
			const FVector FromOrigin = TempLookAtTarget - CurrentCamData.LookAtTarget.Value;
			if (FromOrigin.SizeSquared() > FMath::Square(CurrentCamData.LagMaxDistance.Value))
			{
				TempLookAtTarget = CurrentCamData.LookAtTarget.Value + FromOrigin.GetClampedToMaxSize(
					CurrentCamData.LagMaxDistance.Value);
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
				bClampedDist = true;
#endif
			}
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			if (DebugVCPos)
			{
				DrawDebugSphere(GetWorld(), CurrentCamData.LookAtTarget.Value, 5.f, 8, FColor::Green);
				DrawDebugSphere(GetWorld(), TempLookAtTarget, 5.f, 8, FColor::Yellow);
				if (IsValid(LockedComponent))
				{
					DrawDebugSphere(GetWorld(), LockedComponent->K2_GetComponentLocation(), 50.f, 8, FColor::Blue);
				}

				const FVector ToOrigin = CurrentCamData.LookAtTarget.Value - TempLookAtTarget;
				DrawDebugDirectionalArrow(GetWorld(), TempLookAtTarget, TempLookAtTarget + ToOrigin * 0.5f, 7.5f,
										  bClampedDist ? FColor::Red : FColor::Green);
				DrawDebugDirectionalArrow(GetWorld(), TempLookAtTarget + ToOrigin * 0.5f, CurrentCamData.LookAtTarget.Value,
										  7.5f,
										  bClampedDist ? FColor::Red : FColor::Green);
			}
#endif
		}
	}

	CurrentArmLengthOffset = FMath::FInterpTo(CurrentArmLengthOffset,TargetArmLengthOffset,DeltaTime,BaseLockArmLengthSpeed);
	
	//计算出相机的位置
	FVector TempCamPos = TempLookAtTarget - CurrentTempRot.Vector() * (TempArmLength + CurrentArmLengthOffset + ANS_ArmLength_ValueOffset);

	//做镜头的碰撞检测
	FVector ResultLoc;
	if (bDoTrace && (TempArmLength != 0.0f))
	{
		bIsCameraFixed = true;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, GetOwner());

		FHitResult Result;
		FVector FromPos = CurrentCamData.LookAtTarget.Value + (TempCamPos - CurrentCamData.LookAtTarget.Value).
			GetSafeNormal() * BaseMinArmLength;
		GetWorld()->SweepSingleByChannel(Result, FromPos, TempCamPos, FQuat::Identity,
		                                 ProbeChannel,
		                                 FCollisionShape::MakeSphere(ProbeSize), QueryParams);

		UnfixedCameraPosition = TempCamPos;

		ResultLoc = BlendLocations(TempCamPos, Result.Location, Result.bBlockingHit, DeltaTime);

		if (ResultLoc == TempCamPos)
		{
			bIsCameraFixed = false;
		}
	}
	else
	{
		ResultLoc = TempCamPos;
		bIsCameraFixed = false;
		UnfixedCameraPosition = ResultLoc;
	}

	LastCamPos = ResultLoc;

	//构造相机的矩阵
	FTransform WorldCamTM(CurrentTempRot, ResultLoc);
	//转换到组件空间的矩阵
	FTransform RelCamTM = WorldCamTM.GetRelativeTransform(GetComponentTransform());

	//更新子对象的位置旋转
	RelativeSocketLocation = RelCamTM.GetLocation();
	RelativeSocketRotation = RelCamTM.GetRotation();
	UpdateChildTransforms();


	//修改镜头的FOV
	if (GetChildComponent(0))
	{
		UCameraComponent* Cam = Cast<UCameraComponent>(GetChildComponent(0));
		if (Cam) Cam->SetFieldOfView(CurrentCamData.CamFOV.Value);
	}


	//缓存给下一帧使用
	LastCamData = CurrentCamData;
	LastCamData.LookAtTarget.Value = TempLookAtTarget;
	LastCamData.ArmLength.Value = TempArmLength;
	LastCamData.ArmRot.Value = RotToVec(CurrentTempRot);
}

//PRAGMA_ENABLE_OPTIMIZATION
