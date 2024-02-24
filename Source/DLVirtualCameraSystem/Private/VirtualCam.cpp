// Fill out your copyright notice in the Description page of Project Settings.


#include "VirtualCam.h"

#include "VCProcessor.h"
#include "Kismet/KismetMathLibrary.h"
#include "VisualLogger/VisualLogger.h"


UVirtualCam::UVirtualCam()
{
	PrimaryComponentTick.bCanEverTick = true;
	if (VCData) CamBlendDataPtr = &VCData->CamBlendData;
}

int32 UVirtualCam::GetOrder_Implementation()
{
	return Order;
}

void UVirtualCam::RegisterArmComponent_Implementation(UActorComponent* Comp)
{
	ArmComponent = Cast<UVCSpringArmComponent>(Comp);
}

void UVirtualCam::MixVectorData(const FVCBlendVector& BlendVector, const FVCBlendVector& LerpStartData,
                                const bool IsRot,
                                const FTransform& LocalToWorld,
                                const float CurrentTime,const float MaxTime,FVCBlendVector& OutData) const
{
	if (BlendVector.BlendMode == EVCBlendMode::Inherit) return;

	const FVector CacheData = OutData.Value;

	//统一处理值
	FVector TempValue = BlendVector.Value;
	if (BlendVector.ValueType == EVCBlendValueType::Curve && BlendVector.BlendMode != EVCBlendMode::Lerp)
	{
		if (ensureAlwaysMsgf(BlendVector.CurveValue, TEXT("忘记配置 Curve 了 ")))
		{
			TempValue = BlendVector.CurveValue->GetVectorValue(CurrentTime);
		}
	}

	//统一处理本地空间，转换到世界空间计算。
	if (BlendVector.CoordinateType == EVCBlendCoordinate::Local)
	{
		if (IsRot)
		{
			if (BlendVector.BlendMode != EVCBlendMode::Add)
			{
				const FRotator Rot = VecToRot(TempValue);
				const FRotator NewRot = LocalToWorld.TransformRotation(Rot.Quaternion()).Rotator();
				TempValue = RotToVec(NewRot);
			}
		}
		else
		{
			if (BlendVector.BlendMode == EVCBlendMode::Add)
			{
				TempValue = LocalToWorld.TransformVector(TempValue);
			}
			else
			{
				TempValue = LocalToWorld.TransformPosition(TempValue);
			}
		}
	}

	//处理混合模式
	if (BlendVector.BlendMode == EVCBlendMode::Add)
	{
		if (IsRot)
		{
			const FRotator NewRot = VecToRot(OutData.Value) + VecToRot(TempValue);
			OutData.Value = RotToVec(NewRot);
		}
		else
		{
			OutData.Value = OutData.Value + TempValue;
		}
	}
	else if (BlendVector.BlendMode == EVCBlendMode::Override)
	{
		OutData.Value = TempValue;
	}
	else if (BlendVector.BlendMode == EVCBlendMode::Lerp)
	{
		if (IsRot)
		{
			const FRotator TemRot = UKismetMathLibrary::RLerp(VecToRot(LerpStartData.Value), VecToRot(TempValue),
			                                                  CurrentTime / MaxTime, true);
			OutData.Value = RotToVec(TemRot);
		}
		else
		{
			OutData.Value = FMath::Lerp(LerpStartData.Value, TempValue,
			                            CurrentTime / MaxTime);
		}
	}

	//忽略
	if (BlendVector.IgnoreX) OutData.Value.X = CacheData.X;
	if (BlendVector.IgnoreY) OutData.Value.Y = CacheData.Y;
	if (BlendVector.IgnoreZ) OutData.Value.Z = CacheData.Z;
}

