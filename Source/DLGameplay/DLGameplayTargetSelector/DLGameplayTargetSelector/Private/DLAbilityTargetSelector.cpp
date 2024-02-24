#include "DLAbilityTargetSelector.h"

#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogDLGameplayTargetSelector);


void UDLGameplayTargetSelectorBase::CalculateTargetUnits(
	const AActor* OriginalActor,
	const FString& CalculateMsg,
	bool& Succeed,
	TArray<FHitResult>& RetUnits, FGameplayTargetSelectorExtendData ExtendData)
{
	TGuardValue<TWeakObjectPtr<AActor>> GuardOriginActor(TempOriginActor, const_cast<AActor*>(OriginalActor));
	Succeed = false;
	FTransform Transform;
	const bool Ok = CalculateAnchorTransform(OriginalActor, CalculateMsg, Transform);
	if (Ok)
	{
		if (ExtendData.bOverrideRotation)
		{
			Transform.SetRotation(ExtendData.OverrideRotation.Quaternion());
		}

		if (ExtendData.bOverrideLocation)
		{
			Transform.SetLocation(ExtendData.OverrideLocation);
		}

		this->K2_CalculateTargetUnits(OriginalActor, Transform, CalculateMsg, Succeed, RetUnits);
	}
}

void UDLGameplayTargetSelectorBase::CalculateTargetUnitsWithTransform(const UObject* WorldContext,
                                                                      const FTransform& OriginalTransform,
                                                                      const FString& CalculateMsg, bool& Succeed,
                                                                      TArray<FHitResult>& RetUnits)
{
	TGuardValue<TWeakObjectPtr<AActor>> GuardOriginActor(TempOriginActor, nullptr);
	this->K2_CalculateTargetUnits(WorldContext, OriginalTransform, CalculateMsg, Succeed, RetUnits);
}


void UDLGameplayTargetSelectorBase::CalculateTargetTransformsWithTransform(const UObject* WorldContext,
                                                                           const FTransform& OriginalTransform,
                                                                           const FString& CalculateMsg, bool& Succeed,
                                                                           TArray<FTransform>& RetTransform)
{
	TGuardValue<TWeakObjectPtr<AActor>> GuardOriginActor(TempOriginActor, nullptr);
	this->K2_CalculateTargetTransforms(WorldContext, OriginalTransform, CalculateMsg, Succeed, RetTransform);
}

void UDLGameplayTargetSelectorBase::CalculateTargetTransforms(
	const AActor* OriginalActor,
	const FString& CalculateMsg,
	bool& Succeed,
	TArray<FTransform>& RetPoints,
	FGameplayTargetSelectorExtendData ExtendData)
{
	TGuardValue<TWeakObjectPtr<AActor>> GuardOriginActor(TempOriginActor, const_cast<AActor*>(OriginalActor));
	Succeed = false;
	FTransform Transform;
	const bool Ok = CalculateAnchorTransform(OriginalActor, CalculateMsg, Transform);
	if (Ok)
	{
		if (ExtendData.bOverrideRotation)
		{
			Transform.SetRotation(ExtendData.OverrideRotation.Quaternion());
		}

		if (ExtendData.bOverrideLocation)
		{
			Transform.SetLocation(ExtendData.OverrideLocation);
		}

		this->K2_CalculateTargetTransforms(OriginalActor, Transform, CalculateMsg, Succeed, RetPoints);
	}
}

void UDLGameplayTargetSelectorBase::CalculateTargetLocationWithTransform(
	const UObject* WorldContext, const FTransform& OriginalTransform,
	const FString& CalculateMsg, bool& Succeed, TArray<FVector>& RetPoints)
{
	static TArray<FTransform> Temp;
	Temp.Empty();

	RetPoints.Empty();

	CalculateTargetTransformsWithTransform(WorldContext, OriginalTransform, CalculateMsg, Succeed, Temp);
	if (Succeed)
	{
		for (const auto& Value : Temp)
		{
			RetPoints.Add(Value.GetLocation());
		}
	}
}

