// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "PathControllerTraceAsset.generated.h"

UENUM(BlueprintType)
enum class EPathControllerLaunchType : uint8
{
	//到目标
	LaunchToTarget,
	//到坐标
	LaunchToLocation,
	//延方向
	LaunchToDirection,
	//朝目标进行自由修正
	LaunchToTargetWithFreeTrack
};

/**
 *
 */
UCLASS(BlueprintType)
class DLGAMEPLAYPATHCONTROLLER_API UPathControllerTraceAsset : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY()
		FSplineCurves SplineCurves;

	/** Number of steps per spline segment to place in the reparameterization table */
	int32 ReparamStepsPerSegment;

	/** Whether the endpoints of the spline are considered stationary when traversing the spline at non-constant velocity.  Essentially this sets the endpoints' tangents to zero vectors. */
	bool bStationaryEndpoints;

	/** Default up vector in local space to be used when calculating transforms along the spline */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Spline)
		FVector DefaultUpVector;

	/**
	 * @brief 横轴为进度[0,1],纵轴为速度 cm/s
	 * @warning 如果你的速度过慢，则可能会导致无法飞到预定目标
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		UCurveFloat* CustomSpeedCurve = nullptr;

	/**
	 * @brief 轨迹飞行时间的标准时长
	 * @warning 如果当前时间超过标准时长，将按照SpeedOverDuration的速度继续保持最后的朝向继续往前飞行，直到被外部停止或子物体被销毁
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.1f, UIMin = 0.1f, ClampMax = 999.0f, UIMax = 999.0f))
		float Duration = 1.0f;

	/**
	 * @brief 当前时间超过标准时长时的速度，将按照SpeedOverDuration的速度继续保持最后的朝向继续往前飞行，直到被外部停止或子物体被销毁
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 100.0f, UIMin = 100.0f, ClampMax = 99999999.0f, UIMax = 99999999.0f))
		float SpeedOverDuration = 100.0f;

	/** Number for tweaking playback rate of this PCAsset globally. */
	float RateScale;

	/**
	 * @brief 是否跟随旋转
	 * @warning 应用轨迹修正之后，跟随旋转会失效
	 */
	// UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	// 	bool AllowRotate = true;

	/**
	* @brief 重力加速度，单位 cm/s
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FVector SubObjectForword = FVector(1,0,0);


	/**
	 * @brief 重力加速度，单位 cm/s
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float GravityAcceleration = 0.0f;

	/**
	 * @brief 迭代频率,没多少帧迭代一次
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (ClampMin = 1, UIMin = 1, ClampMax = 250, UIMax = 250), Category = "性能")
		int32 IterateFrequency = 1;

	/**
	 * @brief 轨迹修正权重
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "轨迹修正"
		, meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 360.0f, UIMax = 360.0f))
		float FixAngle = 6.0f;

	/**
	 * @brief 轨迹修正最大时长
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "轨迹修正"
		, meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 9999.0f, UIMax = 9999.0f))
		float MaxFixDuration = 5.0f;

	/**
	 * @brief 轨迹修正频率，每迭代多少次进行一次修正,也即是说修正的周期为“IterateFrequency * TraceFixFrequency”
	 * @bug 有bug，暂时不开放使用
	 */
	UPROPERTY(BlueprintReadWrite, Category = "轨迹修正", meta = (ClampMin = 1, UIMin = 1, ClampMax = 99, UIMax = 99))
		int32 TraceFixFrequency = 1;

	/**
	 * @brief 触发一定命中去做表现的时间，该时间不得大于 飞行轨迹持续时间
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "轨迹修正 | 一定命中")
		float HomingTriggerTime = 0.8f;

	/**
	 * @brief 一定命中去做表现过程中的速度叠加幅度，该叠加是对当前速度进行叠加
	 * @details  触发该阶段时的速度 * (在该阶段中已运行的帧数 * HomingFixAmplitude + 1)
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "轨迹修正 | 一定命中")
		float HomingFixAmplitude = 0.5f;

	/**
	 * @brief 如果打开此选项，在子物体到达目标半径范围内之后会进入一个新的飞行阶段
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "轨迹修正 | 进入目标范围处理")
		bool EnableTargetRadiusCheck = false;

	/**
	 * @brief 子物体飞行到目标Actor的 “胶囊体半径 + TargetRadius” 范围内 强制切换飞行阶段,单位 cm
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (EditCondition = "EnableTargetRadiusCheck"), Category = "轨迹修正 | 进入目标范围处理")
		float TargetRadius = 50.0f;

	/**
	 * @brief 子物体到达目标范围内后的 “基于到达时速度” 的速度倍率
	 * @details  触发该阶段时的速度 * SpeedRateAfterArrivedTarget
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (ClampMin = 1.0f, UIMin = 1.0f, ClampMax = 100.0f, UIMax = 100.0f), meta = (EditCondition = "EnableTargetRadiusCheck"), Category = "轨迹修正 | 进入目标范围处理")
		float SpeedRateAfterArrivedTarget = 1.0f;

	//模拟调用的函数类型
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "模拟参数")
		EPathControllerLaunchType FunctionType = EPathControllerLaunchType::LaunchToTarget;

	//是否跟踪
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "模拟参数",
		meta = (EditInline, EditCondition = "FunctionType == EPathControllerLaunchType::LaunchToTarget", EditConditionHides))
		bool bIsTrack;

	//一定命中
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "模拟参数",
		meta = (EditInline, EditCondition = "bIsTrack || FunctionType == EPathControllerLaunchType::LaunchToTargetWithFreeTrack", EditConditionHides))
		bool bIsHoming;

	//目标位置
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "模拟参数",
		meta = (EditInline, EditCondition = "FunctionType == EPathControllerLaunchType::LaunchToLocation", EditConditionHides))
		FVector TargetLocation;

	//相对方向
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "模拟参数")
		FVector RelativeDirection = FVector(1.0f, 0.0f, 0.0f);

	//最大距离
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "模拟参数",
	meta = (EditInline, EditCondition = "FunctionType == EPathControllerLaunchType::LaunchToLocation", EditConditionHides))
		float MaxDistance = -1.0f;

	//沿轨迹运动的模型
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "运动物体", meta = (AllowedClasses = "StaticMesh"))
		TSoftObjectPtr<UStreamableRenderAsset> PreviewMesh;

	//目标物体
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "目标物体")
		TSubclassOf<AActor> TargetActor;

private:
	/**
	 * Whether the spline is to be considered as a closed loop.
	 * Use SetClosedLoop() to set this property, and IsClosedLoop() to read it.
	 */

	bool bClosedLoop;

	bool bLoopPositionOverride;

	float LoopPosition;

