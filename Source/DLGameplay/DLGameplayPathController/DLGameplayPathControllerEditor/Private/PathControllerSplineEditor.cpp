// Fill out your copyright notice in the Description page of Project Settings.


#include "PathControllerSplineEditor.h"

#include "Components/SplineComponent.h"
#include "DLGameplayPathController/Public/PathControllerTraceAsset.h"


// Sets default values
APathControllerSplineEditor::APathControllerSplineEditor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

// Called when the game starts or when spawned
void APathControllerSplineEditor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APathControllerSplineEditor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APathControllerSplineEditor::CopyPathControllerAsset(const APathControllerSplineEditor* Source,
	UPathControllerTraceAsset* Target)
{
	Target->SplineCurves = Source->Spline->SplineCurves;
	Target->Duration = Source->Spline->Duration;
	Target->ReparamStepsPerSegment = Source->Spline->ReparamStepsPerSegment;
	Target->bStationaryEndpoints = Source->Spline->bStationaryEndpoints;
	Target->DefaultUpVector = Source->Spline->DefaultUpVector;

	Target->SetClosedLoop(Source->Spline->IsClosedLoop());
	Target->CustomSpeedCurve = Source->CustomSpeedCurve;
	Target->GravityAcceleration = Source->GravityAcceleration;


	// Target->FixWeight = Source->FixWeight;
	Target->RateScale = Source->RateScale;
	// Target->AllowRotate = Source->AllowRotate;
	Target->HomingTriggerTime = Source->HomingTriggerTime;
	Target->HomingFixAmplitude = Source->HomingFixAmplitude;
	Target->IterateFrequency = Source->IterateDelay;
	Target->TargetRadius = Source->TargetRadius;
}