void UDLGameplayTargetSelectorBase::CalculateTargetLocation(const AActor* OriginalActor, const FString& CalculateMsg,
                                                            bool& Succeed, TArray<FVector>& RetPoints, FGameplayTargetSelectorExtendData ExtendData)
{
	static TArray<FTransform> Temp;
	Temp.Empty();

	RetPoints.Empty();

	CalculateTargetTransforms(OriginalActor, CalculateMsg, Succeed, Temp, ExtendData);
	if (Succeed)
	{
		for (const auto& Value : Temp)
		{
			RetPoints.Add(Value.GetLocation());
		}
	}
}


const ACharacter* UDLGameplayTargetSelectorBase::GetAnchorCharacter(const AActor* OriginalActor,
                                                                    const FString& CalculateMsg)
{
	if (OriginalActor->IsA<ACharacter>())
	{
		return Cast<ACharacter>(OriginalActor);
	}

	ensureAlwaysMsgf(false, TEXT("不是Character  [%s]"), *CalculateMsg);

	return nullptr;
}


bool UDLGameplayTargetSelectorBase::CalculateAnchorTransform(const AActor* OriginalActor, const FString& CalculateMsg,
	FTransform& OutTransform)
{
	OutTransform = FTransform::Identity;

	FTransform CharacterTransform;
	CharacterTransform = FTransform::Identity;

	if (AnchorSelectType == EAnchorSelectType::ActorTransform)
	{
		CharacterTransform = OriginalActor->GetActorTransform();
	}
	else
	{
		const ACharacter* AnchorCharacter = GetAnchorCharacter(OriginalActor, CalculateMsg);
		if (!AnchorCharacter)
		{
			UE_LOG(LogDLGameplayTargetSelector, Warning, TEXT("无法获取锚点的Object对象  [%s]"), *CalculateMsg);
			return false;
		}

		CharacterTransform = AnchorCharacter->GetMesh()->GetSocketTransform(AnchorSocketName);
	}

	OutTransform = CharacterTransform;


	return true;
}

TArray<FString> UDLGameplayTargetSelectorBase::GetSockeNames() const
{
#if WITH_EDITOR
	const bool IsPreviewMesh = this->PreviewMesh.Get() != nullptr &&
		this->PreviewMesh.Get()->GetRenderAssetType() == EStreamableRenderAssetType::SkeletalMesh;
	if (IsPreviewMesh)
	{
		//USkeletalMesh
		USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(this->PreviewMesh.Get());
		if (SkeletalMesh)
		{
			const int32 SocketsNum = SkeletalMesh->NumSockets();
			TArray<FString> NameArray;
			for (int i = 0; i < SocketsNum; ++i)
			{
				const auto& TempSocket = SkeletalMesh->GetSocketByIndex(i);
				NameArray.Add(TempSocket->SocketName.ToString());
			}

			const auto& AllBonesInfo = SkeletalMesh->GetRefSkeleton().GetRefBoneInfo();
			for (const auto& Bone : AllBonesInfo)
			{
				NameArray.Add(Bone.Name.ToString());
			}
			return NameArray;
		}
	}
#endif
	return { "" };
}


void UDLGameplayTargetSelectorCoordinateSimple::K2_CalculateTargetTransforms_Implementation(
	const UObject* WorldContext,
	const FTransform& InAnchorTransform,
	const FString& CalculateMsg,
	bool& Succeed,
	TArray<FTransform>& RetPoints)
{
	RetPoints.Empty();
	RetPoints.Add(RelativeTransform * InAnchorTransform);
	Succeed = true;
}

void UDLTargetSelectorRevisedByDistance::Revised(const FTransform& InCoordinate)
{
	RevisedOffset = FTransform::Identity;
	FVector RandomDirection(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f),
	                        FMath::RandRange(-1.0f, 1.0f));
	RandomDirection.Normalize();
	const float RandomLength = FMath::RandRange(MinDistance, MaxDistance);

	FTransform Result = InCoordinate;
	const FVector Center = InCoordinate.GetLocation();
	Result.SetLocation(Center + RandomDirection * RandomLength);
	RevisedOffset.SetLocation(RandomDirection * RandomLength);

	//return Result;
}

