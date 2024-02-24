// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PathControllerDef.h"
#include "Components/ActorComponent.h"
#include "DLPathControllerComponent.generated.h"

class IPCStepperInterface;
class UPathControllerTraceAsset;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPCTrackStopped, UPathControllerTraceAsset*, Asset, const bool, bIsInterruped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPCKeyPointNotify, UPathControllerTraceAsset*, Asset, const int32, PointIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPCSimpleEvent, UPathControllerTraceAsset*, Asset);


UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class DLGAMEPLAYPATHCONTROLLER_API UDLPathControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDLPathControllerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
public:

	/**
	 * @brief 朝目标Actor发射子物体
	 * @param Asset 轨迹资产
	 * @param TargetActor 目标Actor
	 * @param bIsTrack 是否跟踪
	 * @param bIsHoming 一定命中
	 * @param PersistTime 轨迹最大飞行持续时间，如果该值为-1.0则表示使用使用轨迹资产中的Duration
	 * @param InitialDirection 初始方向
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (AdvanceDisplay = "bIsHoming | PersistTime"))
		void LaunchToTarget(UPathControllerTraceAsset* Asset, AActor* TargetActor, const bool bIsTrack = false, const bool bIsHoming = false
			, const float PersistTime = -1.0f, const FVector InitialDirection = FVector(1.0f, 0.0f, 0.0f));

	/**
	 * @brief 朝指定位置发射子物体
	 * @param Asset 轨迹资产
	 * @param TargetLocation 目标位置
	 * @param PersistTime 轨迹最大飞行持续时间，如果该值为-1.0则表示使用使用轨迹资产中的Duration
	 * @param InitialDirection 初始方向
	 * @param MaxDistance 最大飞行距离，小于代表使用轨迹的距离最为最大距离
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void LaunchToLocation(UPathControllerTraceAsset* Asset, FVector TargetLocation, const float PersistTime = -1.0f, const FVector InitialDirection = FVector(1.0f, 0.0f, 0.0f), float MaxDistance = -1.0f);

	/**
	 * @brief 朝指定方向发射子物体
	 * @param Asset 轨迹资产
	 * @param RelativeDirection 子物体前向与目标位置的方向
	 * @param PersistTime 轨迹最大飞行持续时间，如果该值为-1.0则表示使用使用轨迹资产中的Duration
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void LaunchToDirection(UPathControllerTraceAsset* Asset, const FVector RelativeDirection = FVector(1.0f, 0.0f, 0.0f), const float PersistTime = -1.0f);

	/**
	 * @brief 朝指定目标进行自由跟踪
	 * @param Asset 轨迹资产
	 * @param TargetActor 目标Actor
	 * @param bIsHoming 一定命中
	 * @param InitialDirection 初始方向
	 * @param PersistTime 轨迹最大飞行持续时间，如果该值为-1.0则表示使用使用轨迹资产中的Duration
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void LaunchToTargetWithFreeTrack(UPathControllerTraceAsset* Asset, AActor* TargetActor, const FVector InitialDirection = FVector(1.0f, 0.0f, 0.0f), const bool bIsHoming = false, const float PersistTime = -1.0f);

	UFUNCTION(BlueprintCallable)
		void Stop();

	UFUNCTION(BlueprintCallable)
		void Start();

public:
	UFUNCTION(BlueprintCallable)
		void TrackComplete(UPathControllerTraceAsset* Asset);

	UFUNCTION(BlueprintCallable)
		void OnKeyPoint(UPathControllerTraceAsset* Asset, const int32 PointIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetIsStopped() const { return bIsStopped; };

	UFUNCTION(BlueprintCallable)
		void TriggerHoming(UPathControllerTraceAsset* Asset);

#if WITH_EDITOR
	FORCEINLINE float GetPreviewDeltaTime() const { return PreviewDeltaSeconds; }
#endif

	bool ShowDebugLine = false;
protected:
	void OnStop();

public:

	/**
	 * @brief 轨迹停止时调用，如果是外部调用导致的，则bIsInterruped为true，轨迹自己停止的为false
	 */
	UPROPERTY(BlueprintAssignable)
		FPCTrackStopped OnStopped;

	/**
	 * @brief 在composite中，Asset指的是构成Composite的PCAsset片段；在PCAsset中，指的是PCAsset它自身。
	 */
	UPROPERTY(BlueprintAssignable)
		FPCKeyPointNotify OnKeyPointChanged;

	/**
	 * @brief 触发一定命中表现
	 */
	UPROPERTY(BlueprintAssignable)
		FPCSimpleEvent OnHomingTriggered;

	/**
	 * @brief 到达目标Actor范围内
	 */
	UPROPERTY(BlueprintAssignable)
		FPCSimpleEvent OnArriveTargetRadius;

protected:
	UPROPERTY(BlueprintReadWrite)
		FPCLaunchParam CurLaunchParam;

	UPROPERTY(BlueprintReadWrite)
		class UPCTraceStepperBase* PCAssetStepper = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UPathControllerTraceAsset* DefaultPCAsset = nullptr;
private:
	bool bIsInterrupted = false;
	bool bIsStopped = false;
	bool bIsPlaying = false;


private:
#if WITH_EDITORONLY_DATA
	float PreviewDeltaSeconds = 0.0f;
#endif

};