public:
	static UPathControllerTraceAsset* CopyAsset(const UPathControllerTraceAsset* OtherAsset);

public:
	void UpdateSpline();

	FInterpCurveVector& GetSplinePointsPosition() { return SplineCurves.Position; }
	const FInterpCurveVector& GetSplinePointsPosition() const { return SplineCurves.Position; }
	FInterpCurveQuat& GetSplinePointsRotation() { return SplineCurves.Rotation; }
	const FInterpCurveQuat& GetSplinePointsRotation() const { return SplineCurves.Rotation; }
	FInterpCurveVector& GetSplinePointsScale() { return SplineCurves.Scale; }
	const FInterpCurveVector& GetSplinePointsScale() const { return SplineCurves.Scale; }

	void SetClosedLoop(const bool InClosedLoop) { bClosedLoop = InClosedLoop; }


	/**
	 * @brief 获取轨迹的总位移
	 * @return
	 */
	UFUNCTION(BlueprintCallable)
		float GetSplineDisplacement() const;

	/**
	 * @brief 获取轨迹的总位移的平方
	 * @return
	 */
	UFUNCTION(BlueprintCallable)
		float GetSplineDisplacementSquared() const;

	/** Get location along spline at the provided input key value */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetLocationAtSplineInputKey(float InKey) const;

	/** Get tangent along spline at the provided input key value */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetTangentAtSplineInputKey(float InKey) const;

	/** Get unit direction along spline at the provided input key value */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetDirectionAtSplineInputKey(float InKey) const;

	/** Get rotator corresponding to rotation along spline at the provided input key value */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FRotator GetRotationAtSplineInputKey(float InKey) const;

	/** Get quaternion corresponding to rotation along spline at the provided input key value */
	FQuat GetQuaternionAtSplineInputKey(float InKey) const;

	/** Get up vector at the provided input key value */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetUpVectorAtSplineInputKey(float InKey) const;

	/** Get right vector at the provided input key value */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetRightVectorAtSplineInputKey(float InKey) const;

	/** Get transform at the provided input key value */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FTransform GetTransformAtSplineInputKey(float InKey, bool bUseScale = false) const;

	/** Get roll in degrees at the provided input key value */
	UFUNCTION(BlueprintCallable, Category = Spline)
		float GetRollAtSplineInputKey(float InKey) const;

	/** Get scale at the provided input key value */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetScaleAtSplineInputKey(float InKey) const;

	/** Get distance along the spline at the provided input key value */
	UFUNCTION(BlueprintCallable, Category = Spline)
		float GetDistanceAlongSplineAtSplineInputKey(float InKey) const;