void FDLTargetSelectorCoordinateInfo::SetRevisedTransform()
{
	if (bOverrideGlobalRevised && Revised)
	{
		Revised->Revised(Coordinate);
		RevisedOffset = Revised->RevisedOffset;
		//return Result;
	}
	RevisedOffset = FTransform::Identity;
	//return Coordinate;
}

FTransform FDLTargetSelectorCoordinateInfo::GetFinalTransform() const
{
	FTransform Result = Coordinate;
	Result.SetLocation(Coordinate.GetLocation() + RevisedOffset.GetLocation());
	return Result;
}

void UDLTargetSelectorRevisedCoordinate::Revised(const FTransform& InCoordinate)
{
	RevisedOffset = FTransform::Identity;
	//return InCoordinate;
}

void UDLGameplayTargetSelectorCoordinateCollection::K2_CalculateTargetTransforms_Implementation(
	const UObject* WorldContext, const FTransform& InAnchorTransform, const FString& CalculateMsg, bool& Succeed,
	TArray<FTransform>& RetPoints)
{
	RetPoints.Empty();
	RevisedTransform();
	for (auto& P : RelativeTransformArray)
	{
		FTransform ResultTransform;
		ResultTransform.SetLocation(InAnchorTransform.TransformPositionNoScale(P.GetFinalTransform().GetLocation()));
		ResultTransform.SetRotation(InAnchorTransform.GetRotation() * P.Coordinate.GetRotation());
		RetPoints.Add(ResultTransform);
	}
	Succeed = RetPoints.Num() != 0;
}

void UDLGameplayTargetSelectorCoordinateCollection::RevisedTransform()
{
	for (auto& i : RelativeTransformArray)
	{
		if (GlobalRevised)
		{
			if (i.bOverrideGlobalRevised)
			{
				i.SetRevisedTransform();
			}
			else
			{
				GlobalRevised->Revised(i.Coordinate);
				i.RevisedOffset = GlobalRevised->RevisedOffset;
			}
		}
		else
		{
			i.SetRevisedTransform();
		}
	}
}

FTransform UDLGameplayTargetSelectorCoordinateCollection::GetFinalTransformByIndex(int32 Index)
{
	if(RelativeTransformArray.IsValidIndex(Index))
	{
		return RelativeTransformArray[Index].GetFinalTransform();
	}
	return FTransform::Identity;
}

FTransform UDLGameplayTargetSelectorCoordinateCollection::GetRevisedOffsetByIndex(int32 Index)
{
	if (RelativeTransformArray.IsValidIndex(Index))
	{
		return RelativeTransformArray[Index].RevisedOffset;
	}
	return FTransform::Identity;
}

void UDLGameplayTargetSelectorCoordinateCollection::ClearRevisedOffset()
{
	if (GlobalRevised)
	{
		GlobalRevised->RevisedOffset = FTransform::Identity;
	}

	for (auto& i : RelativeTransformArray)
	{
		i.RevisedOffset = FTransform::Identity;
		if (i.Revised)
		{
			i.Revised->RevisedOffset = FTransform::Identity;
		}
	}
}

TArray<FTransform> UDLGameplayTargetSelectorDynamicGeneration::CreatRandomCoordinate()
{
	TArray<FTransform> DotArray;
	const auto CreatNum = CoordinateNum;
	switch (ShapeType)
	{
	case ECoordinateConstraintShape::Cylinder:
		if(IsOpenRandomOffset)
		{
			DotArray = CreateRandomCoordinateByCylinder(CreatNum);
		}
		else
		{
			DotArray = CreateCoordinateByCylinder(CreatNum);
		}
		break;
	case ECoordinateConstraintShape::Box:
		DotArray = CreateRandomCoordinateByBox(CreatNum);
		break;
	case ECoordinateConstraintShape::Sphere:
		DotArray = CreateRandomCoordinateBySphere(CreatNum);
		break;
	default:
		break;
	}

	UE_LOG(LogTemp, Warning, TEXT("CreatCoordinateDot Num = %d"), DotArray.Num());

	if (DotArray.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreatCoordinateDot Failed"));
	}

	return DotArray;
}