void UVirtualCam::MixFloatData(const FVCBlendFloat& BlendFloat, const FVCBlendFloat& LerpStartData,
                               const float CurrentTime,const float MaxTime,FVCBlendFloat& OutData) const
{
	if (BlendFloat.BlendMode == EVCBlendMode::Inherit) return;
	//统一处理值
	float TempValue = BlendFloat.Value;
	if (BlendFloat.ValueType == EVCBlendValueType::Curve && BlendFloat.BlendMode != EVCBlendMode::Lerp)
	{
		if (ensureAlwaysMsgf(BlendFloat.CurveValue, TEXT("忘记配置 Curve 了 ")))
		{
			TempValue = BlendFloat.CurveValue->GetFloatValue(CurrentTime);
		}
	}

	if (BlendFloat.BlendMode == EVCBlendMode::Add)
	{
		OutData.Value = OutData.Value + TempValue;
	}
	else if (BlendFloat.BlendMode == EVCBlendMode::Override)
	{
		OutData.Value = TempValue;
	}
	else if (BlendFloat.BlendMode == EVCBlendMode::Lerp)
	{
		OutData.Value = FMath::Lerp(LerpStartData.Value, TempValue,
		                            CurrentTime / MaxTime);
	}
}

void UVirtualCam::TryFixVCamData_Internal(const FTransform& ActorLocalToWorld, const FTransform& ArmLocalToWorld,
                                          FVCCamBlendData* InFixCamData,const float CurrentTime,const float MaxTime, FVCCamBlendData& OutCamData)
{
	if (InFixCamData->LookAtTargetOffsetWithArmRot == EVCBlendBool::BoolTrue)
	{
		MixVectorData(InFixCamData->LookAtTarget, CacheCamData.LookAtTarget, false, ArmLocalToWorld,
		              CurrentTime,MaxTime,OutCamData.LookAtTarget);
	}
	else
	{
		MixVectorData(InFixCamData->LookAtTarget, CacheCamData.LookAtTarget, false, ActorLocalToWorld,
		              CurrentTime,MaxTime,OutCamData.LookAtTarget);
	}

	if (InFixCamData->EnableTargetLag != EVCBlendBool::Inherit)
	{
		OutCamData.EnableTargetLag = InFixCamData->EnableTargetLag;
	}

	MixFloatData(InFixCamData->TargetLagSpeed, CacheCamData.TargetLagSpeed,CurrentTime,MaxTime, OutCamData.TargetLagSpeed);

	MixFloatData(InFixCamData->LagMaxDistance, CacheCamData.LagMaxDistance,CurrentTime,MaxTime, OutCamData.LagMaxDistance);

	MixVectorData(InFixCamData->ArmRot, CacheCamData.ArmRot, true, ActorLocalToWorld,CurrentTime,MaxTime, OutCamData.ArmRot);


	// if (InFixCamData->EnableMoveRotFix != EVCBlendBool::Inherit)
	// {
	// 	OutCamData.EnableMoveRotFix = InFixCamData->EnableMoveRotFix;
	// }
	//
	// MixFloatData(InFixCamData->MoveRotFixSpeed, CacheCamData.MoveRotFixSpeed, OutCamData.MoveRotFixSpeed);

	if (InFixCamData->EnableArmRotLag != EVCBlendBool::Inherit)
	{
		OutCamData.EnableArmRotLag = InFixCamData->EnableArmRotLag;
	}

	MixFloatData(InFixCamData->ArmRotLagSpeed, CacheCamData.ArmRotLagSpeed,CurrentTime,MaxTime, OutCamData.ArmRotLagSpeed);

	MixFloatData(InFixCamData->ArmLength, CacheCamData.ArmLength,CurrentTime,MaxTime, OutCamData.ArmLength);

	MixFloatData(InFixCamData->CamFOV, CacheCamData.CamFOV,CurrentTime,MaxTime, OutCamData.CamFOV);
}

