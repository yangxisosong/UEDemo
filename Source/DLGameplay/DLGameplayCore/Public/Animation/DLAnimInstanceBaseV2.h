// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLAnimBaseDef.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "DLAnimInstanceBaseV2.generated.h"

class UDLCharacterAnimationAssetBase;
class UDLCharacterMovementComponentBase;
class ICharacterStateAccessor;
/**
 * 动画实例基类
 */
UCLASS(meta = (BlueprintThreadSafe))
class DLGAMEPLAYCORE_API UDLAnimInstanceBaseV2 : public UAnimInstance
{
	GENERATED_BODY()

public:

	/**
	 * @brief 获取角色状态的接口，角色的 Gameplay 状态都应该从这个接口获取
	 * @return
	 */
	UFUNCTION(BlueprintPure, Category = "Animation | Tool")
		TScriptInterface<ICharacterStateAccessor> GetCharacterStateAccessor() const;

	/**
	 * @brief 获取移动组件
	 * @return
	 * @note  在使用时 根据设计，禁止Cast为他的子类  而是应该依赖于 子类特定的接口
	 * @see UDLCharacterMovementComponentBase
	 */
	UFUNCTION(BlueprintPure, Category = "Animation | Tool")
		UDLCharacterMovementComponentBase* GetMovementComponentBase() const;

	/**
	 * @brief 获取Clamp过的动画曲线
	 * @param CurveName
	 * @param Bias
	 * @param ClampMin
	 * @param ClampMax
	 * @return
	 */
	UFUNCTION(BlueprintPure, Category = "Animation | Tool")
		float GetAnimCurveClamped(const FName CurveName, float Bias, float ClampMin, float ClampMax) const
	{
		return FMath::Clamp(GetCurveValue(CurveName) + Bias, ClampMin, ClampMax);
	}

public:
	/**
	 * @brief 对动画蓝图进行初始化
	 * @param InitParams
	 */
	void InitAnimIns(const FDLAnimInsInitParams& InitParams);

public:
	/**
	 * @brief 应用调试角色信息
	 */
	UFUNCTION(CallInEditor)
		void ApplyDebugAnimCharacterInfo() { ApplyDebugAnimCharacterInfo_Impl(); };

protected:

#pragma region UAnimInstance Implements

#if WITH_EDITOR
	/*开启AnimGraph的FastPath变量优化警告*/
	virtual bool PCV_ShouldWarnAboutNodesNotUsingFastPath() const override final { return true; }

	virtual bool PCV_ShouldNotifyAboutNodesNotUsingFastPath() const override final { return true; }
#endif

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override final;

#pragma endregion

public:
	/**
	 * @brief 获取角色动画相关信息
	 * @return
	 */
	UFUNCTION(BlueprintPure)
		const FAnimCharacterInfoBase& K2_GetAnimCharacterInfo()
	{
		return GetAnimCharacterInfo();
	};

	/**
	 * @brief 获取动画配置
	 * @return
	 */
	UFUNCTION(BlueprintPure)
		const FAnimConfigBase& K2_GetAnimConfig()
	{
		return GetAnimConfig();
	}

public:
	//get or set methods
	virtual FAnimCharacterInfoBase& GetAnimCharacterInfoRef()
	{
		ensureMsgf(false, TEXT("必须实现该函数！！！"));
		static FAnimCharacterInfoBase Tmp{};
		return Tmp;
	};

	const FAnimCharacterInfoBase& GetAnimCharacterInfo()
	{
		return GetAnimCharacterInfoRef();
	}

	virtual FAnimCharacterInfoBase& GetPrevAnimCharacterInfoRef()
	{
		ensureMsgf(false, TEXT("必须实现该函数！！！"));
		static FAnimCharacterInfoBase Tmp{};
		return Tmp;
	};

	virtual void CopyPrevAnimCharacterInfo(FAnimCharacterInfoBase& NewInfo)
	{
		ensureMsgf(false, TEXT("必须实现该函数！！！"));
	};

	const FAnimCharacterInfoBase& GetPrevAnimCharacterInfo()
	{
		return GetPrevAnimCharacterInfoRef();
	}

	virtual FAnimConfigBase& GetAnimConfigRef()
	{
		ensureMsgf(false, TEXT("必须实现该函数！！！"));
		static FAnimConfigBase Tmp{};
		return Tmp;
	}

	virtual void SetAnimConfig(const FAnimConfigBase* InAnimConfig) PURE_VIRTUAL(UDLAnimInstanceBaseV2::SetAnimConfig);

	const FAnimConfigBase& GetAnimConfig()
	{
		return GetAnimConfigRef();
	}

	const bool& GetIsInitialized() const
	{
		return bIsInitAnimIns;
	};

	virtual void OnUpdateAnimation(float DeltaSeconds) PURE_VIRTUAL(UDLAnimInstanceBase::TickAnimation);

	/**
	 * @brief 设置新的AnimCharacterInfo以及记录旧的信息
	 * @param DeltaSeconds
	 */
	virtual void CopyNewAnimCharacterInfo(float DeltaSeconds) PURE_VIRTUAL(UDLAnimInstanceBase::CopyNewAnimCharacterInfo);

	virtual bool OnInitAnimIns(const FDLAnimInsInitParams& InitParams);

	/**
	 * @brief 在初始化前调用
	 * @param InitParams
	 */
	virtual bool OnPreInit(const FDLAnimInsInitParams& InitParams) { return true; }

	/**
	 * @brief 在初始化完成调用
	 * @param InitParams
	 */
	virtual void OnPostInit(const FDLAnimInsInitParams& InitParams) {}

	/**
	 * @brief 在获取最新的角色信息前调用
	 * @param DeltaSeconds
	 */
	virtual bool OnPreUpdateAnimation(float DeltaSeconds) { return true; }

	/**
	 * @brief 在NextPrevAnimCharacterInfo更新前调用
	 * @param DeltaSeconds
	 */
	virtual void OnPostUpdateAnimation(float DeltaSeconds) {}

	virtual void ApplyDebugAnimCharacterInfo_Impl() {}

	virtual bool GetIsEnableAnimLog() { return false; }

	virtual void PrintLog(const FString& Log);

protected:
	UFUNCTION(BlueprintImplementableEvent)
		void OnUpdateAnimation_BP(float DeltaSeconds);

	UFUNCTION(BlueprintCallable)
		void K2_PrintAnimLog(const FString& Log);

	UFUNCTION(BlueprintNativeEvent)
		FString GetLogCategory();
	virtual FString GetLogCategory_Implementation() { return "UDLAnimInstanceBaseV2"; }

protected:
	UPROPERTY()
		TScriptInterface<ICharacterStateAccessor> CharacterStateAccessor;

	UPROPERTY()
		UDLCharacterMovementComponentBase* MovementComponentBase;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Preview")
		TSoftObjectPtr<UDLCharacterAnimationAssetBase> PreviewCharacterAnimationAsset;
#endif

private:
	bool bIsInitAnimIns = false;
};
