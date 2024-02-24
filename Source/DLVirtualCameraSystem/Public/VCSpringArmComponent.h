// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interface/IDLSpringArm.h"
#include "Interface/Interface_VirtualCam.h"
#include "VCSpringArmComponent.generated.h"


/**
 * 
 */
UCLASS(ClassGroup = Camera, meta = (BlueprintSpawnableComponent), hideCategories = (Mobility))
class DLVIRTUALCAMERASYSTEM_API UVCSpringArmComponent
	: public USpringArmComponent
	, public IDLSpringArm
{
	GENERATED_BODY()

public:
	UVCSpringArmComponent();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void AddActiveCam(TScriptInterface<IMyInterface_CameraData> CamDataInterface);

	UFUNCTION(BlueprintCallable)
	void RemoveActiveCam(TScriptInterface<IMyInterface_CameraData> CamDataInterface);

	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVCCamBlendData VCGetLastCamData();
	
	UFUNCTION(BlueprintCallable)
	void LockTarget(USceneComponent* Target);

	UFUNCTION(BlueprintCallable)
	void LockTargetArmLengthOffset(float offset);

public:
	float ANS_ArmLength_CurrentTime = 0;
	float ANS_ArmLength_MaxTime = 0;
	float ANS_ArmLength_ValueOffset = 0;
	virtual void BeginArmLength(float Duration,UAnimSequenceBase* Anim) override;
	virtual void TickArmLength(const UCurveFloat* Curve, float TickTime) override;
	virtual void EndArmLength() override;

private:
	void SortVCams(); 

	void GetIterationDesiredCamData(const FTransform& ActorLocalToWorld, const FTransform& ArmLocalToWorld,
	                               float DeltaTime, FVCCamBlendData& MyCamData);

	virtual void
	UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;

	
	FVCCamBlendData CurrentCamData;
	FVCCamBlendData LastCamData;
	FVector LastCamPos;

	float CurrentArmLengthOffset = 0;
	float TargetArmLengthOffset = 0;
	bool bIsLockMode = false;
	UPROPERTY()
	USceneComponent* LockedComponent;

	UPROPERTY()
	TArray<TScriptInterface<IMyInterface_CameraData>> ActiveCams;

	UPROPERTY()
	TArray<UObject*> VCams;

	// UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	// bool ShowDebug = true;

	//目标点偏移是否使用镜头旋转，如果使用角色会一直在镜头的某一边。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	bool BaseTargetWithArmRot = false;

	//目标点偏移，本地空间。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	FVector BaseTargetOffset;

	//是否启用 目标点的延迟效果。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	bool BaseEnableTargetLag = true;
	
	//目标点的延迟效果靠近时候的跟随速度，越快会越没有延迟。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	float BasePosLagSpeed = 10;
	
	// //目标点的延迟效果靠近时候的跟随速度的曲线，横坐标是当前距离。
	// UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	// UCurveFloat* BaseCurve_PosLagLerp;
	
	//弹簧臂插值的曲线,横坐标是当前值
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	UCurveFloat* BaseCurve_ArmLengthLerp_Near;
	
	//弹簧臂插值的曲线,横坐标是当前值
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	UCurveFloat* BaseCurve_ArmLengthLerp_Far;
	
	//标点的延迟效果的最大距离，超过后不会再延迟了。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	float BaseLagMaxDistance = 200;

	// //是否启用 目标因为移动的延迟修正移动的角度，结果上会出现玩家一直按右键，角色在原地转圈。
	// UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	// bool BaseEnableMoveRotFix = false;
	//
	// //延迟修正移动角度的速度，值越大 转圈越快。
	// UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	// float BaseMoveRotFixSpeed = 10;

	//是否启用 相机旋转的延迟
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	bool BaseEnableArmRotLag = false;

	//相机旋转延迟 的速度
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	float BaseArmRotSpeed = 10;

	//弹簧臂长度
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	float BaseArmLength = 300;

	//弹簧臂长度在锁定下的变化速度
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	float BaseLockArmLengthSpeed = 10;
	
	//弹簧臂最小允许的长度
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	float BaseMinArmLength = 150;
	
	//镜头的视场
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | Base", meta=(AllowPrivateAccess))
	float BaseCamFOV = 90;

	//最小俯仰角
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | PitchAngle", meta=(AllowPrivateAccess))
	float MinPitch = -80;

	//最大俯仰角
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | PitchAngle", meta=(AllowPrivateAccess))
	float MaxPitch = 40;

	// //锁定下高度差缩放值
	// UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | LockMode",
	// 	meta=(AllowPrivateAccess, UIMin=0, UIMax=1, ClampMin=0, ClampMax=1))
	// float LockZSizeScale = 0.5;
	
	//锁定下最小角度
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | LockMode", meta=(AllowPrivateAccess))
	float LockMinPitch = -30;

	//锁定下最大角度
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | LockMode", meta=(AllowPrivateAccess))
	float LockMaxPitch = -10;

	//锁定下的俯仰角的最大偏移，用一个曲线控制，横坐标是角色和目标的XY距离。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | LockMode", meta=(AllowPrivateAccess))
	UCurveFloat* LockMaxPitchCurve;
	
	//锁定下的俯仰角的额外偏移，用一个曲线控制，横坐标是角色和目标的距离。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | LockMode", meta=(AllowPrivateAccess))
	UCurveFloat* LockPitchOffsetCurve;

	//锁定下的左右旋转的额外偏移，用一个曲线控制，横坐标是角色和目标的距离。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | LockMode", meta=(AllowPrivateAccess))
	UCurveFloat* LockYawOffsetCurve;

	//值越大锁定下越看向目标。范围0-1
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | LockMode",
		meta=(AllowPrivateAccess, UIMin=0, UIMax=1, ClampMin=0, ClampMax=1))
	float LookAtRotationLerpVar = 0.5;
	
	//进入锁定状态镜头移动的速度。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | LockMode", meta=(AllowPrivateAccess))
	float LockCameraRotationLagSpeed = 20;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | LockMode", meta=(AllowPrivateAccess))
	UCurveFloat* YawLockCameraRotationLagSpeedCurve;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | LockMode", meta=(AllowPrivateAccess))
	UCurveFloat* PitchLockCameraRotationLagSpeedCurve;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | LockMode", meta=(AllowPrivateAccess))
	UCurveFloat* YawLockAngleScaleCurve;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | LockMode", meta=(AllowPrivateAccess))
	UCurveFloat* PitchLockAngleScaleCurve;
	
	
	//骨骼吸附忽略X
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | SnapSocket", meta=(AllowPrivateAccess))
	bool SnapSocketIgnoreX = false;

	//骨骼吸附忽略Y
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | SnapSocket", meta=(AllowPrivateAccess))
	bool SnapSocketIgnoreY = false;

	//骨骼吸附忽略Z
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VirtualCam | SnapSocket", meta=(AllowPrivateAccess))
	bool SnapSocketIgnoreZ = false;
};
