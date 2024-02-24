// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PathControllerSplineEditor.generated.h"

class UPathControllerTraceAsset;
class UPathControllerAsset;
class USplineComponent;
UCLASS(Abstract)
class DLGAMEPLAYPATHCONTROLLEREDITOR_API APathControllerSplineEditor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APathControllerSplineEditor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	/**
	 * @brief 轨迹曲线
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USplineComponent* Spline = nullptr;

	/**
	 * @brief 重力加速度，单位 cm/s
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float GravityAcceleration = 0.0f;

	/**
	 * @brief 横轴为样条轨迹中的速度 cm/s,纵轴为位移 cm
	 * @warning 如果你的速度过慢，则可能会导致无法飞到预定目标
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UCurveFloat* CustomSpeedCurve = nullptr;

	/**
	 * @brief 跟踪修正权重
	 */
	UPROPERTY(EditAnywhere, Category = PCAsset, meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 1.0f, UIMax = 1.0f))
		float FixWeight = 0.5f;

	/**
	 * @brief 是否跟随旋转
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool AllowRotate = true;

	/**
	 * @brief 留给一定命中去做表现的时间，这个阶段的触发时间为“飞行轨迹持续时间 - HomingTriggerTime”
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float HomingTriggerTime = 0.2f;

	/**
	 * @brief 一定命中去做表现过程中的速度叠加幅度，该叠加是对当前速度进行叠加,并且每次迭代都会再之前的基础上进行叠加
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float HomingFixAmplitude = 0.2f;

	/**
	 * @brief 迭代频率,每多少帧迭代一次
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int IterateDelay = 1;

	/** Number for tweaking playback rate of this PCAsset globally. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PCAsset)
		float RateScale;

	/**
	 * @brief 子物体飞行到目标Actor的 “胶囊体半径 + TargetRadius” 范围内 强制切换飞行阶段,单位 cm
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float TargetRadius = 50.0f;
public:
	UFUNCTION(BlueprintCallable, Category = "PathController | Tools")
		static void CopyPathControllerAsset(const APathControllerSplineEditor* Source, UPathControllerTraceAsset* Target);
};