void UVirtualCam::TryFixVCamData_Implementation(const FTransform& ActorLocalToWorld, const FTransform& ArmLocalToWorld,
                                                const FTransform& LockTargetTransform,const float DeltaTime, FVCCamBlendData& OutCamData)
{
	if (!bIsEnable) return;
	if (!CamBlendDataPtr)
	{
		ensureMsgf(0, TEXT("播放镜头动画时 没有镜头数据"));
		return;
	}

	CurveCurrentTime = FMath::Clamp(CurveCurrentTime, 0.0f, CamBlendDataPtr->CurveMaxTime);

	if (VCPreProcessor)
	{
		
		FVCCamBlendData PreOutData;
		FProcessorEnv Env;
		Env.EnvDataObject = ProcessorEnv;
		Env.bIsForward = bIsForward;
		Env.MaxTime = CamBlendDataPtr->CurveMaxTime;
		Env.CurveCurrentTime = CurveCurrentTime;
		Env.DeltaTime = DeltaTime;
		float CurrentTime = CurveCurrentTime;
		float MaxTime = CamBlendDataPtr->CurveMaxTime;
		VCPreProcessor->TryFixVCamData(ActorLocalToWorld, ArmLocalToWorld, Env,CurrentTime,MaxTime, PreOutData);
		TryFixVCamData_Internal(ActorLocalToWorld, ArmLocalToWorld, &PreOutData,CurrentTime,MaxTime, OutCamData);
		
	}

	TryFixVCamData_Internal(ActorLocalToWorld, ArmLocalToWorld, CamBlendDataPtr,CurveCurrentTime,CamBlendDataPtr->CurveMaxTime, OutCamData);

	if (VCPostProcessor)
	{
		FVCCamBlendData PostOutData;
		FProcessorEnv Env;
		Env.EnvDataObject = ProcessorEnv;
		Env.bIsForward = bIsForward;
		Env.MaxTime = CamBlendDataPtr->CurveMaxTime;
		Env.CurveCurrentTime = CurveCurrentTime;
		Env.DeltaTime = DeltaTime;
		float CurrentTime = CurveCurrentTime;
		float MaxTime = CamBlendDataPtr->CurveMaxTime;
		VCPostProcessor->TryFixVCamData(ActorLocalToWorld, ArmLocalToWorld, Env,CurrentTime,MaxTime, PostOutData);
		TryFixVCamData_Internal(ActorLocalToWorld, ArmLocalToWorld, &PostOutData, CurrentTime,MaxTime,OutCamData);
	}
}

void UVirtualCam::VCSetProcessorData(UObject* Env)
{
	ProcessorEnv = Env;
}
void UVirtualCam::VCActive(UVCDataAssetDef* Data)
{
	if (Data)
	{
		CleanDatas();
		CamBlendDataPtrArray.Add(&Data->CamBlendData);
		UVCProcessor* Pre = nullptr;
		if (Data->PreProcessor) Pre = NewObject<UVCProcessor>(GetWorld(), Data->PreProcessor);
		VCPreProcessorArray.Add(Pre);
		UVCProcessor* Post = nullptr;
		if (Data->PostProcessor) Post = NewObject<UVCProcessor>(GetWorld(), Data->PostProcessor);
		VCPostProcessorArray.Add(Post);
		bIsEnable = true;
		SetComponentTickEnabled(false);
		SwitchToStart();
	}
	else
	{
		ensureMsgf(0, TEXT("播放镜头动画时 没有镜头数据"));
	}
}

void UVirtualCam::VCActiveStruct(FVCCamBlendData InCamBlendData)
{
	CamBlendData = InCamBlendData;
	CamBlendDataPtr = &CamBlendData;
	bIsEnable = true;
	SetComponentTickEnabled(false);
}

void UVirtualCam::VCActiveWithTimer(UVCDataAssetDef* Data, const EVCTimeOutAction InTimeOutAction)
{
	if (Data)
	{
		CleanDatas();
		CamBlendDataPtrArray.Add(&Data->CamBlendData);
		UVCProcessor* Pre = nullptr;
		if (Data->PreProcessor) Pre = NewObject<UVCProcessor>(GetWorld(), Data->PreProcessor);
		VCPreProcessorArray.Add(Pre);
		UVCProcessor* Post = nullptr;
		if (Data->PostProcessor) Post = NewObject<UVCProcessor>(GetWorld(), Data->PostProcessor);
		VCPostProcessorArray.Add(Post);
		bIsEnable = true;
		bIsForward = true;
		TimeOutAction = InTimeOutAction;
		SetComponentTickEnabled(true);
		SwitchToStart();
	}
	else
	{
		ensureMsgf(0, TEXT("播放镜头动画时 没有镜头数据"));
	}
}

