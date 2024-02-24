#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "IDLVirtualCameraController.generated.h"


class UVCDataAssetDef;

UENUM()
enum class EVCTimeOutAction : uint8
{
	// 一直循环播放
	Loop,

	// 保持结束的状态
	Keep,

	// 停止
	Stop,
};


UINTERFACE(BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class DLVIRTUALCAMERASYSTEM_API UDLVirtualCameraController : public  UInterface
{
	GENERATED_BODY()
};



/**
 * @brief 所有使用虚拟相机的实体都应该实现这个接口，外部只通过这个接口来控制相机
 */
class DLVIRTUALCAMERASYSTEM_API IDLVirtualCameraController : public IInterface
{
	GENERATED_BODY()

public:

	/**
	 * @brief 激活相机
	 * @param VirtualCameraID  这个相机的ID，因为一个实体可能挂载多个虚拟相机 
	 * @param DataArray VC 需要的资产文件，来描述激活后的效果
	 * @param InTimeOutAction 激活后的超时策略
	 */
	UFUNCTION(BlueprintCallable)
	virtual void ActiveVirtualCamera(const FGameplayTag& VirtualCameraID, TArray<UVCDataAssetDef*> DataArray, const EVCTimeOutAction InTimeOutAction) = 0;


	/**
	 * @brief 停止相机
	 * @param VirtualCameraID VC 的ID 
	*/
	UFUNCTION(BlueprintCallable)
	virtual void StopVirtualCamera(const FGameplayTag& VirtualCameraID) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void SetIsMouseMoved(const bool BisMoved) = 0;
	
	UFUNCTION(BlueprintCallable)
	virtual bool CheckIsMouseMoved() = 0;

	UFUNCTION(BlueprintCallable)
	virtual USceneComponent* GetDLSpringArm()
	{
		ensureAlwaysMsgf(true, TEXT("不能在这里"));
		return nullptr;
	};

};