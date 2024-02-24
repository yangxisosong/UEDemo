// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "TargetSelectorActor.generated.h"

UCLASS()
class AAnchorActor : public AActor
{
	GENERATED_BODY()
public:
	AAnchorActor()
	{
		SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
		SphereComponent->ShapeColor = FColor::Green;
		SphereComponent->SetupAttachment(GetRootComponent());
		SphereComponent->SetSphereRadius(1.0f);
		SphereComponent->SetVisibility(false);
	}

public:
	UPROPERTY()
	USphereComponent* SphereComponent;
};

UCLASS()
class AShapeActor : public AActor
{
	GENERATED_BODY()
};

UCLASS()
class ASphereActor : public AShapeActor
{
	GENERATED_BODY()
public:
	ASphereActor()
	{
		SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
		SphereComponent->ShapeColor = FColor::Orange;
		SphereComponent->SetupAttachment(GetRootComponent());
		SphereComponent->SetSphereRadius(10.0f);
	}

public:
	UPROPERTY()
	USphereComponent* SphereComponent;
};

UCLASS()
class ABoxActor : public AShapeActor
{
	GENERATED_BODY()
public:
	ABoxActor()
	{
		BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
		BoxComponent->ShapeColor = FColor::Orange;
		BoxComponent->SetupAttachment(GetRootComponent());
		BoxComponent->SetBoxExtent(FVector(10, 10, 10));
	}

public:
	UPROPERTY()
	UBoxComponent* BoxComponent;
};

UCLASS()
class ACapsuleActor : public AShapeActor
{
	GENERATED_BODY()
public:
	ACapsuleActor()
	{
		CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
		CapsuleComponent->ShapeColor = FColor::Orange;
		CapsuleComponent->SetupAttachment(GetRootComponent());
		CapsuleComponent->SetCapsuleSize(10.0f, 20.0f);
	}

public:
	UPROPERTY()
	UCapsuleComponent* CapsuleComponent;
};

UCLASS()
class APointActor : public AShapeActor
{
	GENERATED_BODY()
public:
	APointActor()
	{
		SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PointCollision"));
		SphereComponent->ShapeColor = FColor::Red;
		SphereComponent->SetupAttachment(GetRootComponent());
		SphereComponent->SetSphereRadius(1);

		ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ForwardArrow"));
		if (ArrowComponent)
		{
			ArrowComponent->ArrowColor = FColor::Red;
			ArrowComponent->SetupAttachment(GetRootComponent());
			ArrowComponent->SetEditorScale(0.5);
		}
	}

public:
	UPROPERTY()
	USphereComponent* SphereComponent;
	UArrowComponent* ArrowComponent;
};