bool UVirtualCam::SwitchToNext()
{
	if (VCPreProcessor) VCPreProcessor->EndProcess(ProcessorEnv);
	if (VCPostProcessor) VCPostProcessor->EndProcess(ProcessorEnv);
	VCPreProcessor = nullptr;
	VCPostProcessor = nullptr;
	if (CurrentDataIndex < CamBlendDataPtrArray.Num() - 1)
	{
		CurrentDataIndex++;
		CamBlendDataPtr = CamBlendDataPtrArray[CurrentDataIndex];
		VCPreProcessor = VCPreProcessorArray[CurrentDataIndex];
		VCPostProcessor = VCPostProcessorArray[CurrentDataIndex];
		if (VCPreProcessor) VCPreProcessor->BeginProcess(ProcessorEnv);
		if (VCPostProcessor) VCPostProcessor->BeginProcess(ProcessorEnv);
		if (ArmComponent)
		{
			CacheCamData = ArmComponent->VCGetLastCamData();
		}
		CurveCurrentTime = 0;
		return true;
	}
	return false;
}

bool UVirtualCam::SwitchToEnd()
{
	if (CamBlendDataPtrArray.Num() == 0) return false;

	if (VCPreProcessor) VCPreProcessor->EndProcess(ProcessorEnv);
	if (VCPostProcessor) VCPostProcessor->EndProcess(ProcessorEnv);
	VCPreProcessor = nullptr;
	VCPostProcessor = nullptr;
	CurrentDataIndex = CamBlendDataPtrArray.Num() - 1;
	CamBlendDataPtr = CamBlendDataPtrArray[CurrentDataIndex];
	VCPreProcessor = VCPreProcessorArray[CurrentDataIndex];
	VCPostProcessor = VCPostProcessorArray[CurrentDataIndex];
	if (VCPreProcessor) VCPreProcessor->BeginProcess(ProcessorEnv);
	if (VCPostProcessor) VCPostProcessor->BeginProcess(ProcessorEnv);
	if (ArmComponent)
	{
		CacheCamData = ArmComponent->VCGetLastCamData();
	}
	CurveCurrentTime = CamBlendDataPtr->CurveMaxTime;
	return true;
}

bool UVirtualCam::SwitchToPre()
{
	if (VCPreProcessor) VCPreProcessor->EndProcess(ProcessorEnv);
	if (VCPostProcessor) VCPostProcessor->EndProcess(ProcessorEnv);
	VCPreProcessor = nullptr;
	VCPostProcessor = nullptr;
	if (CurrentDataIndex > 0)
	{
		CurrentDataIndex--;
		CamBlendDataPtr = CamBlendDataPtrArray[CurrentDataIndex];
		VCPreProcessor = VCPreProcessorArray[CurrentDataIndex];
		VCPostProcessor = VCPostProcessorArray[CurrentDataIndex];
		if (VCPreProcessor) VCPreProcessor->BeginProcess(ProcessorEnv);
		if (VCPostProcessor) VCPostProcessor->BeginProcess(ProcessorEnv);
		if (ArmComponent)
		{
			CacheCamData = ArmComponent->VCGetLastCamData();
		}
		CurveCurrentTime = CamBlendDataPtr->CurveMaxTime;
		return true;
	}
	return false;
}

bool UVirtualCam::SwitchToStart()
{
	if (CamBlendDataPtrArray.Num() == 0)
	{
		ensureMsgf(0, TEXT("动画数据长度为0，错误！"));
		return false;
	}
	if (VCPreProcessor) VCPreProcessor->EndProcess(ProcessorEnv);
	if (VCPostProcessor) VCPostProcessor->EndProcess(ProcessorEnv);
	VCPreProcessor = nullptr;
	VCPostProcessor = nullptr;
	CurrentDataIndex = 0;
	CamBlendDataPtr = CamBlendDataPtrArray[CurrentDataIndex];
	VCPreProcessor = VCPreProcessorArray[CurrentDataIndex];
	VCPostProcessor = VCPostProcessorArray[CurrentDataIndex];
	if (VCPreProcessor) VCPreProcessor->BeginProcess(ProcessorEnv);
	if (VCPostProcessor) VCPostProcessor->BeginProcess(ProcessorEnv);
	if (ArmComponent)
	{
		CacheCamData = ArmComponent->VCGetLastCamData();
	}
	CurveCurrentTime = 0;
	return true;
}