void UDLGameplayTargetSelectorDynamicGeneration::K2_CalculateTargetTransforms_Implementation(
	const UObject* WorldContext, const FTransform& InAnchorTransform, const FString& CalculateMsg, bool& Succeed,
	TArray<FTransform>& RetPoints)
{
	RetPoints.Empty();
	for (auto& P : RelativeTransformArray)
	{
		auto PointsTransform = CreatRandomCoordinate();
		auto LocalTrans= P.GetFinalTransform();

		auto WorldTransform = InAnchorTransform.TransformPositionNoScale(LocalTrans.GetLocation());
		for (auto& Item : PointsTransform)
		{
			FTransform Transform = InAnchorTransform;
			auto Rotation = InAnchorTransform.GetRotation() * P.Coordinate.GetRotation();
			const FVector RotateVector = UKismetMathLibrary::Quat_RotateVector(Rotation, Item.GetLocation());
			Transform.SetLocation(WorldTransform + RotateVector);
			Transform.SetRotation(Rotation);
			RetPoints.Add(Transform);
		}
	}


	Succeed = true;
}

TArray<FTransform> UDLGameplayTargetSelectorDynamicGeneration::CreateRandomCoordinateByBox(const int32 CreatNum)
{
	TArray<FTransform> Result;

	const int32 XMaxBlockNum = FMath::Floor((MaxLength) / (MinDistanceToDot * 2));
	const int32 YMaxBlockNum = FMath::Floor((MaxWidth) / (MinDistanceToDot * 2));
	const int32 ZMaxBlockNum = FMath::Floor((MaxHeight) / (MinDistanceToDot * 2));


	const int32 XMinBlockNum = FMath::Floor((MinLength) / (MinDistanceToDot * 2));
	const int32 YMinBlockNum = FMath::Floor((MinWidth) / (MinDistanceToDot * 2));
	const int32 ZMinBlockNum = FMath::Floor((MinHeight) / (MinDistanceToDot * 2));

	TArray<FVector> PosArry;
	for (int32 x = 0; x < XMaxBlockNum; x++)
	{
		for (int32 y = 0; y < YMaxBlockNum; y++)
		{
			for (int32 z = 0; z < ZMaxBlockNum; z++)
			{
				if ((x >= XMinBlockNum || y >= YMinBlockNum || z >= ZMinBlockNum) && x <= XMaxBlockNum && y <= YMaxBlockNum
					&& z <= ZMaxBlockNum)
				{
					PosArry.Add(FVector(x, y, z));

					const int32 Index = FMath::RandRange(0, PosArry.Num() - 1);

					PosArry.Swap(Index, PosArry.Num() - 1);
				}
			}
		}
	}


	MaxLimitNum = PosArry.Num();
	UE_LOG(LogTemp, Warning, TEXT("CreatCoordinateDot MaxSiz=%d"), MaxLimitNum);
	const int32 LoopNum = FMath::Min(MaxLimitNum, CreatNum);

	for (int32 i = 0; i < LoopNum; i++)
	{
		FTransform TempTransform;
		FVector Location;
		const int32 XSymbol = FMath::RandBool() ? 1 : -1;
		const int32 YSymbol = FMath::RandBool() ? 1 : -1;
		const int32 ZSymbol = FMath::RandBool() ? 1 : -1;

		Location.X = (MinDistanceToDot * PosArry[i].X * 2) * XSymbol;
		Location.Y = (MinDistanceToDot * PosArry[i].Y * 2) * YSymbol;
		Location.Z = (MinDistanceToDot * PosArry[i].Z * 2) * ZSymbol;

		TempTransform.SetLocation(Location);
		Result.Add(TempTransform);
	}

	return Result;
}