public:
	/** Specify whether the spline is a closed loop or not, and if so, the input key corresponding to the loop point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void SetClosedLoopAtPosition(bool bInClosedLoop, float Key, bool bUpdateSpline = true);

	/** Clears all the points in the spline */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void ClearSplinePoints(bool bUpdateSpline = true);

	/** Adds an FSplinePoint to the spline. This contains its input key, position, tangent, rotation and scale. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void AddPoint(const FSplinePoint& Point, bool bUpdateSpline = true);

	/** Adds an array of FSplinePoints to the spline. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void AddPoints(const TArray<FSplinePoint>& Points, bool bUpdateSpline = true);

	/** Adds a point to the spline */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void AddSplinePoint(const FVector& Position, bool bUpdateSpline = true);

	/** Adds a point to the spline at the specified index */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void AddSplinePointAtIndex(const FVector& Position, int32 Index, bool bUpdateSpline = true);

	/** Removes point at specified index from the spline */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void RemoveSplinePoint(int32 Index, bool bUpdateSpline = true);

	/** Sets the spline to an array of points */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void SetSplinePoints(const TArray<FVector>& Points, bool bUpdateSpline = true);

	/** Move an existing point to a new location */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void SetLocationAtSplinePoint(int32 PointIndex, const FVector& InLocation, bool bUpdateSpline = true);


	/** Specify the tangent at a given spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void SetTangentAtSplinePoint(int32 PointIndex, const FVector& InTangent, bool bUpdateSpline = true);

	/** Specify the tangents at a given spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void SetTangentsAtSplinePoint(int32 PointIndex, const FVector& InArriveTangent, const FVector& InLeaveTangent, bool bUpdateSpline = true);

	/** Specify the up vector at a given spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void SetUpVectorAtSplinePoint(int32 PointIndex, const FVector& InUpVector, bool bUpdateSpline = true);

	/** Set the rotation of an existing spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void SetRotationAtSplinePoint(int32 PointIndex, const FRotator& InRotation, bool bUpdateSpline = true);

	/** Set the scale at a given spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void SetScaleAtSplinePoint(int32 PointIndex, const FVector& InScaleVector, bool bUpdateSpline = true);

	/** Get the type of a spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		ESplinePointType::Type GetSplinePointType(int32 PointIndex) const;

	/** Specify the type of a spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void SetSplinePointType(int32 PointIndex, ESplinePointType::Type Type, bool bUpdateSpline = true);

	/** Get the number of points that make up this spline */
	UFUNCTION(BlueprintCallable, Category = Spline)
		int32 GetNumberOfSplinePoints() const;

	/** Get the number of segments that make up this spline */
	UFUNCTION(BlueprintCallable, Category = Spline)
		int32 GetNumberOfSplineSegments() const;

	/** Get the location at spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetLocationAtSplinePoint(int32 PointIndex) const;

	/** Get the direction at spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetDirectionAtSplinePoint(int32 PointIndex) const;

	/** Get the tangent at spline point. This fetches the Leave tangent of the point. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetTangentAtSplinePoint(int32 PointIndex) const;

	/** Get the arrive tangent at spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetArriveTangentAtSplinePoint(int32 PointIndex) const;

	/** Get the leave tangent at spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetLeaveTangentAtSplinePoint(int32 PointIndex) const;

	/** Get the rotation at spline point as a quaternion */
	FQuat GetQuaternionAtSplinePoint(int32 PointIndex) const;

	/** Get the rotation at spline point as a rotator */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FRotator GetRotationAtSplinePoint(int32 PointIndex) const;

	/** Get the up vector at spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetUpVectorAtSplinePoint(int32 PointIndex) const;

	/** Get the right vector at spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetRightVectorAtSplinePoint(int32 PointIndex) const;

	/** Get the amount of roll at spline point, in degrees */
	UFUNCTION(BlueprintCallable, Category = Spline)
		float GetRollAtSplinePoint(int32 PointIndex) const;

	/** Get the scale at spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetScaleAtSplinePoint(int32 PointIndex) const;

	/** Get the transform at spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FTransform GetTransformAtSplinePoint(int32 PointIndex, bool bUseScale = false) const;

	/** Get location and tangent at a spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void GetLocationAndTangentAtSplinePoint(int32 PointIndex, FVector& Location, FVector& Tangent) const;

	/** Get the distance along the spline at the spline point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		float GetDistanceAlongSplineAtSplinePoint(int32 PointIndex) const;

	/** Returns total length along this spline */
	UFUNCTION(BlueprintCallable, Category = Spline)
		float GetSplineLength() const;

	/** Sets the default up vector used by this spline */
	UFUNCTION(BlueprintCallable, Category = Spline)
		void SetDefaultUpVector(const FVector& UpVector);

	/** Gets the default up vector used by this spline */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetDefaultUpVector() const;

	/** Given a distance along the length of this spline, return the corresponding input key at that point */
	UFUNCTION(BlueprintCallable, Category = Spline)
		float GetInputKeyAtDistanceAlongSpline(float Distance) const;

	/** Given a distance along the length of this spline, return the point in space where this puts you */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetLocationAtDistanceAlongSpline(float Distance) const;

	/** Given a distance along the length of this spline, return a unit direction vector of the spline tangent there. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetDirectionAtDistanceAlongSpline(float Distance) const;


	/** Given a distance along the length of this spline, return the tangent vector of the spline there. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetTangentAtDistanceAlongSpline(float Distance) const;

	/** Given a distance along the length of this spline, return a quaternion corresponding to the spline's rotation there. */
	FQuat GetQuaternionAtDistanceAlongSpline(float Distance) const;

	/** Given a distance along the length of this spline, return a rotation corresponding to the spline's rotation there. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FRotator GetRotationAtDistanceAlongSpline(float Distance) const;


	/** Given a distance along the length of this spline, return a unit direction vector corresponding to the spline's up vector there. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetUpVectorAtDistanceAlongSpline(float Distance) const;

	/** Given a distance along the length of this spline, return a unit direction vector corresponding to the spline's right vector there. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetRightVectorAtDistanceAlongSpline(float Distance) const;

	/** Given a distance along the length of this spline, return the spline's roll there, in degrees. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		float GetRollAtDistanceAlongSpline(float Distance) const;

	/** Given a distance along the length of this spline, return the spline's scale there. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetScaleAtDistanceAlongSpline(float Distance) const;

	/** Given a distance along the length of this spline, return an FTransform corresponding to that point on the spline. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FTransform GetTransformAtDistanceAlongSpline(float Distance, bool bUseScale = false) const;

	/** Given a time from 0 to the spline duration, return the point in space where this puts you */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetLocationAtTime(float Time, bool bUseConstantVelocity = false) const;

	/** Given a time from 0 to the spline duration, return a unit direction vector of the spline tangent there. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetDirectionAtTime(float Time, bool bUseConstantVelocity = false) const;

	/** Given a time from 0 to the spline duration, return the spline's tangent there. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetTangentAtTime(float Time, bool bUseConstantVelocity = false) const;

	/** Given a time from 0 to the spline duration, return a quaternion corresponding to the spline's rotation there. */
	FQuat GetQuaternionAtTime(float Time, bool bUseConstantVelocity = false) const;

	/** Given a time from 0 to the spline duration, return a rotation corresponding to the spline's position and direction there. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FRotator GetRotationAtTime(float Time, bool bUseConstantVelocity = false) const;

	/** Given a time from 0 to the spline duration, return the spline's up vector there. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetUpVectorAtTime(float Time, bool bUseConstantVelocity = false) const;

	/** Given a time from 0 to the spline duration, return the spline's right vector there. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetRightVectorAtTime(float Time, bool bUseConstantVelocity = false) const;

	/** Given a time from 0 to the spline duration, return the spline's transform at the corresponding position. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FTransform GetTransformAtTime(float Time, bool bUseConstantVelocity = false, bool bUseScale = false) const;

	/** Given a time from 0 to the spline duration, return the spline's roll there, in degrees. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		float GetRollAtTime(float Time, bool bUseConstantVelocity = false) const;

	/** Given a time from 0 to the spline duration, return the spline's scale there. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector GetScaleAtTime(float Time, bool bUseConstantVelocity = false) const;

	/**
	 * @brief return the input key closest to that WorldLocation.
	 * @param WorldLocation
	 * @param RelativeTransform 轨迹初始所处Transform
	 * @return
	 */
	UFUNCTION(BlueprintCallable, Category = Spline)
		float FindInputKeyClosestToWorldLocation(const FVector& WorldLocation, const FTransform& RelativeTransform) const;


	/** Given a location, in world space, return the point on the curve that is closest to the location. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector FindLocationClosestToWorldLocation(const FVector& WorldLocation, const FTransform& RelativeTransform) const;

	/** Given a location, in world space, return a unit direction vector of the spline tangent closest to the location. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector FindDirectionClosestToWorldLocation(const FVector& WorldLocation, const FTransform& RelativeTransform) const;

	/** Given a location, in world space, return the tangent vector of the spline closest to the location. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector FindTangentClosestToWorldLocation(const FVector& WorldLocation, const FTransform& RelativeTransform) const;

	/** Given a location, in world space, return a quaternion corresponding to the spline's rotation closest to the location. */
	FQuat FindQuaternionClosestToWorldLocation(const FVector& WorldLocation, const FTransform& RelativeTransform) const;

	/** Given a location, in world space, return rotation corresponding to the spline's rotation closest to the location. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FRotator FindRotationClosestToWorldLocation(const FVector& WorldLocation, const FTransform& RelativeTransform) const;

	/** Given a location, in world space, return a unit direction vector corresponding to the spline's up vector closest to the location. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector FindUpVectorClosestToWorldLocation(const FVector& WorldLocation, const FTransform& RelativeTransform) const;

	/** Given a location, in world space, return a unit direction vector corresponding to the spline's right vector closest to the location. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector FindRightVectorClosestToWorldLocation(const FVector& WorldLocation, const FTransform& RelativeTransform) const;

	/** Given a location, in world space, return the spline's roll closest to the location, in degrees. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		float FindRollClosestToWorldLocation(const FVector& WorldLocation, const FTransform& RelativeTransform) const;

	/** Given a location, in world space, return the spline's scale closest to the location. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FVector FindScaleClosestToWorldLocation(const FVector& WorldLocation, const FTransform& RelativeTransform) const;

	/** Given a location, in world space, return an FTransform closest to that location. */
	UFUNCTION(BlueprintCallable, Category = Spline)
		FTransform FindTransformClosestToWorldLocation(const FVector& WorldLocation, const FTransform& RelativeTransform, bool bUseScale = false) const;

	float FindLocationsAtDisplacement(const float CurInputKey, const float Displacement, FVector& OutLocations);

	template< class T >
	float InaccurateFindNearest(const FInterpCurve<T>& InterpCurve, const float CurInputKey, float NeedDisplacementSq, FVector& OutLocation);

	template< class T >
	float InaccurateFindNearestOnSegment(const FInterpCurve<T>& InterpCurve, const float CurInputKey, float NeedDisplacementSq, float& OutDisplacementSq, int32 PtIdx, FVector& OutLocation) const;
