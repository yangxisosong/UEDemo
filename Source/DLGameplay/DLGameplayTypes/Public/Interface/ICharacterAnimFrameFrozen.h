#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "ICharacterAnimFrameFrozen.generated.h"



USTRUCT(BlueprintType)
struct FDLCharacterAnimFrameFrozenArg
{
	GENERATED_BODY()
public:

	/**
	 * @brief 定帧的持续时间
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Duration = 0.f;

	/**
	 * @brief 定帧时的 时间 动画播放速度的曲线  归一化
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UCurveFloat* AnimSpeedCurve;

	/**
	* @brief 定帧的类型
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories = "Gameplay.Character.FrameFrozenType"))
		FGameplayTag FrameFrozenType;

	/**
	 * @brief 需要定帧的部位
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FGameplayTagContainer Parts;
};



UINTERFACE(MinimalAPI, BlueprintType)
class UDLCharacterAnimFrameFrozen : public UInterface
{
	GENERATED_BODY()
public:

};


/**
 * @brief 动画定帧的接口
 */
class DLGAMEPLAYTYPES_API IDLCharacterAnimFrameFrozen : public IInterface
{
	GENERATED_BODY()
public:


	/**
	 * @brief 触发定帧
	 * @param FrameFrozenData
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void TriggerFrameFrozen(FDLCharacterAnimFrameFrozenArg FrameFrozenData);


	/**
	 * @brief 定帧是否完成
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool IsComplateFrameFrozen();

	/**
	 * @brief 停止定帧
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void StopFrameFrozen();
};