bool UVirtualCam::CleanDatas()
{
	CamBlendDataPtrArray.Empty();
	VCPreProcessorArray.Empty();
	VCPostProcessorArray.Empty();
	return true;
}

void UVirtualCam::VCActiveArrayWithTimer(TArray<UVCDataAssetDef*> DataArray, const EVCTimeOutAction InTimeOutAction)
{
	if (DataArray.Num() > 0)
	{
		CleanDatas();
		for (auto Data : DataArray)
		{
			if (Data)
			{
				CamBlendDataPtrArray.Add(&Data->CamBlendData);
				UVCProcessor* Pre = nullptr;
				if (Data->PreProcessor) Pre = NewObject<UVCProcessor>(GetWorld(), Data->PreProcessor);
				VCPreProcessorArray.Add(Pre);
				UVCProcessor* Post = nullptr;
				if (Data->PostProcessor) Post = NewObject<UVCProcessor>(GetWorld(), Data->PostProcessor);
				VCPostProcessorArray.Add(Post);
			}
		}
		bIsEnable = true;
		bIsForward = true;
		TimeOutAction = InTimeOutAction;
		SetComponentTickEnabled(true);
		SwitchToStart();
	}
	else
	{
		ensureMsgf(0, TEXT("播放镜头动画时 没有镜头数据"));
	}
}

void UVirtualCam::VCActiveStructWithTimer(FVCCamBlendData InCamBlendData,
                                          const EVCTimeOutAction InTimeOutAction)
{
	CleanDatas();
	CamBlendData = InCamBlendData;
	CamBlendDataPtrArray.Add(&CamBlendData);
	VCPreProcessorArray.Add(nullptr);
	VCPostProcessorArray.Add(nullptr);
	bIsEnable = true;
	bIsForward = true;
	TimeOutAction = InTimeOutAction;
	SetComponentTickEnabled(true);
	SwitchToStart();
}

void UVirtualCam::VCSetLoopCount(const int32 InLoopCount)
{
	LoopCount = InLoopCount;
}

void UVirtualCam::VCStopActive()
{
	bIsEnable = false;
	SetComponentTickEnabled(false);
	if (VCPreProcessor) VCPreProcessor->EndProcess(ProcessorEnv);
	if (VCPostProcessor) VCPostProcessor->EndProcess(ProcessorEnv);
	VCPreProcessor = nullptr;
	VCPostProcessor = nullptr;
}

void UVirtualCam::VCUpdateTimer(float NewTime)
{
	CurveCurrentTime = NewTime;
}

void UVirtualCam::VCSetData(UVCDataAssetDef* Data)
{
	CleanDatas();
	if (Data)
	{
		CamBlendDataPtrArray.Add(&Data->CamBlendData);
		UVCProcessor* Pre = nullptr;
		if (Data->PreProcessor) Pre = NewObject<UVCProcessor>(GetWorld(), Data->PreProcessor);
		VCPreProcessorArray.Add(Pre);
		UVCProcessor* Post = nullptr;
		if (Data->PostProcessor) Post = NewObject<UVCProcessor>(GetWorld(), Data->PostProcessor);
		VCPostProcessorArray.Add(Post);
	}
	else
	{
		ensureMsgf(0, TEXT("Data 不能为Null"));
	}
}