private:
	/** The dummy value used for queries when there are no point in a spline */
	static const FInterpCurvePointVector DummyPointPosition;
	static const FInterpCurvePointQuat DummyPointRotation;
	static const FInterpCurvePointVector DummyPointScale;

private:
	/** Returns the length of the specified spline segment up to the parametric value given */
	float GetSegmentLength(const int32 Index, const float Param = 1.0f) const;

	/** Returns the parametric value t which would result in a spline segment of the given length between S(0)...S(t) */
	float GetSegmentParamFromLength(const int32 Index, const float Length, const float SegmentLength) const;

	/** Returns a const reference to the specified position point, but gives back a dummy point if there are no points */
	inline const FInterpCurvePointVector& GetPositionPointSafe(int32 PointIndex) const
	{
		const TArray<FInterpCurvePointVector>& Points = SplineCurves.Position.Points;
		const int32 NumPoints = Points.Num();
		if (NumPoints > 0)
		{
			const int32 ClampedIndex = (bClosedLoop && PointIndex >= NumPoints) ? 0 : FMath::Clamp(PointIndex, 0, NumPoints - 1);
			return Points[ClampedIndex];
		}
		else
		{
			return DummyPointPosition;
		}
	}

	/** Returns a const reference to the specified rotation point, but gives back a dummy point if there are no points */
	inline const FInterpCurvePointQuat& GetRotationPointSafe(int32 PointIndex) const
	{
		const TArray<FInterpCurvePointQuat>& Points = SplineCurves.Rotation.Points;
		const int32 NumPoints = Points.Num();
		if (NumPoints > 0)
		{
			const int32 ClampedIndex = (bClosedLoop && PointIndex >= NumPoints) ? 0 : FMath::Clamp(PointIndex, 0, NumPoints - 1);
			return Points[ClampedIndex];
		}
		else
		{
			return DummyPointRotation;
		}
	}

	/** Returns a const reference to the specified scale point, but gives back a dummy point if there are no points */
	inline const FInterpCurvePointVector& GetScalePointSafe(int32 PointIndex) const
	{
		const TArray<FInterpCurvePointVector>& Points = SplineCurves.Scale.Points;
		const int32 NumPoints = Points.Num();
		if (NumPoints > 0)
		{
			const int32 ClampedIndex = (bClosedLoop && PointIndex >= NumPoints) ? 0 : FMath::Clamp(PointIndex, 0, NumPoints - 1);
			return Points[ClampedIndex];
		}
		else
		{
			return DummyPointScale;
		}
	}
private:
	friend class UPathControllerLibrary;
};
