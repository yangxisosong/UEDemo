// Fill out your copyright notice in the Description page of Project Settings
#include "Animation/DLAnimInstanceBaseV2.h"
#include "Animation/DLAnimationLibrary.h"
#include "Animation/DLCharacterAnimationAsset.h"
#include "GameFramework/Character.h"
#include "Component/DLCharacterMovementComponentBase.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/ICharacterStateAccessor.h"
#include "Kismet/GameplayStatics.h"

TScriptInterface<ICharacterStateAccessor> UDLAnimInstanceBaseV2::GetCharacterStateAccessor() const
{
	return CharacterStateAccessor;
}

UDLCharacterMovementComponentBase* UDLAnimInstanceBaseV2::GetMovementComponentBase() const
{
	return MovementComponentBase;
}

void UDLAnimInstanceBaseV2::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UDLAnimInstanceBaseV2::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//防止某些数学计算出错
	if (FMath::IsNearlyZero(DeltaSeconds))
	{
		return;
	}

	if (!GetIsInitialized())
	{
#if WITH_EDITOR
		//非运行时调用OnUpdateAnimation_BP
		if (!IsRunningGame())
		{
			OnUpdateAnimation_BP(DeltaSeconds);
		}
#endif
		return;
	}

	if (!OnPreUpdateAnimation(DeltaSeconds))
	{
		ensureMsgf(false, TEXT("预先UpdateAnimation失败了！！！！中断Update流程"));
		return;
	}

	CopyNewAnimCharacterInfo(DeltaSeconds);

	OnUpdateAnimation(DeltaSeconds);
	OnUpdateAnimation_BP(DeltaSeconds);

	//每帧标记一次旧数据,Tick完成之后我们就能够确定下次PrevAnimCharacterInfo了
	// FAnimCharacterInfoBase& NextPrevAnimCharacterInfo = GetNextPrevAnimCharacterInfo();
	// NextPrevAnimCharacterInfo = GetAnimCharacterInfo();

	OnPostUpdateAnimation(DeltaSeconds);
}

void UDLAnimInstanceBaseV2::PrintLog(const FString& Log)
{
	UE_LOG(LogTemp, Log, TEXT("<%s> %s"), *GetLogCategory(), *Log);
}

void UDLAnimInstanceBaseV2::K2_PrintAnimLog(const FString& Log)
{
	if (GetIsEnableAnimLog())
	{
		PrintLog(Log);
	}
}

void UDLAnimInstanceBaseV2::InitAnimIns(const FDLAnimInsInitParams& InitParams)
{
	if (GetIsInitialized())
	{
		ensureMsgf(false, TEXT("为什么要初始化两次呢？？？？"));
		return;
	}
	if (!OnPreInit(InitParams))
	{
		ensureMsgf(false, TEXT("预先初始化失败了！！！！中断初始化流程"));
		return;
	}

	GetOwningComponent()->AddTickPrerequisiteActor(TryGetPawnOwner());

	bIsInitAnimIns = OnInitAnimIns(InitParams);
	if (!ensureAlwaysMsgf(bIsInitAnimIns, TEXT("初始化失败了！！！！中断初始化流程")))
	{
		return;
	}
	OnPostInit(InitParams);
}

bool UDLAnimInstanceBaseV2::OnInitAnimIns(const FDLAnimInsInitParams& InitParams)
{
	TArray<FText> Tmp;
	if (InitParams.AnimConfig.IsValidConfig(Tmp))
	{
		SetAnimConfig(&InitParams.AnimConfig);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("使用调试动画配置！！！"));
#if WITH_EDITOR
		if (PreviewCharacterAnimationAsset)
		{
			const auto AnimConfigAsset = PreviewCharacterAnimationAsset.LoadSynchronous();
			if (ensureAlwaysMsgf(AnimConfigAsset, TEXT("没有配置预览动画资产")))
			{
				const FAnimConfigBase& AnimConfig = AnimConfigAsset->GetAnimConfig();
				SetAnimConfig(&AnimConfig);
			}
		}
#endif
		if (!GetAnimConfig().IsValidConfig(Tmp))
		{
			for (auto Text : Tmp)
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid Config:%s"), *Text.ToString());
			}

			// ensureAlwaysMsgf(false, TEXT("动画配置错误！！！"));
			return false;
		}
	}

	CharacterStateAccessor = TryGetPawnOwner();
	MovementComponentBase = Cast<UDLCharacterMovementComponentBase>(TryGetPawnOwner()->GetMovementComponent());

	if (!ensureAlwaysMsgf(CharacterStateAccessor, TEXT("应该可以获取到 读取状态的 接口")))
	{
		return false;
	}

	if (!ensureAlwaysMsgf(MovementComponentBase, TEXT("应该可以获取到  移动组件的 接口")))
	{
		return false;
	}
	return true;
}
