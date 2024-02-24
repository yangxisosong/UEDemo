#pragma once
#include "Kismet/KismetSystemLibrary.h"

class DLGAMEPLAYCORE_API DLAnimDebugHelper
{
	// utility functions to draw trace debug shapes,
	// 	which are derived from Engine/Private/KismetTraceUtils.h.
	// 	Sadly the functions are private, which was the reason
	// 	why there reimplemented here.
	static void DrawDebugLineTraceSingle(const UWorld* World,
										const FVector& Start,
										const FVector& End,
										EDrawDebugTrace::Type DrawDebugType,
										bool bHit,
										const FHitResult& OutHit,
										FLinearColor TraceColor,
										FLinearColor TraceHitColor,
										float DrawTime);

	static void DrawDebugCapsuleTraceSingle(const UWorld* World,
										   const FVector& Start,
										   const FVector& End,
										   const FCollisionShape& CollisionShape,
										   EDrawDebugTrace::Type DrawDebugType,
										   bool bHit,
										   const FHitResult& OutHit,
										   FLinearColor TraceColor,
										   FLinearColor TraceHitColor,
										   float DrawTime);

	static void DrawDebugSweptSphere(const UWorld* InWorld,
									 FVector const& Start,
									 FVector const& End,
									 float Radius,
									 FColor const& Color,
									 bool bPersistentLines = false,
									 float LifeTime = -1.f,
									 uint8 DepthPriority = 0);

	static void DrawDebugSphereTraceSingle(const UWorld* World,
										  const FVector& Start,
										  const FVector& End,
										  const FCollisionShape& CollisionShape,
										  EDrawDebugTrace::Type DrawDebugType,
										  bool bHit,
										  const FHitResult& OutHit,
										  FLinearColor TraceColor,
										  FLinearColor TraceHitColor,
										  float DrawTime);
};

