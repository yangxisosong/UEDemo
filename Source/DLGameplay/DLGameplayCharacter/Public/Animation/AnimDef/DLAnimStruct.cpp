// Fill out your copyright notice in the Description page of Project Settings.

#include "DLAnimStruct.h"
#include "Animation/DLAnimCommonLibrary.h"
#include "GameplayTagsManager.h"

bool FAnimBlendCurvesConfig::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;
	if (!DiagonalScaleAmountCurve)
	{
		Errors.Add(FText::FromString(TEXT("DiagonalScaleAmountCurve in AnimBlendCurvesConfig is invalid!")));
		Result = false;
	}

	if (!StrideBlend_N_Walk)
	{
		Errors.Add(FText::FromString(TEXT("StrideBlend_N_Walk in AnimBlendCurvesConfig is invalid!")));
		Result = false;
	}

	if (!StrideBlend_N_Run)
	{
		Errors.Add(FText::FromString(TEXT("StrideBlend_N_Run in AnimBlendCurvesConfig is invalid!")));
		Result = false;
	}

	if (!LandPredictionCurve)
	{
		Errors.Add(FText::FromString(TEXT("LandPredictionCurve in AnimBlendCurvesConfig is invalid!")));
		Result = false;
	}

	if (!LeanInAirCurve)
	{
		Errors.Add(FText::FromString(TEXT("LeanInAirCurve in AnimBlendCurvesConfig is invalid!")));
		Result = false;
	}

	if (!YawOffset_FB)
	{
		Errors.Add(FText::FromString(TEXT("YawOffset_FB in AnimBlendCurvesConfig is invalid!")));
		Result = false;
	}

	if (!YawOffset_LR)
	{
		Errors.Add(FText::FromString(TEXT("YawOffset_LR in AnimBlendCurvesConfig is invalid!")));
		Result = false;
	}

	return Result;
}

bool FStopAnimConfigs::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;
	if (!Forward)
	{
		Errors.Add(FText::FromString(TEXT("Forward in StopAnimConfigs is invalid!")));
		Result = false;
	}

	if (!Backward)
	{
		Errors.Add(FText::FromString(TEXT("Backward in StopAnimConfigs is invalid!")));
		Result = false;
	}

	if (!Left)
	{
		Errors.Add(FText::FromString(TEXT("Left in StopAnimConfigs is invalid!")));
		Result = false;
	}

	if (!Right)
	{
		Errors.Add(FText::FromString(TEXT("Right in StopAnimConfigs is invalid!")));
		Result = false;
	}

	return Result;
}

float FCharacterSpeedConfig::GetSpeedByDirection(const EMovementDirection Direction) const
{
	switch (Direction)
	{
	case EMovementDirection::Forward:
		return ForwardSpeed;
	case EMovementDirection::Right:
	case EMovementDirection::Left:
		return LeftAndRightSpeed;
	case EMovementDirection::Backward:
		return BackSpeed;
	}
	ensureMsgf(false, TEXT("居然还有其它的方向！！！！"));
	return 0.0f;
}

inline float DegreeToRadians(float Degree)
{
	return Degree / 180.0f * PI;
}

inline float RadiansToDegree(float Radians)
{
	return Radians / PI * 180.0f;
}

inline float EccentricAngle(float A, float B, float Radians)
{
	return FMath::Atan2(A * FMath::Sin(Radians), B * FMath::Cos(Radians));
}

inline TPair<float, float> GetEllipseXY(float A, float B, float Degree)
{
	DL_ANIM_LOG(Warning, TEXT("<GetEllipseXY> 前 A:%f,B:%f")
		, A, B);

	const float Radians = DegreeToRadians(Degree);
	const float EccAngle = EccentricAngle(A, B, Radians);
	float X = A * FMath::Cos(EccAngle);
	float Y = B * FMath::Sin(EccAngle);

	return TPair<float, float>{ X, Y };
}

