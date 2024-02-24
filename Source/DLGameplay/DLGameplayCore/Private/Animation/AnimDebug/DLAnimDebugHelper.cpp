#include "DLGameplayCore/Public/Animation/AnimDebug/DLAnimDebugHelper.h"

#include "DrawDebugHelpers.h"

void DLAnimDebugHelper::DrawDebugLineTraceSingle(const UWorld* World, const FVector& Start, const FVector& End,
	EDrawDebugTrace::Type DrawDebugType, bool bHit, const FHitResult& OutHit, FLinearColor TraceColor,
	FLinearColor TraceHitColor, float DrawTime)
{
#if WITH_EDITOR
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHit.bBlockingHit)
		{
			::DrawDebugLine(World, Start, OutHit.ImpactPoint, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, OutHit.ImpactPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugPoint(World, OutHit.ImpactPoint, 16.0f, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
#endif
}

void DLAnimDebugHelper::DrawDebugCapsuleTraceSingle(const UWorld* World, const FVector& Start, const FVector& End,
	const FCollisionShape& CollisionShape, EDrawDebugTrace::Type DrawDebugType, bool bHit, const FHitResult& OutHit,
	FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
#if WITH_EDITOR
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHit.bBlockingHit)
		{
			::DrawDebugCapsule(World, Start, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(),
							   FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugCapsule(World, OutHit.Location, CollisionShape.GetCapsuleHalfHeight(),
							   CollisionShape.GetCapsuleRadius(), FQuat::Identity, TraceColor.ToFColor(true),
							   bPersistent, LifeTime);
			::DrawDebugLine(World, Start, OutHit.Location, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugPoint(World, OutHit.ImpactPoint, 16.0f, TraceColor.ToFColor(true), bPersistent, LifeTime);

			::DrawDebugCapsule(World, End, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(),
							   FQuat::Identity, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, OutHit.Location, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			::DrawDebugCapsule(World, Start, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(),
							   FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugCapsule(World, End, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(),
							   FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
#endif
}

void DLAnimDebugHelper::DrawDebugSweptSphere(const UWorld* InWorld, FVector const& Start, FVector const& End,
	float Radius, FColor const& Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority)
{
#if WITH_EDITOR
	FVector const TraceVec = End - Start;
	float const Dist = TraceVec.Size();

	FVector const Center = Start + TraceVec * 0.5f;
	float const HalfHeight = (Dist * 0.5f) + Radius;

	FQuat const CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	::DrawDebugCapsule(InWorld, Center, HalfHeight, Radius, CapsuleRot, Color, bPersistentLines, LifeTime,
					   DepthPriority);
#endif
}

void DLAnimDebugHelper::DrawDebugSphereTraceSingle(const UWorld* World, const FVector& Start, const FVector& End,
	const FCollisionShape& CollisionShape, EDrawDebugTrace::Type DrawDebugType, bool bHit, const FHitResult& OutHit,
	FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
#if WITH_EDITOR
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHit.bBlockingHit)
		{
			DrawDebugSweptSphere(World, Start, OutHit.Location, CollisionShape.GetSphereRadius(),
								   TraceColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugSweptSphere(World, OutHit.Location, End, CollisionShape.GetSphereRadius(),
								   TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugPoint(World, OutHit.ImpactPoint, 16.0f, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			DrawDebugSweptSphere(World, Start, End, CollisionShape.GetSphereRadius(), TraceColor.ToFColor(true),
								   bPersistent, LifeTime);
		}
	}
#endif
}
