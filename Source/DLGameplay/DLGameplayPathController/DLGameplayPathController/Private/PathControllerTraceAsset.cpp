// Fill out your copyright notice in the Description page of Project Settings.


#include "PathControllerTraceAsset.h"

#include "Kismet/KismetMathLibrary.h"

const FInterpCurvePointVector UPathControllerTraceAsset::DummyPointPosition(0.0f, FVector::ZeroVector, FVector::ForwardVector, FVector::ForwardVector, CIM_Constant);
const FInterpCurvePointQuat UPathControllerTraceAsset::DummyPointRotation(0.0f, FQuat::Identity);
const FInterpCurvePointVector UPathControllerTraceAsset::DummyPointScale(0.0f, FVector::OneVector);

UPathControllerTraceAsset::UPathControllerTraceAsset(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, ReparamStepsPerSegment(10)
	, bStationaryEndpoints(false)
	, DefaultUpVector(FVector::UpVector)
	, bClosedLoop(false)

{
	SplineCurves.Position.Points.Reset(10);
	SplineCurves.Rotation.Points.Reset(10);
	SplineCurves.Scale.Points.Reset(10);

	SplineCurves.Position.Points.Emplace(0.0f, FVector(0, 0, 0), FVector::ZeroVector, FVector::ZeroVector, CIM_CurveAuto);
	SplineCurves.Rotation.Points.Emplace(0.0f, FQuat::Identity, FQuat::Identity, FQuat::Identity, CIM_CurveAuto);
	SplineCurves.Scale.Points.Emplace(0.0f, FVector(1.0f), FVector::ZeroVector, FVector::ZeroVector, CIM_CurveAuto);

	SplineCurves.Position.Points.Emplace(1.0f, FVector(100, 0, 0), FVector::ZeroVector, FVector::ZeroVector, CIM_CurveAuto);
	SplineCurves.Rotation.Points.Emplace(1.0f, FQuat::Identity, FQuat::Identity, FQuat::Identity, CIM_CurveAuto);
	SplineCurves.Scale.Points.Emplace(1.0f, FVector(1.0f), FVector::ZeroVector, FVector::ZeroVector, CIM_CurveAuto);
	UpdateSpline();
}

UPathControllerTraceAsset* UPathControllerTraceAsset::CopyAsset(const UPathControllerTraceAsset* OtherAsset)
{
	return DuplicateObject<UPathControllerTraceAsset>(OtherAsset, nullptr);
}

void UPathControllerTraceAsset::UpdateSpline()
{
	SplineCurves.UpdateSpline(bClosedLoop, bStationaryEndpoints, ReparamStepsPerSegment, bLoopPositionOverride, LoopPosition, FVector{ 1.0f });
}

float UPathControllerTraceAsset::GetSplineDisplacement() const
{
	const float Num = GetNumberOfSplinePoints();
	if (Num >= 2)
	{
		return FVector::Distance(GetLocationAtSplinePoint(0), GetLocationAtSplinePoint(Num - 1));
	}
	return 0.0f;
}

float UPathControllerTraceAsset::GetSplineDisplacementSquared() const
{
	const float Num = GetNumberOfSplinePoints();
	if (Num > 1)
	{
		return FVector::DistSquared(GetLocationAtSplinePoint(0), GetLocationAtSplinePoint(Num - 1));
	}
	return 0.0f;
}

FVector UPathControllerTraceAsset::GetLocationAtSplineInputKey(float InKey) const
{
	const FVector Location = SplineCurves.Position.Eval(InKey, FVector::ZeroVector);
	return Location;
}

FVector UPathControllerTraceAsset::GetTangentAtSplineInputKey(float InKey) const
{
	const FVector Tangent = SplineCurves.Position.EvalDerivative(InKey, FVector::ZeroVector);
	return Tangent;
}

FVector UPathControllerTraceAsset::GetDirectionAtSplineInputKey(float InKey) const
{
	const FVector Direction = SplineCurves.Position.EvalDerivative(InKey, FVector::ZeroVector).GetSafeNormal();
	return Direction;
}

FRotator UPathControllerTraceAsset::GetRotationAtSplineInputKey(float InKey) const
{
	return GetQuaternionAtSplineInputKey(InKey).Rotator();
}

FQuat UPathControllerTraceAsset::GetQuaternionAtSplineInputKey(float InKey) const
{
	FQuat Quat = SplineCurves.Rotation.Eval(InKey, FQuat::Identity);
	Quat.Normalize();

	const FVector Direction = SplineCurves.Position.EvalDerivative(InKey, FVector::ZeroVector).GetSafeNormal();
	const FVector UpVector = Quat.RotateVector(DefaultUpVector);

	const FQuat Rot = (FRotationMatrix::MakeFromXZ(Direction, UpVector)).ToQuat();

	return Rot;
}

FVector UPathControllerTraceAsset::GetUpVectorAtSplineInputKey(float InKey) const
{
	const FQuat Quat = GetQuaternionAtSplineInputKey(InKey);
	const FVector UpVector = Quat.RotateVector(FVector::UpVector);

	return UpVector;
}

FVector UPathControllerTraceAsset::GetRightVectorAtSplineInputKey(float InKey) const
{
	const FQuat Quat = GetQuaternionAtSplineInputKey(InKey);
	const FVector RightVector = Quat.RotateVector(FVector::RightVector);

	return RightVector;
}

FTransform UPathControllerTraceAsset::GetTransformAtSplineInputKey(float InKey, bool bUseScale) const
{
	const FVector Location(GetLocationAtSplineInputKey(InKey));
	const FQuat Rotation(GetQuaternionAtSplineInputKey(InKey));
	const FVector Scale = bUseScale ? GetScaleAtSplineInputKey(InKey) : FVector(1.0f);

	return FTransform(Rotation, Location, Scale);
}

float UPathControllerTraceAsset::GetRollAtSplineInputKey(float InKey) const
{
	return GetRotationAtSplineInputKey(InKey).Roll;
}

FVector UPathControllerTraceAsset::GetScaleAtSplineInputKey(float InKey) const
{
	const FVector Scale = SplineCurves.Scale.Eval(InKey, FVector(1.0f));
	return Scale;
}

float UPathControllerTraceAsset::GetDistanceAlongSplineAtSplineInputKey(float InKey) const
{
	const int32 NumPoints = SplineCurves.Position.Points.Num();
	const int32 NumSegments = bClosedLoop ? NumPoints : NumPoints - 1;

	if ((InKey >= 0) && (InKey < NumSegments))
	{
		const int32 PointIndex = FMath::FloorToInt(InKey);
		const float Fraction = InKey - PointIndex;
		const int32 ReparamPointIndex = PointIndex * ReparamStepsPerSegment;
		const float Distance = SplineCurves.ReparamTable.Points[ReparamPointIndex].InVal;
		return Distance + GetSegmentLength(PointIndex, Fraction);
	}
	else if (InKey >= NumSegments)
	{
		return SplineCurves.GetSplineLength();
	}

	return 0.0f;

}

void UPathControllerTraceAsset::SetClosedLoopAtPosition(bool bInClosedLoop, float Key, bool bUpdateSpline)
{
	bClosedLoop = bInClosedLoop;
	bLoopPositionOverride = bInClosedLoop;
	LoopPosition = Key;

	if (bUpdateSpline)
	{
		UpdateSpline();
	}
}

void UPathControllerTraceAsset::ClearSplinePoints(bool bUpdateSpline)
{
	SplineCurves.Position.Points.Reset();
	SplineCurves.Rotation.Points.Reset();
	SplineCurves.Scale.Points.Reset();

	if (bUpdateSpline)
	{
		UpdateSpline();
	}
}

static int32 UpperBound(const TArray<FInterpCurvePoint<FVector>>& SplinePoints, float Value)
{
	int32 Count = SplinePoints.Num();
	int32 First = 0;

	while (Count > 0)
	{
		const int32 Middle = Count / 2;
		if (Value >= SplinePoints[First + Middle].InVal)
		{
			First += Middle + 1;
			Count -= Middle + 1;
		}
		else
		{
			Count = Middle;
		}
	}

	return First;
}

void UPathControllerTraceAsset::AddPoint(const FSplinePoint& InSplinePoint, bool bUpdateSpline)
{
	const int32 Index = UpperBound(SplineCurves.Position.Points, InSplinePoint.InputKey);

	SplineCurves.Position.Points.Insert(FInterpCurvePoint<FVector>(
		InSplinePoint.InputKey,
		InSplinePoint.Position,
		InSplinePoint.ArriveTangent,
		InSplinePoint.LeaveTangent,
		ConvertSplinePointTypeToInterpCurveMode(InSplinePoint.Type)
		),
		Index);

	SplineCurves.Rotation.Points.Insert(FInterpCurvePoint<FQuat>(
		InSplinePoint.InputKey,
		InSplinePoint.Rotation.Quaternion(),
		FQuat::Identity,
		FQuat::Identity,
		CIM_CurveAuto
		),
		Index);

	SplineCurves.Scale.Points.Insert(FInterpCurvePoint<FVector>(
		InSplinePoint.InputKey,
		InSplinePoint.Scale,
		FVector::ZeroVector,
		FVector::ZeroVector,
		CIM_CurveAuto
		),
		Index);

	if (bLoopPositionOverride && LoopPosition <= SplineCurves.Position.Points.Last().InVal)
	{
		bLoopPositionOverride = false;
	}

	if (bUpdateSpline)
	{
		UpdateSpline();
	}
}

void UPathControllerTraceAsset::AddPoints(const TArray<FSplinePoint>& InSplinePoints, bool bUpdateSpline)
{
	const int32 NumPoints = SplineCurves.Position.Points.Num();
	SplineCurves.Position.Points.Reserve(NumPoints + InSplinePoints.Num());

	for (const auto& SplinePoint : InSplinePoints)
	{
		AddPoint(SplinePoint, false);
	}

	if (bUpdateSpline)
	{
		UpdateSpline();
	}
}

void UPathControllerTraceAsset::AddSplinePoint(const FVector& Position, bool bUpdateSpline)
{
	// Add the spline point at the end of the array, adding 1.0 to the current last input key.
	// This continues the former behavior in which spline points had to be separated by an interval of 1.0.
	const float InKey = SplineCurves.Position.Points.Num() ? SplineCurves.Position.Points.Last().InVal + 1.0f : 0.0f;

	SplineCurves.Position.Points.Emplace(InKey, Position, FVector::ZeroVector, FVector::ZeroVector, CIM_CurveAuto);
	SplineCurves.Rotation.Points.Emplace(InKey, FQuat::Identity, FQuat::Identity, FQuat::Identity, CIM_CurveAuto);
	SplineCurves.Scale.Points.Emplace(InKey, FVector(1.0f), FVector::ZeroVector, FVector::ZeroVector, CIM_CurveAuto);


	if (bLoopPositionOverride)
	{
		LoopPosition += 1.0f;
	}

	if (bUpdateSpline)
	{
		UpdateSpline();
	}

}

void UPathControllerTraceAsset::AddSplinePointAtIndex(const FVector& Position, int32 Index, bool bUpdateSpline)
{
	int32 NumPoints = SplineCurves.Position.Points.Num();

	if (Index >= 0 && Index <= NumPoints)
	{
		const float InKey = (Index == 0) ? 0.0f : SplineCurves.Position.Points[Index - 1].InVal + 1.0f;

		SplineCurves.Position.Points.Insert(FInterpCurvePoint<FVector>(InKey, Position, FVector::ZeroVector, FVector::ZeroVector, CIM_CurveAuto), Index);
		SplineCurves.Rotation.Points.Insert(FInterpCurvePoint<FQuat>(InKey, FQuat::Identity, FQuat::Identity, FQuat::Identity, CIM_CurveAuto), Index);
		SplineCurves.Scale.Points.Insert(FInterpCurvePoint<FVector>(InKey, FVector(1.0f), FVector::ZeroVector, FVector::ZeroVector, CIM_CurveAuto), Index);

		NumPoints++;

		// Adjust subsequent points' input keys to make room for the value just added
		for (int I = Index + 1; I < NumPoints; ++I)
		{
			SplineCurves.Position.Points[I].InVal += 1.0f;
			SplineCurves.Rotation.Points[I].InVal += 1.0f;
			SplineCurves.Scale.Points[I].InVal += 1.0f;
		}

		if (bLoopPositionOverride)
		{
			LoopPosition += 1.0f;
		}
	}

	if (bUpdateSpline)
	{
		UpdateSpline();
	}

}

void UPathControllerTraceAsset::RemoveSplinePoint(int32 Index, bool bUpdateSpline)
{
	int32 NumPoints = SplineCurves.Position.Points.Num();

	if (Index >= 0 && Index < NumPoints)
	{
		SplineCurves.Position.Points.RemoveAt(Index, 1, false);
		SplineCurves.Rotation.Points.RemoveAt(Index, 1, false);
		SplineCurves.Scale.Points.RemoveAt(Index, 1, false);

		NumPoints--;

		// Adjust all following spline point input keys to close the gap left by the removed point
		while (Index < NumPoints)
		{
			SplineCurves.Position.Points[Index].InVal -= 1.0f;
			SplineCurves.Rotation.Points[Index].InVal -= 1.0f;
			SplineCurves.Scale.Points[Index].InVal -= 1.0f;
			Index++;
		}

		if (bLoopPositionOverride)
		{
			LoopPosition -= 1.0f;
		}
	}

	if (bUpdateSpline)
	{
		UpdateSpline();
	}

}

void UPathControllerTraceAsset::SetSplinePoints(const TArray<FVector>& Points, bool bUpdateSpline)
{
	const int32 NumPoints = Points.Num();
	SplineCurves.Position.Points.Reset(NumPoints);
	SplineCurves.Rotation.Points.Reset(NumPoints);
	SplineCurves.Scale.Points.Reset(NumPoints);

	float InputKey = 0.0f;
	for (const auto& Point : Points)
	{

		SplineCurves.Position.Points.Emplace(InputKey, Point, FVector::ZeroVector, FVector::ZeroVector, CIM_CurveAuto);
		SplineCurves.Rotation.Points.Emplace(InputKey, FQuat::Identity, FQuat::Identity, FQuat::Identity, CIM_CurveAuto);
		SplineCurves.Scale.Points.Emplace(InputKey, FVector(1.0f), FVector::ZeroVector, FVector::ZeroVector, CIM_CurveAuto);


		InputKey += 1.0f;
	}

	bLoopPositionOverride = false;

	if (bUpdateSpline)
	{
		UpdateSpline();
	}

}

void UPathControllerTraceAsset::SetLocationAtSplinePoint(int32 PointIndex, const FVector& InLocation, bool bUpdateSpline)
{
	const int32 NumPoints = SplineCurves.Position.Points.Num();

	if ((PointIndex >= 0) && (PointIndex < NumPoints))
	{
		SplineCurves.Position.Points[PointIndex].OutVal = InLocation;

		if (bUpdateSpline)
		{
			UpdateSpline();
		}
	}
}

void UPathControllerTraceAsset::SetTangentAtSplinePoint(int32 PointIndex, const FVector& InTangent, bool bUpdateSpline)
{
	const int32 NumPoints = SplineCurves.Position.Points.Num();

	if ((PointIndex >= 0) && (PointIndex < NumPoints))
	{
		SplineCurves.Position.Points[PointIndex].LeaveTangent = InTangent;
		SplineCurves.Position.Points[PointIndex].ArriveTangent = InTangent;
		SplineCurves.Position.Points[PointIndex].InterpMode = CIM_CurveUser;

		if (bUpdateSpline)
		{
			UpdateSpline();
		}
	}

}

void UPathControllerTraceAsset::SetTangentsAtSplinePoint(int32 PointIndex, const FVector& InArriveTangent,
	const FVector& InLeaveTangent, bool bUpdateSpline)
{
	const int32 NumPoints = SplineCurves.Position.Points.Num();

	if ((PointIndex >= 0) && (PointIndex < NumPoints))
	{
		SplineCurves.Position.Points[PointIndex].ArriveTangent = InArriveTangent;
		SplineCurves.Position.Points[PointIndex].LeaveTangent = InLeaveTangent;
		SplineCurves.Position.Points[PointIndex].InterpMode = CIM_CurveUser;

		if (bUpdateSpline)
		{
			UpdateSpline();
		}
	}

}

void UPathControllerTraceAsset::SetUpVectorAtSplinePoint(int32 PointIndex, const FVector& InUpVector, bool bUpdateSpline)
{
	const int32 NumPoints = SplineCurves.Position.Points.Num();

	if ((PointIndex >= 0) && (PointIndex < NumPoints))
	{
		const FVector TransformedUpVector = InUpVector.GetSafeNormal();

		FQuat Quat = FQuat::FindBetween(DefaultUpVector, TransformedUpVector);
		SplineCurves.Rotation.Points[PointIndex].OutVal = Quat;

		if (bUpdateSpline)
		{
			UpdateSpline();
		}
	}
}

void UPathControllerTraceAsset::SetRotationAtSplinePoint(int32 PointIndex, const FRotator& InRotation, bool bUpdateSpline)
{
	if (SplineCurves.Rotation.Points.IsValidIndex(PointIndex))
	{
		const FQuat Quat = InRotation.Quaternion();

		FVector UpVector = Quat.GetUpVector();
		SetUpVectorAtSplinePoint(PointIndex, UpVector, false);

		FVector Direction = Quat.GetForwardVector();
		SetTangentAtSplinePoint(PointIndex, Direction, false);

		if (bUpdateSpline)
		{
			UpdateSpline();
		}
	}
}

void UPathControllerTraceAsset::SetScaleAtSplinePoint(int32 PointIndex, const FVector& InScaleVector, bool bUpdateSpline)
{
	if (SplineCurves.Rotation.Points.IsValidIndex(PointIndex))
	{
		SplineCurves.Scale.Points[PointIndex].OutVal = InScaleVector;

		if (bUpdateSpline)
		{
			UpdateSpline();
		}
	}
}

ESplinePointType::Type UPathControllerTraceAsset::GetSplinePointType(int32 PointIndex) const
{
	if ((PointIndex >= 0) && (PointIndex < SplineCurves.Position.Points.Num()))
	{
		return ConvertInterpCurveModeToSplinePointType(SplineCurves.Position.Points[PointIndex].InterpMode);
	}

	return ESplinePointType::Constant;
}

void UPathControllerTraceAsset::SetSplinePointType(int32 PointIndex, ESplinePointType::Type Type, bool bUpdateSpline)
{
	if ((PointIndex >= 0) && (PointIndex < SplineCurves.Position.Points.Num()))
	{
		SplineCurves.Position.Points[PointIndex].InterpMode = ConvertSplinePointTypeToInterpCurveMode(Type);
		if (bUpdateSpline)
		{
			UpdateSpline();
		}
	}
}

int32 UPathControllerTraceAsset::GetNumberOfSplinePoints() const
{
	// No longer returns an imaginary extra endpoint if closed loop is set
	const int32 NumPoints = SplineCurves.Position.Points.Num();
	return NumPoints;
}

int32 UPathControllerTraceAsset::GetNumberOfSplineSegments() const
{
	const int32 NumPoints = SplineCurves.Position.Points.Num();
	return (bClosedLoop ? NumPoints : NumPoints - 1);
}

FVector UPathControllerTraceAsset::GetLocationAtSplinePoint(int32 PointIndex) const
{
	const FInterpCurvePointVector& Point = GetPositionPointSafe(PointIndex);
	const FVector& Location = Point.OutVal;
	return Location;
}

FVector UPathControllerTraceAsset::GetDirectionAtSplinePoint(int32 PointIndex) const
{
	const FInterpCurvePointVector& Point = GetPositionPointSafe(PointIndex);
	const FVector Direction = Point.LeaveTangent.GetSafeNormal();
	return Direction;

}

FVector UPathControllerTraceAsset::GetTangentAtSplinePoint(int32 PointIndex) const
{
	const FInterpCurvePointVector& Point = GetPositionPointSafe(PointIndex);
	const FVector& Direction = Point.LeaveTangent;
	return Direction;

}

FVector UPathControllerTraceAsset::GetArriveTangentAtSplinePoint(int32 PointIndex) const
{
	const FInterpCurvePointVector& Point = GetPositionPointSafe(PointIndex);
	const FVector& Direction = Point.ArriveTangent;
	return Direction;

}

FVector UPathControllerTraceAsset::GetLeaveTangentAtSplinePoint(int32 PointIndex) const
{
	const FInterpCurvePointVector& Point = GetPositionPointSafe(PointIndex);
	const FVector& Direction = Point.LeaveTangent;
	return Direction;
}

FQuat UPathControllerTraceAsset::GetQuaternionAtSplinePoint(int32 PointIndex) const
{
	const FInterpCurvePointQuat& Point = GetRotationPointSafe(PointIndex);
	return GetQuaternionAtSplineInputKey(Point.InVal);

}

FRotator UPathControllerTraceAsset::GetRotationAtSplinePoint(int32 PointIndex) const
{
	const FInterpCurvePointQuat& Point = GetRotationPointSafe(PointIndex);
	return GetRotationAtSplineInputKey(Point.InVal);
}

FVector UPathControllerTraceAsset::GetUpVectorAtSplinePoint(int32 PointIndex) const
{
	const FInterpCurvePointQuat& Point = GetRotationPointSafe(PointIndex);
	return GetUpVectorAtSplineInputKey(Point.InVal);

}

FVector UPathControllerTraceAsset::GetRightVectorAtSplinePoint(int32 PointIndex) const
{
	const FInterpCurvePointQuat& Point = GetRotationPointSafe(PointIndex);
	return GetRightVectorAtSplineInputKey(Point.InVal);
}

float UPathControllerTraceAsset::GetRollAtSplinePoint(int32 PointIndex) const
{
	const FInterpCurvePointQuat& Point = GetRotationPointSafe(PointIndex);
	return GetRollAtSplineInputKey(Point.InVal);
}

FVector UPathControllerTraceAsset::GetScaleAtSplinePoint(int32 PointIndex) const
{
	const FInterpCurvePointVector& Point = GetScalePointSafe(PointIndex);
	return Point.OutVal;
}

FTransform UPathControllerTraceAsset::GetTransformAtSplinePoint(int32 PointIndex, bool bUseScale) const
{
	const FInterpCurvePointQuat& Point = GetRotationPointSafe(PointIndex);
	return GetTransformAtSplineInputKey(Point.InVal, bUseScale);
}

void UPathControllerTraceAsset::GetLocationAndTangentAtSplinePoint(int32 PointIndex, FVector& Location,
	FVector& Tangent) const
{
	const FInterpCurvePointQuat& Point = GetRotationPointSafe(PointIndex);
	const float InputKey = Point.InVal;
	Location = GetLocationAtSplineInputKey(InputKey);
	Tangent = GetTangentAtSplineInputKey(InputKey);

}

float UPathControllerTraceAsset::GetDistanceAlongSplineAtSplinePoint(int32 PointIndex) const
{
	const int32 NumPoints = SplineCurves.Position.Points.Num();
	const int32 NumSegments = bClosedLoop ? NumPoints : NumPoints - 1;

	if ((PointIndex >= 0) && (PointIndex < NumSegments + 1))
	{
		return SplineCurves.ReparamTable.Points[PointIndex * ReparamStepsPerSegment].InVal;
	}

	return 0.0f;

}

float UPathControllerTraceAsset::GetSplineLength() const
{
	return SplineCurves.GetSplineLength();
}

void UPathControllerTraceAsset::SetDefaultUpVector(const FVector& UpVector)
{
	DefaultUpVector = UpVector;
	UpdateSpline();
}

FVector UPathControllerTraceAsset::GetDefaultUpVector() const
{
	return DefaultUpVector;
}

float UPathControllerTraceAsset::GetInputKeyAtDistanceAlongSpline(float Distance) const
{
	const int32 NumPoints = SplineCurves.Position.Points.Num();

	if (NumPoints < 2)
	{
		return 0.0f;
	}

	const float TimeMultiplier = Duration / (bClosedLoop ? NumPoints : (NumPoints - 1.0f));
	return SplineCurves.ReparamTable.Eval(Distance, 0.0f) * TimeMultiplier;
}

FVector UPathControllerTraceAsset::GetLocationAtDistanceAlongSpline(float Distance) const
{
	const float Param = SplineCurves.ReparamTable.Eval(Distance, 0.0f);
	return GetLocationAtSplineInputKey(Param);

}

FVector UPathControllerTraceAsset::GetDirectionAtDistanceAlongSpline(float Distance) const
{
	const float Param = SplineCurves.ReparamTable.Eval(Distance, 0.0f);
	return GetDirectionAtSplineInputKey(Param);
}

FVector UPathControllerTraceAsset::GetTangentAtDistanceAlongSpline(float Distance) const
{
	const float Param = SplineCurves.ReparamTable.Eval(Distance, 0.0f);
	return GetTangentAtSplineInputKey(Param);

}

FQuat UPathControllerTraceAsset::GetQuaternionAtDistanceAlongSpline(float Distance) const
{
	const float Param = SplineCurves.ReparamTable.Eval(Distance, 0.0f);
	return GetQuaternionAtSplineInputKey(Param);

}

FRotator UPathControllerTraceAsset::GetRotationAtDistanceAlongSpline(float Distance) const
{
	const float Param = SplineCurves.ReparamTable.Eval(Distance, 0.0f);
	return GetRotationAtSplineInputKey(Param);
}

FVector UPathControllerTraceAsset::GetUpVectorAtDistanceAlongSpline(float Distance) const
{
	const float Param = SplineCurves.ReparamTable.Eval(Distance, 0.0f);
	return GetUpVectorAtSplineInputKey(Param);
}

FVector UPathControllerTraceAsset::GetRightVectorAtDistanceAlongSpline(float Distance) const
{
	const float Param = SplineCurves.ReparamTable.Eval(Distance, 0.0f);
	return GetRightVectorAtSplineInputKey(Param);
}

float UPathControllerTraceAsset::GetRollAtDistanceAlongSpline(float Distance) const
{
	const float Param = SplineCurves.ReparamTable.Eval(Distance, 0.0f);
	return GetRollAtSplineInputKey(Param);

}

FVector UPathControllerTraceAsset::GetScaleAtDistanceAlongSpline(float Distance) const
{
	const float Param = SplineCurves.ReparamTable.Eval(Distance, 0.0f);
	return GetScaleAtSplineInputKey(Param);
}

FTransform UPathControllerTraceAsset::GetTransformAtDistanceAlongSpline(float Distance, bool bUseScale) const
{

	const float Param = SplineCurves.ReparamTable.Eval(Distance, 0.0f);
	return GetTransformAtSplineInputKey(Param, bUseScale);
}

FVector UPathControllerTraceAsset::GetLocationAtTime(float Time, bool bUseConstantVelocity) const
{
	if (Duration == 0.0f)
	{
		return FVector::ZeroVector;
	}

	if (bUseConstantVelocity)
	{
		return GetLocationAtDistanceAlongSpline(Time / Duration * GetSplineLength());
	}
	else
	{
		const int32 NumPoints = SplineCurves.Position.Points.Num();
		const int32 NumSegments = bClosedLoop ? NumPoints : NumPoints - 1;
		const float TimeMultiplier = NumSegments / Duration;
		return GetLocationAtSplineInputKey(Time * TimeMultiplier);
	}
}

FVector UPathControllerTraceAsset::GetDirectionAtTime(float Time, bool bUseConstantVelocity) const
{
	if (Duration == 0.0f)
	{
		return FVector::ZeroVector;
	}

	if (bUseConstantVelocity)
	{
		return GetDirectionAtDistanceAlongSpline(Time / Duration * GetSplineLength());
	}
	else
	{
		const int32 NumPoints = SplineCurves.Position.Points.Num();
		const int32 NumSegments = bClosedLoop ? NumPoints : NumPoints - 1;
		const float TimeMultiplier = NumSegments / Duration;
		return GetDirectionAtSplineInputKey(Time * TimeMultiplier);
	}

}

FVector UPathControllerTraceAsset::GetTangentAtTime(float Time, bool bUseConstantVelocity) const
{
	if (Duration == 0.0f)
	{
		return FVector::ZeroVector;
	}

	if (bUseConstantVelocity)
	{
		return GetTangentAtDistanceAlongSpline(Time / Duration * GetSplineLength());
	}
	else
	{
		const int32 NumPoints = SplineCurves.Position.Points.Num();
		const int32 NumSegments = bClosedLoop ? NumPoints : NumPoints - 1;
		const float TimeMultiplier = NumSegments / Duration;
		return GetTangentAtSplineInputKey(Time * TimeMultiplier);
	}

}

FQuat UPathControllerTraceAsset::GetQuaternionAtTime(float Time, bool bUseConstantVelocity) const
{
	if (Duration == 0.0f)
	{
		return FQuat::Identity;
	}

	if (bUseConstantVelocity)
	{
		return GetQuaternionAtDistanceAlongSpline(Time / Duration * GetSplineLength());
	}
	else
	{
		const int32 NumPoints = SplineCurves.Position.Points.Num();
		const int32 NumSegments = bClosedLoop ? NumPoints : NumPoints - 1;
		const float TimeMultiplier = NumSegments / Duration;
		return GetQuaternionAtSplineInputKey(Time * TimeMultiplier);
	}

}

FRotator UPathControllerTraceAsset::GetRotationAtTime(float Time, bool bUseConstantVelocity) const
{
	if (Duration == 0.0f)
	{
		return FRotator::ZeroRotator;
	}

	if (bUseConstantVelocity)
	{
		return GetRotationAtDistanceAlongSpline(Time / Duration * GetSplineLength());
	}
	else
	{
		const int32 NumPoints = SplineCurves.Position.Points.Num();
		const int32 NumSegments = bClosedLoop ? NumPoints : NumPoints - 1;
		const float TimeMultiplier = NumSegments / Duration;
		return GetRotationAtSplineInputKey(Time * TimeMultiplier);
	}

}

FVector UPathControllerTraceAsset::GetUpVectorAtTime(float Time, bool bUseConstantVelocity) const
{
	if (Duration == 0.0f)
	{
		return FVector::ZeroVector;
	}

	if (bUseConstantVelocity)
	{
		return GetUpVectorAtDistanceAlongSpline(Time / Duration * GetSplineLength());
	}
	else
	{
		const int32 NumPoints = SplineCurves.Position.Points.Num();
		const int32 NumSegments = bClosedLoop ? NumPoints : NumPoints - 1;
		const float TimeMultiplier = NumSegments / Duration;
		return GetUpVectorAtSplineInputKey(Time * TimeMultiplier);
	}

}

FVector UPathControllerTraceAsset::GetRightVectorAtTime(float Time, bool bUseConstantVelocity) const
{
	if (Duration == 0.0f)
	{
		return FVector::ZeroVector;
	}

	if (bUseConstantVelocity)
	{
		return GetRightVectorAtDistanceAlongSpline(Time / Duration * GetSplineLength());
	}
	else
	{
		const int32 NumPoints = SplineCurves.Position.Points.Num();
		const int32 NumSegments = bClosedLoop ? NumPoints : NumPoints - 1;
		const float TimeMultiplier = NumSegments / Duration;
		return GetRightVectorAtSplineInputKey(Time * TimeMultiplier);
	}

}

FTransform UPathControllerTraceAsset::GetTransformAtTime(float Time, bool bUseConstantVelocity, bool bUseScale) const
{
	if (Duration == 0.0f)
	{
		return FTransform::Identity;
	}

	if (bUseConstantVelocity)
	{
		return GetTransformAtDistanceAlongSpline(Time / Duration * GetSplineLength(), bUseScale);
	}
	else
	{
		const int32 NumPoints = SplineCurves.Position.Points.Num();
		const int32 NumSegments = bClosedLoop ? NumPoints : NumPoints - 1;
		const float TimeMultiplier = NumSegments / Duration;
		return GetTransformAtSplineInputKey(Time * TimeMultiplier, bUseScale);
	}
}

float UPathControllerTraceAsset::GetRollAtTime(float Time, bool bUseConstantVelocity) const
{
	if (Duration == 0.0f)
	{
		return 0.0f;
	}

	if (bUseConstantVelocity)
	{
		return GetRollAtDistanceAlongSpline(Time / Duration * GetSplineLength());
	}
	else
	{
		const int32 NumPoints = SplineCurves.Position.Points.Num();
		const int32 NumSegments = bClosedLoop ? NumPoints : NumPoints - 1;
		const float TimeMultiplier = NumSegments / Duration;
		return GetRollAtSplineInputKey(Time * TimeMultiplier);
	}

}

FVector UPathControllerTraceAsset::GetScaleAtTime(float Time, bool bUseConstantVelocity) const
{
	if (Duration == 0.0f)
	{
		return FVector(1.0f);
	}

	if (bUseConstantVelocity)
	{
		return GetScaleAtDistanceAlongSpline(Time / Duration * GetSplineLength());
	}
	else
	{
		const int32 NumPoints = SplineCurves.Position.Points.Num();
		const int32 NumSegments = bClosedLoop ? NumPoints : NumPoints - 1;
		const float TimeMultiplier = NumSegments / Duration;
		return GetScaleAtSplineInputKey(Time * TimeMultiplier);
	}

}

float UPathControllerTraceAsset::FindInputKeyClosestToWorldLocation(const FVector& WorldLocation,
	const FTransform& RelativeTransform) const
{
	const FVector LocalLocation = RelativeTransform.InverseTransformPosition(WorldLocation);
	float Dummy;
	return SplineCurves.Position.InaccurateFindNearest(LocalLocation, Dummy);
}

FVector UPathControllerTraceAsset::FindLocationClosestToWorldLocation(const FVector& WorldLocation,
	const FTransform& RelativeTransform) const
{
	const float Param = FindInputKeyClosestToWorldLocation(WorldLocation, RelativeTransform);
	return GetLocationAtSplineInputKey(Param);
}

FVector UPathControllerTraceAsset::FindDirectionClosestToWorldLocation(const FVector& WorldLocation,
	const FTransform& RelativeTransform) const
{
	const float Param = FindInputKeyClosestToWorldLocation(WorldLocation, RelativeTransform);
	return GetDirectionAtSplineInputKey(Param);
}

FVector UPathControllerTraceAsset::FindTangentClosestToWorldLocation(const FVector& WorldLocation,
	const FTransform& RelativeTransform) const
{
	const float Param = FindInputKeyClosestToWorldLocation(WorldLocation, RelativeTransform);
	return GetTangentAtSplineInputKey(Param);
}

FQuat UPathControllerTraceAsset::FindQuaternionClosestToWorldLocation(const FVector& WorldLocation,
	const FTransform& RelativeTransform) const
{
	const float Param = FindInputKeyClosestToWorldLocation(WorldLocation, RelativeTransform);
	return GetQuaternionAtSplineInputKey(Param);
}

FRotator UPathControllerTraceAsset::FindRotationClosestToWorldLocation(const FVector& WorldLocation,
	const FTransform& RelativeTransform) const
{
	const float Param = FindInputKeyClosestToWorldLocation(WorldLocation, RelativeTransform);
	return GetRotationAtSplineInputKey(Param);
}

FVector UPathControllerTraceAsset::FindUpVectorClosestToWorldLocation(const FVector& WorldLocation,
	const FTransform& RelativeTransform) const
{
	const float Param = FindInputKeyClosestToWorldLocation(WorldLocation, RelativeTransform);
	return GetUpVectorAtSplineInputKey(Param);
}

FVector UPathControllerTraceAsset::FindRightVectorClosestToWorldLocation(const FVector& WorldLocation,
	const FTransform& RelativeTransform) const
{
	const float Param = FindInputKeyClosestToWorldLocation(WorldLocation, RelativeTransform);
	return GetRightVectorAtSplineInputKey(Param);
}

float UPathControllerTraceAsset::FindRollClosestToWorldLocation(const FVector& WorldLocation,
	const FTransform& RelativeTransform) const
{
	const float Param = FindInputKeyClosestToWorldLocation(WorldLocation, RelativeTransform);
	return GetRollAtSplineInputKey(Param);
}

FVector UPathControllerTraceAsset::FindScaleClosestToWorldLocation(const FVector& WorldLocation,
	const FTransform& RelativeTransform) const
{
	const float Param = FindInputKeyClosestToWorldLocation(WorldLocation, RelativeTransform);
	return GetScaleAtSplineInputKey(Param);
}

FTransform UPathControllerTraceAsset::FindTransformClosestToWorldLocation(const FVector& WorldLocation,
	const FTransform& RelativeTransform, bool bUseScale) const
{
	const float Param = FindInputKeyClosestToWorldLocation(WorldLocation, RelativeTransform);
	return GetTransformAtSplineInputKey(Param, bUseScale);
}

float UPathControllerTraceAsset::FindLocationsAtDisplacement(const float CurInputKey, const float Displacement, FVector& OutLocation)
{
	const float NeedDisplacementSq = FMath::Square(Displacement);
	return InaccurateFindNearest(SplineCurves.Position, CurInputKey, NeedDisplacementSq, OutLocation);
}

template <class T>
float UPathControllerTraceAsset::InaccurateFindNearest(const FInterpCurve<T>& InterpCurve, const float CurInputKey, float NeedDisplacementSq, FVector& OutLocation)
{
	const int32 NumPoints = InterpCurve.Points.Num();
	const int32 NumSegments = InterpCurve.bIsLooped ? NumPoints : NumPoints - 1;
	OutLocation = FVector::ZeroVector;
	if (NumPoints > 1)
	{
		const int32 CurSegment = FMath::FloorToInt(CurInputKey);
		float CurDistance = 0.0f;
		for (int32 Segment = CurSegment; Segment < NumSegments; ++Segment)
		{
			const float BestResult = InaccurateFindNearestOnSegment(InterpCurve, CurInputKey, NeedDisplacementSq, CurDistance, Segment, OutLocation);
			if (BestResult > CurInputKey)
			{
				return BestResult;
			}
		}
	}

	if (NumPoints == 1)
	{
		return InterpCurve.Points[0].InVal;
	}

	return -1.0f;
}

template <class T>
float UPathControllerTraceAsset::InaccurateFindNearestOnSegment(const FInterpCurve<T>& InterpCurve, const float CurInputKey, float NeedDisplacementSq, float& OutDisplacementSq, int32 PtIdx, FVector& OutLocation) const
{
	const int32 NumPoints = InterpCurve.Points.Num();
	const int32 LastPoint = NumPoints - 1;
	const int32 NextPtIdx = (InterpCurve.bIsLooped && PtIdx == LastPoint) ? 0 : (PtIdx + 1);
	check(PtIdx >= 0 && ((InterpCurve.bIsLooped && PtIdx < NumPoints) || (!InterpCurve.bIsLooped && PtIdx < LastPoint)));

	const float NextInVal = (InterpCurve.bIsLooped && PtIdx == LastPoint) ? (InterpCurve.Points[LastPoint].InVal + InterpCurve.LoopKeyOffset) : InterpCurve.Points[NextPtIdx].InVal;

	if (CIM_Constant == InterpCurve.Points[PtIdx].InterpMode || CIM_Linear == InterpCurve.Points[PtIdx].InterpMode)
	{

		const float Distance = (InterpCurve.Points[PtIdx].OutVal - InterpCurve.Points[NextPtIdx].OutVal).SizeSquared();
		OutDisplacementSq += Distance;

		float ActualDistance = Distance;
		if (float(PtIdx) <= CurInputKey)
		{
			ActualDistance = ActualDistance * (float(NextPtIdx) - CurInputKey);
		}

		if (NeedDisplacementSq > ActualDistance)
		{
			return -1.0f;
		}

		float BestResult = (NeedDisplacementSq / Distance) + PtIdx;
		if (BestResult <= CurInputKey)
		{
			BestResult = -1.0f;
		}
		return BestResult;
	}
	const float Diff = NextInVal - InterpCurve.Points[PtIdx].InVal;


	{
		const int32 PointsChecked = 3;
		const int32 IterationNum = 3;
		const float Scale = 0.75;

		//使用牛顿迭代法迭代三次，分别为0，0.5，1
		float ValuesT[PointsChecked];
		ValuesT[0] = 0.0f;
		ValuesT[1] = 0.5f;
		ValuesT[2] = 1.0f;

		T InitialPoints[PointsChecked];
		InitialPoints[0] = InterpCurve.Points[PtIdx].OutVal;
		InitialPoints[1] = FMath::CubicInterp(InterpCurve.Points[PtIdx].OutVal
			, InterpCurve.Points[PtIdx].LeaveTangent * Diff, InterpCurve.Points[NextPtIdx].OutVal
			, InterpCurve.Points[NextPtIdx].ArriveTangent * Diff, ValuesT[1]);
		InitialPoints[2] = InterpCurve.Points[NextPtIdx].OutVal;

		// float DistancesSq[PointsChecked];

		// for (int32 point = 0; point < PointsChecked; ++point)
		// {
		// 	//Algorithm explanation: http://permalink.gmane.org/gmane.games.devel.sweng/8285
		// 	T FoundPoint = InitialPoints[point];
		// 	float LastMove = 1.0f;
		// 	for (int32 iter = 0; iter < IterationNum; ++iter)
		// 	{
		// 		const T LastBestTangent = FMath::CubicInterpDerivative(Points[PtIdx].OutVal
		// 			, Points[PtIdx].LeaveTangent * Diff, Points[NextPtIdx].OutVal
		// 			, Points[NextPtIdx].ArriveTangent * Diff, ValuesT[point]);
		// 		const T Delta = (PointInSpace - FoundPoint);
		// 		float Move = (LastBestTangent | Delta) / LastBestTangent.SizeSquared();
		// 		Move = FMath::Clamp(Move, -LastMove * Scale, LastMove * Scale);
		// 		ValuesT[point] += Move;
		// 		ValuesT[point] = FMath::Clamp(ValuesT[point], 0.0f, 1.0f);
		// 		LastMove = FMath::Abs(Move);
		// 		FoundPoint = FMath::CubicInterp(Points[PtIdx].OutVal, Points[PtIdx].LeaveTangent * Diff
		// 			, Points[NextPtIdx].OutVal, Points[NextPtIdx].ArriveTangent * Diff, ValuesT[point]);
		// 	}
		// 	DistancesSq[point] = (FoundPoint - PointInSpace).SizeSquared();
		// 	ValuesT[point] = ValuesT[point] * Diff + Points[PtIdx].InVal;
		// }
	}

	return -1.0f;
}

float UPathControllerTraceAsset::GetSegmentLength(const int32 Index, const float Param) const
{
	return SplineCurves.GetSegmentLength(Index, Param, bClosedLoop, FVector{ 1.0f });
}

float UPathControllerTraceAsset::GetSegmentParamFromLength(const int32 Index, const float Length,
	const float SegmentLength) const
{
	if (SegmentLength == 0.0f)
	{
		return 0.0f;
	}

	// Given a function P(x) which yields points along a spline with x = 0...1, we can define a function L(t) to be the
	// Euclidian length of the spline from P(0) to P(t):
	//
	//    L(t) = integral of |dP/dt| dt
	//         = integral of sqrt((dx/dt)^2 + (dy/dt)^2 + (dz/dt)^2) dt
	//
	// This method evaluates the inverse of this function, i.e. given a length d, it obtains a suitable value for t such that:
	//    L(t) - d = 0
	//
	// We use Newton-Raphson to iteratively converge on the result:
	//
	//    t' = t - f(t) / (df/dt)
	//
	// where: t is an initial estimate of the result, obtained through basic linear interpolation,
	//        f(t) is the function whose root we wish to find = L(t) - d,
	//        (df/dt) = d(L(t))/dt = |dP/dt|

	// TODO: check if this works OK with delta InVal != 1.0f

	const int32 NumPoints = SplineCurves.Position.Points.Num();
	const int32 LastPoint = NumPoints - 1;

	check(Index >= 0 && ((bClosedLoop && Index < NumPoints) || (!bClosedLoop && Index < LastPoint)));
	check(Length >= 0.0f && Length <= SegmentLength);

	float Param = Length / SegmentLength;  // initial estimate for t

	// two iterations of Newton-Raphson is enough
	for (int32 Iteration = 0; Iteration < 2; ++Iteration)
	{
		float TangentMagnitude = SplineCurves.Position.EvalDerivative(Index + Param, FVector::ZeroVector).Size();
		if (TangentMagnitude > 0.0f)
		{
			Param -= (GetSegmentLength(Index, Param) - Length) / TangentMagnitude;
			Param = FMath::Clamp(Param, 0.0f, 1.0f);
		}
	}

	return Param;

}