float FCharacterSpeedConfig::CalculateSpeed(const FVector& Velocity, const FRotator& CharacterRotation) const
{
	// // Velocity Blend代表了每个方向的速度。使用它是为了让blend space 有更好的效果。
	// const FVector LocRelativeVelocityDir =
	// 	CharacterRotation.UnrotateVector(Velocity.GetSafeNormal(0.1f));

	// const FVector LocRelativeVelocityDir = Velocity;
	// const float Sum = FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y) +
	// 	FMath::Abs(LocRelativeVelocityDir.Z);
	// const FVector RelativeDir = LocRelativeVelocityDir / Sum;


	// const float Angle = FMath::Atan(FMath::Abs(LocRelativeVelocityDir.X / LocRelativeVelocityDir.Y));
	const FVector LocRelativeVelocityDir = CharacterRotation.UnrotateVector(Velocity.GetSafeNormal(0.1f));

	const float Sum = FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y);
	if (FMath::IsNearlyZero(Sum))
	{
		return GetSpeedByDirection(EMovementDirection::Forward);
	}
	DL_ANIM_LOG(Warning, TEXT("<CalculateVelocityBlend> Velocity:%s,Sum:%f"), *Velocity.ToString(), Sum);
	float Result = 0.0f;
	if (LocRelativeVelocityDir.X > 0.0f)
	{
		// const auto Pos = GetEllipseXY(
		// 	GetSpeedByDirection(EMovementDirection::Left)
		// 	, GetSpeedByDirection(EMovementDirection::Forward)
		// , Angle);
		//
		// Result = FMath::Sqrt(Pos.Key * Pos.Key + Pos.Value * Pos.Value);
		//
		// DL_ANIM_LOG(Warning, TEXT("<CalculateVelocityBlend> 前 Angle:%f,Pos:%s,Result:%f")
		// , Angle, *FString::Printf(TEXT("{x:%f,y:%f}"), Pos.Key, Pos.Value), Result);
		Result = (LocRelativeVelocityDir.X / Sum) * GetSpeedByDirection(EMovementDirection::Forward)
			+ (FMath::Abs(LocRelativeVelocityDir.Y) / Sum) * GetSpeedByDirection(EMovementDirection::Left);
	}
	else
	{
		// const auto Pos = GetEllipseXY(
		// 	GetSpeedByDirection(EMovementDirection::Left)
		// 	, GetSpeedByDirection(EMovementDirection::Backward)
		// , Angle);
		//
		// Result = FMath::Sqrt(Pos.Key * Pos.Key + Pos.Value * Pos.Value);
		// DL_ANIM_LOG(Warning, TEXT("<CalculateVelocityBlend> 后  Angle:%f,Pos:%s,Result:%f")
		// , Angle, *FString::Printf(TEXT("{x:%f,y:%f}"), Pos.Key, Pos.Value), Result);
		Result = (FMath::Abs(LocRelativeVelocityDir.X) / Sum) * GetSpeedByDirection(EMovementDirection::Backward)
			+ (FMath::Abs(LocRelativeVelocityDir.Y) / Sum) * GetSpeedByDirection(EMovementDirection::Left);
	}
	return Result;
	//
	// DL_ANIM_LOG(Warning, TEXT("<CalculateVelocityBlend>  LocRelativeVelocityDir:%s,RelativeDir:%s")
	// , *LocRelativeVelocityDir.ToString(), *RelativeDir.ToString());
	//
	// if (LocRelativeVelocityDir.IsNearlyZero())
	// {
	// 	return GetSpeedByDirection(EMovementDirection::Forward);
	// }
	//
	// FVelocityBlend Result;
	// Result.Forward = FMath::Clamp(RelativeDir.X, 0.0f, 1.0f);
	// Result.Backward = FMath::Abs(FMath::Clamp(RelativeDir.X, -1.0f, 0.0f));
	// Result.Left = FMath::Abs(FMath::Clamp(RelativeDir.Y, -1.0f, 0.0f));
	// Result.Right = FMath::Clamp(RelativeDir.Y, 0.0f, 1.0f);
	//
	// const float Speed = Result.Forward * GetSpeedByDirection(EMovementDirection::Forward)
	// 	- Result.Backward * GetSpeedByDirection(EMovementDirection::Backward)
	// 	- Result.Left * GetSpeedByDirection(EMovementDirection::Left)
	// 	+ Result.Right * GetSpeedByDirection(EMovementDirection::Right);
	//
	// return FMath::Clamp(FMath::Abs(Speed), GetSpeedByDirection(EMovementDirection::Backward)
	// 	, GetSpeedByDirection(EMovementDirection::Forward));
}

