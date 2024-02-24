// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimDef/DLAnimStruct.h"
#include "Animation/AnimInstance.h"
#include "DLAnimInstanceBase.generated.h"

class UDLCharacterMovementComponentBase;
class ICharacterStateAccessor;
class AAnimationTestCharacter;
class UBlendSpace;
class UBlendSpace1D;
class ACharacter;
class UDLAnimComponentBase;
class UAnimMontage;

UCLASS(meta = (BlueprintThreadSafe))
class DLGAMEPLAYCHARACTER_API UDLAnimInstanceBase 
	: public UAnimInstance
{
	GENERATED_BODY()

public:

	/**
	 * @brief 获取角色状态的接口，角色的 Gameplay 状态都应该从这个接口获取
	 * @return
	 */
	UFUNCTION(BlueprintPure)
		TScriptInterface<ICharacterStateAccessor> GetCharacterStateAccessor() const;

	/**
	 * @brief 获取移动组件
	 * @return
	 * @note  在使用时 根据设计，禁止Cast为他的子类  而是应该依赖于 子类特定的接口
	 * @see UDLCharacterMovementComponentBase 
	 */
	UFUNCTION(BlueprintPure)
		UDLCharacterMovementComponentBase* GetMovementComponentBase() const;

protected:

#pragma region UAnimInstance

#if WITH_EDITOR
	/*开启AnimGraph的FastPath变量优化警告*/
	virtual bool PCV_ShouldWarnAboutNodesNotUsingFastPath() const override final { return true; }

	virtual bool PCV_ShouldNotifyAboutNodesNotUsingFastPath() const override final { return true; }
#endif

	virtual void NativeInitializeAnimation() override final;

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override final;

#pragma endregion

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation | Config")
		FORCEINLINE FName GetBoneByTag(const FGameplayTag Name)
	{
		return AnimConfig.SkeletonConfig.GetBoneByTag(Name);
	}

	// UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation | Config")
	// 	const FDLAnimAsset GetAnimAssetByTag(const FGameplayTag Name) const
	// {
	// 	if (AnimConfig.Animations.Contains(Name))
	// 	{
	// 		return *(AnimConfig.Animations.Find(Name));
	// 	}
	// 	else
	// 	{
	// 		return FDLAnimAsset();
	// 	}
	// }

	// UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation | Config")
	// 	UBlendSpace1D* GetBlendSpace1DByTag(const FGameplayTag Name) const;
	//
	// UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation | Config")
	// 	UBlendSpace* GetBlendSpaceByTag(const FGameplayTag Name) const;
	//
	// UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation | Config")
	// 	UAnimSequence* GetAnimSeqByTag(const FGameplayTag Name) const;
	//
	// UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation | Config")
	// 	UAnimMontage* GetAnimMontageByTag(const FGameplayTag Name) const;

	UFUNCTION(BlueprintPure, Category = "Animation | Curve")
		float GetCurveValueByEnum(const EDLAnimCurveName Curve) const;


	/*调试函数*/
	UFUNCTION(CallInEditor)
		void ApplyDebugAnimCharacterInfo();

	UFUNCTION(CallInEditor)
		void LoadDebugAnimConfig();

protected:
	UDLAnimComponentBase* GetAnimCompFromOwner() const;

	AAnimationTestCharacter* GetOwnerCharacter() const;

protected:
	virtual void OnUpdateAnimation(float DeltaSeconds) PURE_VIRTUAL(UDLAnimInstanceBase::TickAnimation);

	virtual void OnInitAnimIns();

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Config")
		FAnimConfig AnimConfig;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Control")
		FAnimCharacterInfo AnimCharacterInfo;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Control")
		FAnimCharacterInfo PrevAnimCharacterInfo;

	UPROPERTY(EditAnywhere)
		FString DebugAnimCharacterInfo;

	UPROPERTY(EditAnywhere)
		FString DebugAnimConfigName;


	UPROPERTY()
		TScriptInterface<ICharacterStateAccessor> CharacterStateAccessor;

	UPROPERTY()
		UDLCharacterMovementComponentBase* MovementComponentBase;

private:
	void CopyCharacterInfoFromAnimComp();

private:
	FAnimCharacterInfo NextPrevAnimCharacterInfo;
};