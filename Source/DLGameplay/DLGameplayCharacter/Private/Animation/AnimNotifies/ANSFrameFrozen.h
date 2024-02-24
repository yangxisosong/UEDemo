// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANSFrameFrozen.generated.h"

/*
 * 定帧部位
 */
UENUM(BlueprintType)
enum class EFrameFrozenPart : uint8
{
	None		UMETA(ToolTip = "无"),
	RightArm	UMETA(ToolTip = "右手臂"),
	LeftArm		UMETA(ToolTip = "左手臂"),
	RightLeg	UMETA(ToolTip = "右腿"),
	LeftLeg		UMETA(ToolTip = "左腿"),
};

/**
* 定帧模式
*/
UENUM(BlueprintType)
enum class EFrameFrozenMode : uint8
{
	None	UMETA(ToolTip = "无"),
	FullSlomo	UMETA(ToolTip = "整体统一Slomo"),
	PartialSlomo	UMETA(ToolTip = "分部位Slomo")
};

USTRUCT(BlueprintType)
struct FFrameFrozenInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		UAnimSequence* CurAnimSeq = nullptr;

	UPROPERTY(BlueprintReadWrite)
		float StartTimeOffset = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		float EndTimeOffset = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		TSet<EFrameFrozenPart> FrameFrozenParts;

	UPROPERTY(BlueprintReadWrite)
		EFrameFrozenMode FrameFrozenMode = EFrameFrozenMode::None;

	UPROPERTY(BlueprintReadWrite)
		UCurveFloat* TargetPartialCurve = nullptr;
};

/**
 * 顿帧状态通知
 */
UCLASS(Abstract)
class DLGAMEPLAYCHARACTER_API UANSFrameFrozen : public UAnimNotifyState
{
	GENERATED_BODY()

protected:

#if WITH_EDITOR
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent) override;
#endif

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual FString GetNotifyName_Implementation() const override;

protected:
	UPROPERTY(BlueprintReadOnly)
		UAnimSequence* CurAnimSeq = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float StartTimeOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UCurveFloat* FullBodySlomoCurve = nullptr;

	UPROPERTY(BlueprintReadOnly)
		float EndTimeOffset = 0.0f;

	// SocketName to attach to
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	// 	FName SocketName;

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		EFrameFrozenMode FrameFrozenMode = EFrameFrozenMode::None;

private:
	float TimeOffset = 0.0f;
	float DefaultPlayRate = 1.0f;
};