TArray<FTransform> UDLGameplayTargetSelectorDynamicGeneration::CreateRandomCoordinateByCylinder(int32 CreatNum)
{
	TArray<FTransform> Result;

	//需要绕几圈
	int32 XNum = (MaxWidth - MinWidth) / (MinDistanceToDot);
	XNum = FMath::Max(1, XNum);
	if ((MaxWidth - MinWidth) >= MinDistanceToDot)
	{
		XNum++;
	}

	//有几层
	int32 HeightBlockNum = FMath::Floor(MinHeight / (MinDistanceToDot));
	HeightBlockNum = FMath::Max(1, HeightBlockNum);
	if ((MinHeight) >= MinDistanceToDot)
	{
		HeightBlockNum++;
	}

	TArray<FVector> PosArry;

	for (int32 x = 0; x < XNum; x++)
	{
		const int32 NewWidth = MinWidth + x * MinDistanceToDot;
		float Radian = FMath::Asin(MinDistanceToDot / (2 * NewWidth));
		//每圈有几个位置
		const int32 WidthBlockNum = FMath::Floor((PI * 2) / (Radian * 2));
		float RandNum = FMath::RandRange(0.0f, 3.0f);
		//Radian += RandNum;
		for (int32 y = 0; y < WidthBlockNum; y++)
		{
			for (int32 z = 0; z < HeightBlockNum; z++)
			{
				FVector Pos;

				float WidthRand = 0;
				WidthRand = FMath::RandRange(RandomOffset.X, RandomOffset.Y);

				Pos.X = (NewWidth + WidthRand) * FMath::Sin(y * Radian*2 + RandNum);
				Pos.Y = (NewWidth + WidthRand) * FMath::Cos(y * Radian*2 + RandNum);
				Pos.Z = z * MinDistanceToDot;

				PosArry.Add(Pos);

				const int32 Index = FMath::RandRange(0, PosArry.Num() - 1);

				PosArry.Swap(Index, PosArry.Num() - 1);
			}
		}
	}

	MaxLimitNum = PosArry.Num();
	UE_LOG(LogTemp, Warning, TEXT("CreatCoordinateDot MaxSiz=%d"), MaxLimitNum);

	const int32 LoopNum = FMath::Min(MaxLimitNum, CreatNum);

	for (int32 i = 0; i < LoopNum; i++)
	{
		FTransform TempTransform;
		FVector Location = PosArry[i];

		TempTransform.SetLocation(Location);
		Result.Add(TempTransform);
	}
	return Result;
}

TArray<FTransform> UDLGameplayTargetSelectorDynamicGeneration::CreateCoordinateByCylinder(int32 CreatNum)
{
	TArray<FTransform> Result;

	//需要绕几圈
	int32 XNum = (MaxWidth - MinWidth) / (MinDistanceToDot);
	XNum = FMath::Max(1, XNum);
	if ((MaxWidth - MinWidth) >= MinDistanceToDot)
	{
		XNum++;
	}

	//有几层
	int32 HeightBlockNum = FMath::Floor(MinHeight / (MinDistanceToDot));
	HeightBlockNum = FMath::Max(1, HeightBlockNum);
	if ((MinHeight) >= MinDistanceToDot)
	{
		HeightBlockNum++;
	}

	TArray<FVector> PosArry;

	for (int32 x = 0; x < XNum; x++)
	{
		const int32 NewWidth = MinWidth + x * MinDistanceToDot;
		float Radian = FMath::Asin(MinDistanceToDot / (2 * NewWidth));
		//每圈有几个位置
		const int32 WidthBlockNum = FMath::Floor((PI * 2) / (Radian));
		float RandNum = FMath::RandRange(0.0f, 3.0f);
		//Radian += RandNum;
		for (int32 y = 0; y < WidthBlockNum; y++)
		{
			for (int32 z = 0; z < HeightBlockNum; z++)
			{
				FVector Pos;
				Pos.X = NewWidth * FMath::Sin(y * Radian + RandNum);
				Pos.Y = NewWidth * FMath::Cos(y * Radian + RandNum);
				Pos.Z = z * MinDistanceToDot;

				PosArry.Add(Pos);

				const int32 Index = FMath::RandRange(0, PosArry.Num() - 1);

				PosArry.Swap(Index, PosArry.Num() - 1);
			}
		}
	}

	MaxLimitNum = PosArry.Num();
	UE_LOG(LogTemp, Warning, TEXT("CreatCoordinateDot MaxSiz=%d"), MaxLimitNum);

	const int32 LoopNum = FMath::Min(MaxLimitNum, CreatNum);

	for (int32 i = 0; i < LoopNum; i++)
	{
		FTransform TempTransform;
		FVector Location = PosArry[i];

		TempTransform.SetLocation(Location);
		Result.Add(TempTransform);
	}
	return Result;
}

