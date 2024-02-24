// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PredictTask/UnderAttack/UnderAttackPredictTask.h"
#include "UnderAttackDialUpTaskV2.generated.h"

/**
 *
 */
UCLASS()
class DLGAMEPLAYCORE_API UUnderAttackDialUpTaskV2 : public UDLUnderAttackTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		void BeginFly(const FRotator& Direction);

	UFUNCTION(BlueprintCallable)
		void BeginSlide(const FVector& Direction);

	UFUNCTION(BlueprintCallable)
		void EndFly();

	UFUNCTION(BlueprintImplementableEvent)
		void OnEndFly();

	UFUNCTION(BlueprintCallable)
		void EndSlide();

	UFUNCTION(BlueprintImplementableEvent)
		void OnEndSlide();

protected:
	virtual void Tick(float Dt) override;

	UFUNCTION(BlueprintImplementableEvent)
		void OnTick(float Dt);

	virtual void EndTask(const bool Cancel) override;
protected:
	/**
	 * @brief 优先级
	 */
	int32 Priority = 100;

	/**
	 * @brief 飞行时间
	 */
	UPROPERTY(BlueprintReadWrite)
		float FlyDuration = 0.0f;

	/**
	 * @brief 飞行高度
	 */
	UPROPERTY(EditAnywhere, Category = "Config", meta = (UIMin = 0.0f, ClampMin = 0.0f, UIMax = 9999999.f, ClampMax = 9999999.f))
		float FlyHeight = 0.0f;

	/**
	 * @brief 飞行距离
	 */
	UPROPERTY(EditAnywhere, Category = "Config", meta = (UIMin = 0.0f, ClampMin = 0.0f, UIMax = 9999999.f, ClampMax = 9999999.f))
		float FlyDistance = 0.0f;

	/**
	 * @brief 滑行时间
	 */
	UPROPERTY(EditAnywhere, Category = "Config", meta = (UIMin = 0.0f, ClampMin = 0.0f, UIMax = 100.f, ClampMax = 100.f))
		float SlideDuration = 0.0f;

	/**
	 * @brief 滑行距离
	 */
	UPROPERTY(EditAnywhere, Category = "Config", meta = (UIMin = 0.0f, ClampMin = 0.0f, UIMax = 9999999.f, ClampMax = 9999999.f))
		float SlideDistance = 0.0f;

	/**
	 * @brief 飞行偏移曲线
	 * @details 横轴标识进度,纵轴表示位置偏移，
	 * 需要注意的是其Z的值的计算方式是如果FlyHeight不为0，则Z的偏移为Z*FlyHeight，否则直接应用Z
	 * @example 可利用该曲线模拟飞行中的阻力,或者对轨迹进行调整
	 */
	UPROPERTY(EditAnywhere, Category = "Config")
		UCurveVector* Fly_PathOffsetCurve = nullptr;

	/**
	 * @brief 时间映射曲线
	 * @details 横轴标识进度，纵轴表示进度对应的时间点，比如我总时长是1s，当前进度是0.5，我对应的时间可以是0.9s
	 */
	UPROPERTY(EditAnywhere, Category = "Config")
		UCurveFloat* TimeMappingCurve = nullptr;
private:
	TSharedPtr<FRootMotionSource_JumpForce> RootMotionFly = nullptr;
	TSharedPtr<FRootMotionSource_MoveToForce> RootMotionSlide = nullptr;
	uint16 RootMotionId = static_cast<uint16>(ERootMotionSourceID::Invalid);
};