bool FCharacterSpeedConfig::IsValidConfig(TArray<FText>& Errors) const
{
	if (!StopSetting.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("StopSetting in CharacterSpeedConfig is invalid!")));
		return false;
	}
	return true;
}

FCharacterSpeedConfig FMovementSettings::GetSpeedForGait(const EGaitType Gait) const
{
	switch (Gait)
	{
	case EGaitType::Running:
		return Run;
	case EGaitType::Walking:
		return Walk;
	case EGaitType::Sprinting:
		return Sprint;
	default:
		ensureMsgf(false, TEXT("未知的Gait类型！！！！！！！？？？？？？"));
		return Walk;
	}
}

bool FMovementSettings::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;
	if (!Walk.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("Walk in MovementSettings is invalid!")));
		Result = false;
	}

	if (!Run.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("Run in MovementSettings is invalid!")));
		Result = false;
	}

	if (!Sprint.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("Sprint in MovementSettings is invalid!")));
		Result = false;
	}

	if (!MovementCurve)
	{
		Errors.Add(FText::FromString(TEXT("MovementCurve in MovementSettings is invalid!")));
		Result = false;
	}

	if (!RotationRateCurve)
	{
		Errors.Add(FText::FromString(TEXT("RotationRateCurve in MovementSettings is invalid!")));
		Result = false;
	}

	if (!BlendCurvesConfig.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("BlendCurvesConfig in MovementSettings is invalid!")));
		Result = false;
	}

	return Result;
}

bool FMovementStateSettings::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;
	if (!Normal.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("Walk in MovementStateSettings is invalid!")));
		Result = false;
	}

	if (!Locking.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("Locking in MovementStateSettings is invalid!")));
		Result = false;
	}

	// if (!Aiming.IsValidConfig(Errors))
	// {
	// 	Errors.Add(FText::FromString(TEXT("Aiming in MovementStateSettings is invalid!")));
	// 	Result = false;
	// }

	return Result;
}

bool FTurnInPlaceAsset::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;
	if (!Animation)
	{
		Errors.Add(FText::FromString(TEXT("Animation in TurnInPlaceAsset is invalid!")));
		Result = false;
	}
	return Result;
}

bool FAnimCharacterInfo::HasAnyTags(const FGameplayTagContainer& Tags) const
{
	return AnimCharacterStateContainer.HasAny(Tags);
}

bool FAnimCharacterInfo::HasTag(const FGameplayTag& Tag) const
{
	return AnimCharacterStateContainer.HasTag(Tag);
}

bool FAnimTurnInPlaceConfig::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;
	if (!N_TurnIP_L90.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("N_TurnIP_L90 in AnimTurnInPlaceConfig is invalid!")));
		Result = false;
	}

	if (!N_TurnIP_R90.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("N_TurnIP_R90 in AnimTurnInPlaceConfig is invalid!")));
		Result = false;
	}

	if (!N_TurnIP_L180.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("N_TurnIP_L180 in AnimTurnInPlaceConfig is invalid!")));
		Result = false;
	}

	if (!N_TurnIP_R180.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("N_TurnIP_R180 in AnimTurnInPlaceConfig is invalid!")));
		Result = false;
	}
	return Result;
}

