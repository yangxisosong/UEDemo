#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToActorForce.h"
#include "RootMotionDataAsset.generated.h"

UENUM()
enum class ERootMotionType : uint8
{
	ToActor,
	ToLocation,
	Jump,
};

UCLASS(BlueprintType)
class  UDataAsset_RootMotion
	: public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Base")
	ERootMotionType Type = ERootMotionType::ToActor;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Base")
	float Duration = 1;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Base")
	UCurveVector* PathOffsetCurve = nullptr;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Base",
		meta=(EditCondition = "Type != ERootMotionType::ToLocation", EditConditionHides))
	UCurveFloat* TimeMappingCurve= nullptr;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Locked",
		meta=(EditCondition = "Type != ERootMotionType::Jump", EditConditionHides))
	FVector TargetLocationOffset = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Locked",
		meta=(EditCondition = "Type != ERootMotionType::Jump", EditConditionHides))
	ERootMotionMoveToActorTargetOffsetType OffsetAlignment = ERootMotionMoveToActorTargetOffsetType::AlignFromTargetToSource;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Locked",
		meta=(EditCondition = "Type != ERootMotionType::Jump", EditConditionHides))
	float LockMaxDistance = 100;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Locked",
	meta=(EditCondition = "Type != ERootMotionType::Jump", EditConditionHides))
	bool IgnoreZ = true;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Unlock")
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Unlock")
	float Distance = 100;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Unlock",
		meta=(EditCondition = "Type == ERootMotionType::Jump", EditConditionHides))
	float Height = 100;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Unlock",
		meta=(EditCondition = "Type == ERootMotionType::Jump", EditConditionHides))
	float landedTriggerTime = 0;
};

