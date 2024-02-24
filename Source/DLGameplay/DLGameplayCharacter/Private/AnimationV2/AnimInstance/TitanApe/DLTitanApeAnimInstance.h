// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLTitanApeAnimDef.h"
#include "Animation/DLMainAnimInstanceV2.h"
#include "AnimationV2/AnimInstance/Human/DLHumanAnimDef.h"
#include "DLTitanApeAnimInstance.generated.h"

/**
 * 泰坦巨猿动画实例
 */
UCLASS()
class DLGAMEPLAYCHARACTER_API UDLTitanApeAnimInstance : public UDLMainAnimInstanceV2
{
	GENERATED_BODY()
private:
	friend class UDLHumanAnimInsHelper;

protected:
	//UDLAnimInstanceBaseV2 implements

	virtual FAnimCharacterInfoBase& GetAnimCharacterInfoRef() override
	{
		return AnimCharacterInfo;
	};

	virtual FAnimCharacterInfoBase& GetPrevAnimCharacterInfoRef() override
	{
		return PrevAnimCharacterInfo;
	};

	virtual void CopyPrevAnimCharacterInfo(FAnimCharacterInfoBase& NewInfo) override
	{
		static_cast<FAnimCharacterInfoBase>(PrevAnimCharacterInfo) = NewInfo;
	}

	virtual FAnimConfigBase& GetAnimConfigRef() override
	{
		return AnimConfig;
	}

	virtual void SetAnimConfig(const FAnimConfigBase* InAnimConfig) override
	{
		AnimConfig = *(static_cast<const FTitanApeAnimConfig*> (InAnimConfig));
	};

	virtual void CopyNewAnimCharacterInfo(float DeltaSeconds) override;

	virtual void OnUpdateAnimation(float DeltaSeconds) override;

	virtual void OnPostUpdateAnimation(float DeltaSeconds) override;

	virtual bool GetIsEnableAnimLog() override;

	virtual FString GetLogCategory_Implementation() override { return "UDLTitanApeAnimInstance"; }

	virtual void PrintLog(const FString& Log) override;

	virtual bool ShouldMoveCheck() override;

	virtual void OnPostInit(const FDLAnimInsInitParams& InitParams) override;

	virtual	bool CanTurnInPlace();

	virtual void TurnInPlaceCheck(float DeltaSeconds);

private:
	void UpdateMovementState(float DeltaSeconds);

	void UpdateMovementValues(float DeltaSeconds);

	void UpdateRotationValues();

	void UpdateAimingValues(float DeltaSeconds);

	void SetMainState(FHumanAnimCharacterInfo& NewInfo);

	void SetGait(FHumanAnimCharacterInfo& NewInfo);

	void SetTargetRotation(FHumanAnimCharacterInfo& NewInfo);

	void SetMovementState(FHumanAnimCharacterInfo& NewInfo);

	void SetMovementDirection(FHumanAnimCharacterInfo& NewInfo);

	void SetVelocityRelated(FHumanAnimCharacterInfo& NewInfo, float DeltaSeconds);

	void SetMovementAction(FHumanAnimCharacterInfo& NewInfo);

	void SetSpecialState(FHumanAnimCharacterInfo& NewInfo);

	UFUNCTION()
		void OnLanded(const FHitResult& Hit);

	void OnLandFrictionReset() const;

	void TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool OverrideCurrent);

	const FDLAnimTurnInPlaceConfig& GetCurTurnInPlaceConfig() const;
public:
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		FRotator LastMovementInputRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		FRotator LastVelocityRotation = FRotator::ZeroRotator;

	// UPROPERTY(BlueprintReadOnly, Category = "Movement")
	// 	FRotator TargetRotation = FRotator::ZeroRotator;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Info")
		FHumanAnimCharacterInfo AnimCharacterInfo;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Info")
		FHumanAnimCharacterInfo PrevAnimCharacterInfo;

	UPROPERTY(BlueprintReadOnly, Category = "Animation | Config")
		FTitanApeAnimConfig AnimConfig;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Info")
		FAnimVelocityBlend VelocityBlend;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Animation | Info")
		FAnimGraphGroundedValues GroundedValues;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Info")
		FVector RelativeAcceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Info")
		FBodyLeanAmount LeanAmount;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Info")
		FAnimGraphInAirValues InAirValues;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Info")
		FAnimGroundedEntryState GroundedEntryState;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Info")
		FDLAnimGraphAimingValues AimingValues;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Info")
		float FlailRate = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Info | TurnInPlace")
		float ElapsedDelayTime = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Animation | Info")
		FGameplayTagContainer CurrentCharacterState;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Info")
		bool bIsTurning = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Move")
		FVector2D WalkRushBlend = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation | Move")
		FVector2D LastWalkRushBlend = FVector2D::ZeroVector;
protected:
	FVector CurrentAcceleration = FVector::ZeroVector;
	float EasedMaxAcceleration = 0.0f;
	FVector PreviousVelocity = FVector::ZeroVector;

private:
	FHumanAnimCharacterInfo NextPrevAnimCharacterInfo;

	FTimerHandle OnLandedFrictionResetTimer;

	bool PrevIsTurning = false;

	float PrevBrakingFrictionFactor = 0.0f;
};