FAnimSkeletonConfig::FAnimSkeletonConfig()
{
	//初始化默认骨骼映射
	// const int32 MaxNum = StaticEnum<EDLBoneName>()->NumEnums() - 1;
	// for (int32 i = 0; i < MaxNum; ++i)
	// {
	// 	Bones.Emplace(static_cast<EDLBoneName>(i), FName(StaticEnum<EDLBoneName>()->GetNameStringByIndex(i)));
	// }
	const auto Result = UGameplayTagsManager::Get().FindTagNode(TEXT("BoneName"));
	if (Result.IsValid())
	{
		const auto Children = Result.Get()->GetChildTagNodes();
		for (const auto& Tag : Children)
		{
			Bones.Emplace(Tag->GetCompleteTag(), Tag.Get()->GetSimpleTagName());
		}
	}
}

const FName FAnimSkeletonConfig::GetBoneByTag(const FGameplayTag Tag) const
{
	if (ensureMsgf(Bones.Contains(Tag), TEXT("找不到骨骼%s！！！！！！"), *Tag.GetTagName().ToString()))
	{
		return *(Bones.Find(Tag));
	}
	return	NAME_None;
}

bool FRollConfig::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;
	if (!Forward)
	{
		Errors.Add(FText::FromString(TEXT("Forward in RollConfig is invalid!")));
		Result = false;
	}

	if (!Backward)
	{
		Errors.Add(FText::FromString(TEXT("Backward in RollConfig is invalid!")));
		Result = false;
	}

	if (!Left)
	{
		Errors.Add(FText::FromString(TEXT("Left in RollConfig is invalid!")));
		Result = false;
	}

	if (!Right)
	{
		Errors.Add(FText::FromString(TEXT("Right in RollConfig is invalid!")));
		Result = false;
	}

	if (!StepBack)
	{
		Errors.Add(FText::FromString(TEXT("StepBack in RollConfig is invalid!")));
		Result = false;
	}

	return Result;
}

bool FLandConfig::IsValidConfig(TArray<FText>& Errors) const
{
	if (!LandRollMontage)
	{
		Errors.Add(FText::FromString(TEXT("LandRollMontage in LandConfig is invalid!")));
		return false;
	}

	return true;
}

FAnimConfig::FAnimConfig()
{
	//初始化动画槽位
	// const int32 MaxNum = StaticEnum<EDLAnimName>()->NumEnums() - 1;
	// for (int32 i = 0; i < MaxNum; ++i)
	// {
	// 	Animations.Emplace(static_cast<EDLAnimName>(i), FDLAnimAsset());
	// }
	// const auto Result = UGameplayTagsManager::Get().FindTagNode(TEXT("AnimName"));
	// if (Result.IsValid())
	// {
	// 	const auto Children = Result.Get()->GetChildTagNodes();
	// 	for (const auto& Tag : Children)
	// 	{
	// 		Animations.Emplace(Tag->GetCompleteTag(), FDLAnimAsset());
	// 	}
	// }
}

const FAnimBlendCurvesConfig& FAnimConfig::GetBlendCurveConfigByState(const ECharacterMainState State) const
{
	switch (State)
	{
	case ECharacterMainState::Normal:
		return MovementStateSettings.Normal.BlendCurvesConfig;
	case ECharacterMainState::Locking:
		return MovementStateSettings.Locking.BlendCurvesConfig;
	// case ECharacterMainState::Aiming:
	// 	return MovementStateSettings.Aiming.BlendCurvesConfig;
	default:
		ensureMsgf(false, TEXT("居然还有其他的类型！！！！"));
		return MovementStateSettings.Normal.BlendCurvesConfig;
	}
}

bool FAnimConfig::IsValidConfig(TArray<FText>& Errors) const
{
	bool Result = true;
	if (!MovementStateSettings.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("MovementStateSettings in AnimConfig is invalid!")));
		Result = false;
	}
	if (!TurnInPlaceConfig.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("TurnInPlaceConfig in AnimConfig is invalid!")));
		Result = false;
	}
	if (!RollConfig.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("RollConfig in AnimConfig is invalid!")));
		Result = false;
	}

	if (!LandConfig.IsValidConfig(Errors))
	{
		Errors.Add(FText::FromString(TEXT("LandConfig in AnimConfig is invalid!")));
		Result = false;
	}

	return Result;
}

