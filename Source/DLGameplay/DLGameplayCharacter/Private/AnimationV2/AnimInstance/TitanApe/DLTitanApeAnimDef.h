// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationV2/DLAnimCommonDef.h"
#include "AnimationV2/AnimInstance/Human/DLHumanAnimDef.h"
#include "AnimationV2/AnimationTags.h"
#include "DLTitanApeAnimDef.generated.h"

USTRUCT(BlueprintType)
struct FTitanApeAnimConfig :public FAnimConfigBase
{
	GENERATED_BODY()
public:
	/*移动状态设置*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FDLAnimMovementSettings MovementStateSettings;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float VelocityBlendInterpSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float GroundedLeanInterpSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float GroundedLeanScale = 1.0f;

	/*行走动画设计时速*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
		float AnimatedWalkSpeed = 150.0f;

	/*小跑动画设计时速*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
		float AnimatedJogSpeed = 350.0f;

	/*冲刺动画设计时速*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
		float AnimatedSprintSpeed = 600.0f;

	/*最小触发移动的速度*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
		float MinMoveThreshold = 50.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FAnimLandConfig LandConfig;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "TurnInPlace")
		FDLAnimTurnInPlaceConfig TurnInPlaceConfig_Stage_1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "TurnInPlace")
		FDLAnimTurnInPlaceConfig TurnInPlaceConfig_Stage_2_3;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		FHumanIKConfig IKConfig;
public:

	virtual bool IsValidConfig(TArray<FText>& Errors) const override;
};


namespace EAnimationTag
{
	namespace ECharacterState
	{
		namespace ENPC
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stage_1);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stage_2);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stage_3);

			namespace EDefense
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stage_1);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stage_2);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stage_3);
			}
		}
	}
}