TArray<FTransform> UDLGameplayTargetSelectorDynamicGeneration::CreateRandomCoordinateBySphere(int32 CreatNum)
{
	TArray<FTransform> Result;

	const float Radian = FMath::Asin(MinDistanceToDot / MinWidth);
	const int32 WidthBlockNum = FMath::Floor((PI * 2) / (Radian * 2)) / 2;

	TArray<FVector2D> AngleNum;

	for (int32 u = 0; u < WidthBlockNum; u++)
	{
		int32 NewWidth = MinWidth * FMath::Sin((u * 2 * Radian));
		const float NewRadian = FMath::Asin(MinDistanceToDot / NewWidth);
		const int32 Num = FMath::Floor((PI * 2) / (NewRadian * 2));

		UE_LOG(LogTemp, Warning, TEXT("CreatRandomCoordinateBySphere NewWidth=%d Num=%d"), NewWidth, Num);
		for (int32 v = 0; v < Num; v++)
		{
			AngleNum.Add(FVector2D(u, v * (NewRadian / Radian)));

			const int32 Index = FMath::RandRange(0, AngleNum.Num() - 1);

			AngleNum.Swap(Index, AngleNum.Num() - 1);
		}
	}

	MaxLimitNum = AngleNum.Num();
	const int32 LoopNum = FMath::Min(MaxLimitNum, CreatNum);
	for (int32 i = 0; i < LoopNum; i++)
	{
		FTransform TempTransform;
		FVector Location;

		const float u = AngleNum[i].X * Radian * 2;
		const float v = AngleNum[i].Y * Radian * 2;

		const float RandomWidth = FMath::RandRange(MinWidth, MaxWidth);
		Location.X = RandomWidth * FMath::Sin(u) * FMath::Cos(v);
		Location.Y = RandomWidth * FMath::Sin(u) * FMath::Sin(v);
		Location.Z = RandomWidth * FMath::Cos(u);

		TempTransform.SetLocation(Location);
		Result.Add(TempTransform);
	}


	return Result;
}

UDLGameplayTargetSelectorShapeTraceBase::UDLGameplayTargetSelectorShapeTraceBase()
	: UDLGameplayTargetSelectorBase()
{
	TargetObjectType.Add(static_cast<EObjectTypeQuery>(ECC_Pawn));
}


TAutoConsoleVariable<int32> CVarEnabledDebugDraw(
	TEXT("DL.DebugDraw.TargetSelector"),
	1,
	TEXT("设置是否需要绘制碰撞框\n")
	TEXT("0 关闭  1 开启")
);