void UVirtualCam::VCSetDataArray(TArray<UVCDataAssetDef*> DataArray)
{
	if (DataArray.Num() > 0)
	{
		CleanDatas();
		for (auto Data : DataArray)
		{
			if (Data)
			{
				CamBlendDataPtrArray.Add(&Data->CamBlendData);
				UVCProcessor* Pre = nullptr;
				if (Data->PreProcessor) Pre = NewObject<UVCProcessor>(GetWorld(), Data->PreProcessor);
				VCPreProcessorArray.Add(Pre);
				UVCProcessor* Post = nullptr;
				if (Data->PostProcessor) Post = NewObject<UVCProcessor>(GetWorld(), Data->PostProcessor);
				VCPostProcessorArray.Add(Post);
			}
			else
			{
				ensureMsgf(0, TEXT("Data 不能为Null"));
			}
		}
	}
	else
	{
		ensureMsgf(0, TEXT("播放镜头动画时 没有镜头数据"));
	}
}

void UVirtualCam::VCSetDataStruct(FVCCamBlendData InCamBlendData)
{
	CamBlendDataPtr = &InCamBlendData;
}

void UVirtualCam::VCPlayForward(const EVCTimeOutAction InTimeOutAction)
{
	bIsForward = true;
	bIsEnable = true;
	TimeOutAction = InTimeOutAction;
	SetComponentTickEnabled(true);
}

void UVirtualCam::VCPlayForwardFromStart(const EVCTimeOutAction InTimeOutAction)
{
	bIsForward = true;
	bIsEnable = true;
	TimeOutAction = InTimeOutAction;
	CurveCurrentTime = 0.0f;
	SetComponentTickEnabled(true);
}

void UVirtualCam::VCPlayPause()
{
	SetComponentTickEnabled(false);
}

void UVirtualCam::VCPlayReverse(const EVCTimeOutAction InTimeOutAction)
{
	bIsForward = false;
	bIsEnable = true;
	TimeOutAction = InTimeOutAction;
	SetComponentTickEnabled(true);
}

void UVirtualCam::VCPlayReverseFromEnd(const EVCTimeOutAction InTimeOutAction)
{
	bIsForward = false;
	bIsEnable = true;
	TimeOutAction = InTimeOutAction;
	CurveCurrentTime = CamBlendDataPtr->CurveMaxTime;
	SetComponentTickEnabled(true);
}

bool UVirtualCam::VCIsPlayForward()
{
	return bIsForward;
}

float UVirtualCam::VCGetCurrentTime()
{
	return CurveCurrentTime;
}


void UVirtualCam::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
}

void UVirtualCam::TickComponent(const float DeltaTime, const ELevelTick TickType,
                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsEnable && CamBlendDataPtr)
	{
		if (bIsForward)
		{
			if (CurveCurrentTime > CamBlendDataPtr->CurveMaxTime - 0.001f)
			{
				if (!SwitchToNext())
				{
					if (TimeOutAction == EVCTimeOutAction::Keep)
					{
						CurveCurrentTime = CamBlendDataPtr->CurveMaxTime;
						SetComponentTickEnabled(false);
					};
					if (TimeOutAction == EVCTimeOutAction::Stop)
					{
						VCStopActive();
					}
					if (TimeOutAction == EVCTimeOutAction::Loop)
					{
						LoopCount--;
						if (LoopCount < 1)
						{
							VCStopActive();
						}
						else
						{
							SwitchToStart();
						}
					}
				}
			}
			else
			{
				CurveCurrentTime += DeltaTime;
			}
		}
		else
		{
			if (CurveCurrentTime < 0.001f)
			{
				if (!SwitchToPre())
				{
					if (TimeOutAction == EVCTimeOutAction::Keep)
					{
						CurveCurrentTime = 0;
						SetComponentTickEnabled(false);
					};
					if (TimeOutAction == EVCTimeOutAction::Stop)
					{
						VCStopActive();
					}
					if (TimeOutAction == EVCTimeOutAction::Loop)
					{
						LoopCount--;
						if (LoopCount < 1)
						{
							VCStopActive();
						}
						else
						{
							SwitchToEnd();
						}
					}
				}
			}
			else
			{
				CurveCurrentTime -= DeltaTime;
			}
		}
	}
}
