#pragma once

#include "CoreMinimal.h"
#include "TestAlgoLib.generated.h"


UCLASS()
class DLKIT_API UTestAlgoLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * @brief 探测前面的地形
	 * @param WorldContextObject
	 * @param OriginLocation 起始点
	 * @param Orientation 朝向
	 * @param ObjectType
	 * @param Distance 距离
	 * @param IterationCount
	 * @param IterationInterval
	 * @param RetPoint 返回落脚点
	 * @param Sweep
	 */
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
		static void ProbeForwardLandforms(UObject* WorldContextObject, FVector OriginLocation, FVector Orientation, TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType, float
		                                  Distance, int32 IterationCount, float IterationInterval, FVector& RetPoint, bool& Sweep);

	/**
	 * 计算 一个 Actor 是否在另外一个 Actor 的前面
	 * 
	 * @param Origin  原点
	 * @param Target  需要做判定的点
	 * @param FontAngle 被判定为前方的夹角  单位 度
	 * @return TTuple<bool, float> 如果 Target 在  Origin 的前面，返回 True， 否则为 False.   float 为角度
	 */
	static TTuple<bool, float> CalculateActorInFront(class AActor* Origin, class AActor* Target, float FontAngle = 75);

	UFUNCTION(BlueprintCallable, meta = (displayname = CalculateActorInFront, defaultvalue))
	static void CalculateActorInFront_BP(class AActor* Origin, class AActor* Target, float FontAngle, bool& InFront, float& Angle);
};