void UDLGameplayTargetSelectorSphereTrace::K2_CalculateTargetUnits_Implementation(
	const UObject* WorldContext,
	const FTransform& InAnchorTransform,
	const FString& CalculateMsg,
	bool& Succeed, TArray<FHitResult>& RetUnits)
{
	Succeed = false;

	RetUnits.Empty();

	const FVector StartPos = InAnchorTransform.TransformPositionNoScale(StartPosRelativeAnchor);

	const FVector EndPos = InAnchorTransform.TransformPositionNoScale(EndPosRelativeAnchor);


	static TArray<FHitResult> TempHitResults;
	TempHitResults.Empty();

	const bool IsDrawDebug = static_cast<bool>(CVarEnabledDebugDraw.GetValueOnGameThread());


	if (UKismetSystemLibrary::SphereTraceMultiForObjects(
		WorldContext,
		StartPos,
		EndPos,
		Radius,
		TargetObjectType,
		false,
		TArray<AActor*>{},
		IsDrawDebug ? DebugTraceType.GetValue() : EDrawDebugTrace::None,
		TempHitResults,
		bExcludeSelf
	))
	{
		if (UnitTargetSelectFilter)
		{
			FUnitSelectFilterContext Context;
			Context.OriginalActor = TempOriginActor.Get();
			UnitTargetSelectFilter->FilterUnits(Context, TempHitResults, RetUnits);
		}
		else
		{
			RetUnits = TempHitResults;
		}
	}

	Succeed = RetUnits.Num() != 0;
}

void UDLGameplayTargetSelectorBoxTrace::K2_CalculateTargetUnits_Implementation(
	const UObject* WorldContext, const FTransform& InAnchorTransform, const FString& CalculateMsg, bool& Succeed,
	TArray<FHitResult>& RetUnits)
{
	Succeed = false;

	RetUnits.Empty();

	const FVector StartPos = InAnchorTransform.TransformPositionNoScale(StartPosRelativeAnchor);

	const FVector EndPos = InAnchorTransform.TransformPositionNoScale(EndPosRelativeAnchor);

	static TArray<FHitResult> TempHitResults;
	TempHitResults.Empty();

	const FRotator Rotator = InAnchorTransform.Rotator();

	const bool IsDrawDebug = static_cast<bool>(CVarEnabledDebugDraw.GetValueOnGameThread());

	if (UKismetSystemLibrary::BoxTraceMultiForObjects(
		WorldContext,
		StartPos,
		EndPos,
		HalfSize,
		Rotator,
		TargetObjectType,
		false,
		TArray<AActor*>{},
		IsDrawDebug ? DebugTraceType.GetValue() : EDrawDebugTrace::None,
		TempHitResults,
		bExcludeSelf
	))
	{
		if (UnitTargetSelectFilter)
		{
			FUnitSelectFilterContext Context;
			Context.OriginalActor = TempOriginActor.Get();
			UnitTargetSelectFilter->FilterUnits(Context, TempHitResults, RetUnits);
		}
		else
		{
			RetUnits = TempHitResults;
		}
	}

	Succeed = true;
}

void UDLGameplayTargetSelectorCapsuleTrace::K2_CalculateTargetUnits_Implementation(
	const UObject* WorldContext, const FTransform& InAnchorTransform, const FString& CalculateMsg, bool& Succeed,
	TArray<FHitResult>& RetUnits)
{
	Succeed = false;

	RetUnits.Empty();

	const FVector StartPos = InAnchorTransform.TransformPositionNoScale(StartPosRelativeAnchor);

	const FVector EndPos = InAnchorTransform.TransformPositionNoScale(EndPosRelativeAnchor);

	static TArray<FHitResult> TempHitResults;
	TempHitResults.Empty();

	const bool IsDrawDebug = static_cast<bool>(CVarEnabledDebugDraw.GetValueOnGameThread());

	if (UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		WorldContext,
		StartPos,
		EndPos,
		CapsuleRadius,
		CapsuleHalfHeight,
		TargetObjectType,
		false,
		TArray<AActor*>{},
		IsDrawDebug ? DebugTraceType.GetValue() : EDrawDebugTrace::None,
		TempHitResults,
		bExcludeSelf
	))
	{
		if (UnitTargetSelectFilter)
		{
			FUnitSelectFilterContext Context;
			Context.OriginalActor = TempOriginActor.Get();
			UnitTargetSelectFilter->FilterUnits(Context, TempHitResults, RetUnits);
		}
		else
		{
			RetUnits = TempHitResults;
		}
	}

	Succeed = true;
}
