// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VCDataAssetDef.h"
#include "VCProcessor.h"
#include "VCSpringArmComponent.h"
#include "Components/ActorComponent.h"
#include "Interface/IDLVirtualCameraController.h"
#include "Interface/Interface_VirtualCam.h"
#include "VirtualCam.generated.h"



UCLASS(ClassGroup=(Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent),
	hideCategories = (ComponentTick,Tags,ComponentReplication,Activation,Variable,Cooking,AssetUserData,Collision))
class DLVIRTUALCAMERASYSTEM_API UVirtualCam : public UActorComponent, public IMyInterface_CameraData
{
	GENERATED_BODY()  
	 
private:
	void MixVectorData(const FVCBlendVector& BlendVector,const FVCBlendVector& LerpStartData, const bool IsRot, const FTransform& LocalToWorld,
	                   const float CurrentTime,const float MaxTime, FVCBlendVector& OutData) const;
	void MixFloatData(const FVCBlendFloat& BlendFloat, const FVCBlendFloat& LerpStartData,
						const float CurrentTime,const float MaxTime, FVCBlendFloat& OutData) const;

	void TryFixVCamData_Internal(const FTransform& ActorLocalToWorld, const FTransform& ArmLocalToWorld,
										  FVCCamBlendData* InFixCamData,const float CurrentTime,const float MaxTime, FVCCamBlendData& OutCamData);

	   
	int32 CurrentDataIndex = -1;
	bool SwitchToNext();
	bool SwitchToEnd();
	bool SwitchToPre();
	bool SwitchToStart();
	bool CleanDatas();
	
public:
	UVirtualCam();

	virtual int32 GetOrder_Implementation() override;

	virtual void RegisterArmComponent_Implementation(UActorComponent* Comp) override;

	virtual void TryFixVCamData_Implementation(const FTransform& ActorLocalToWorld, const FTransform& ArmLocalToWorld,
	                                           const FTransform& LockTargetTransform,const float DeltaTime,FVCCamBlendData& OutCamData) override;

	//设置预处理的环境变量
	UFUNCTION(BlueprintCallable)
	void VCSetProcessorData(UObject* Env);
	
	//激活镜头，在取消之前永久有效
	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCActive(UVCDataAssetDef* Data);

	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCActiveStruct(FVCCamBlendData InCamBlendData);

	//停止镜头 停止后不会再计算当前虚拟相机对最终相机的贡献了。
	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCStopActive();

	//激活镜头，时间到了之后自动禁用
	//AutoStopActive 勾选后 曲线时间到了会自动停止这个镜头效果
	//AutoStopActive 不勾选 曲线时间到了还会一直使用最后一个时刻的值
	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCActiveWithTimer(UVCDataAssetDef* Data, const EVCTimeOutAction InTimeOutAction = EVCTimeOutAction::Stop);

	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCActiveArrayWithTimer(TArray<UVCDataAssetDef*> DataArray, const EVCTimeOutAction InTimeOutAction = EVCTimeOutAction::Stop);

	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCActiveStructWithTimer(FVCCamBlendData InCamBlendData,
	                             const EVCTimeOutAction InTimeOutAction = EVCTimeOutAction::Stop);

	//设置循环次数
	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCSetLoopCount(const int32 InLoopCount);

	//手动更新计时器
	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCUpdateTimer(float NewTime);


	//手动设置数据，和 VCPlay 相关函数一起使用。
	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCSetData(UVCDataAssetDef* Data);

	//手动设置数据，和 VCPlay 相关函数一起使用。
	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCSetDataArray(TArray<UVCDataAssetDef*> DataArray);
	
	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCSetDataStruct(FVCCamBlendData InCamBlendData);

	//从当前曲线时间 开始正向播放。
	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCPlayForward(const EVCTimeOutAction InTimeOutAction = EVCTimeOutAction::Keep);

	//从曲线时间为0 开始正向播放
	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCPlayForwardFromStart(const EVCTimeOutAction InTimeOutAction = EVCTimeOutAction::Keep);

	//暂停播放，镜头效果这个时候还是作用上了的，只是曲线时间不会走了。
	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCPlayPause();

	//从当前曲线时间 开始反向播放
	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCPlayReverse(const EVCTimeOutAction InTimeOutAction = EVCTimeOutAction::Keep);

	//从曲线时间为最大值 开始反向播放
	UFUNCTION(BlueprintCallable, Category="VirtualCam")
	void VCPlayReverseFromEnd(const EVCTimeOutAction InTimeOutAction = EVCTimeOutAction::Keep);

	//查询是否在正播
	UFUNCTION(BlueprintPure, Category="VirtualCam")
	bool VCIsPlayForward();

	//查询当前曲线时间
	UFUNCTION(BlueprintPure, Category="VirtualCam")
	float VCGetCurrentTime();
	

	virtual void BeginPlay() override;
	virtual void TickComponent(const float DeltaTime, const ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


protected:
	UVCSpringArmComponent* ArmComponent;
	FVCCamBlendData CacheCamData;
	bool bIsForward = true;
	bool bStartTimer = false;
	float CurveCurrentTime = 0.0f;

	UPROPERTY()
	UObject* ProcessorEnv;
	UPROPERTY()
	UVCProcessor* VCPreProcessor;
	UPROPERTY()
	UVCProcessor* VCPostProcessor;
	FVCCamBlendData CamBlendData;
	FVCCamBlendData* CamBlendDataPtr;

	UPROPERTY()
	TArray<UVCProcessor*> VCPreProcessorArray;
	UPROPERTY()
	TArray<UVCProcessor*> VCPostProcessorArray;
	TArray<FVCCamBlendData*> CamBlendDataPtrArray;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Setting")
	int32 Order = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Setting")
	bool bIsEnable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Setting")
	EVCTimeOutAction TimeOutAction = EVCTimeOutAction::Stop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Setting",
		meta=(EditCondition = "TimeOutAction == EVCTimeOutAction::Loop", EditConditionHides))
	int32 LoopCount = 9999999;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Setting")
	UVCDataAssetDef* VCData;
};
