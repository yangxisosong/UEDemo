// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLPathControllerComponent.h"
#include "ActorFactories/ActorFactoryBasicShape.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "PathControllerActor.generated.h"

UCLASS()
class APathControllerActor : public AActor
{
	GENERATED_BODY()
public:
	APathControllerActor()
	{
		PathControllerComponent = CreateDefaultSubobject<UDLPathControllerComponent>(TEXT("PathControllerComponent"));
		FString Path = UActorFactoryBasicShape::BasicSphere.ToString();
		const auto Mesh = ConstructorHelpersInternal::FindOrLoadObject<UStaticMesh>(
			Path,LOAD_None);
		
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Components"));
		if (Mesh)
		{
			MeshComponent->SetStaticMesh(Mesh);
		}
		MeshComponent->SetCollisionProfileName("OverlapAll");
		SetRootComponent(MeshComponent);
		Spline = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
		Spline->SetCollisionProfileName("OverlapAll");
	}

public:
	UPROPERTY()
	UDLPathControllerComponent* PathControllerComponent;
	UPROPERTY()
	UStaticMeshComponent* MeshComponent;
	UPROPERTY()
	USplineComponent* Spline;
};

UCLASS()
class APathControllerTargetActor : public AActor
{
	GENERATED_BODY()
public:
	APathControllerTargetActor()
	{
		//ConstructorHelpers::FObjectFinder<USkeletalMesh> PersonMesh(TEXT("/Game/Model/ren01"));
		FString Path = UActorFactoryBasicShape::BasicCube.ToString();
		const auto Mesh = ConstructorHelpersInternal::FindOrLoadObject<UStaticMesh>(
			Path,LOAD_None);
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Components"));
		if (Mesh)
		{
			MeshComponent->SetStaticMesh(Mesh);
		}
		SetRootComponent(MeshComponent);
		MeshComponent->SetCollisionProfileName("OverlapAll");
	}

	UPROPERTY()
	UStaticMeshComponent* MeshComponent;
};

UCLASS()
class APathControllerAxisAcotr : public AActor
{
	GENERATED_BODY()
public:
	APathControllerAxisAcotr()
	{
		//ConstructorHelpers::FObjectFinder<USkeletalMesh> PersonMesh(TEXT("/Game/Model/ren01"));
		FString Path = UActorFactoryBasicShape::BasicSphere.ToString();
		const auto Mesh = ConstructorHelpersInternal::FindOrLoadObject<UStaticMesh>(
			Path, LOAD_None);
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Components"));
		if (Mesh)
		{
			MeshComponent->SetStaticMesh(Mesh);

			SetActorScale3D(FVector(0.2, 0.2, 0.2));
		}
	}

public:
	UPROPERTY()
		UStaticMeshComponent* MeshComponent;
};