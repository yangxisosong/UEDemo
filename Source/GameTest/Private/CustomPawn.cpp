#include "CustomPawn.h"

#include "GameFramework/FloatingPawnMovement.h"

ACustomPawn::ACustomPawn(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetCanBeDamaged(true);

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	NetPriority = 3.0f;

	BaseEyeHeight = 0.0f;
	bCollideWhenPlacing = false;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(FName(TEXT("SphereComponent")));
	SphereComponent->InitSphereRadius(30);
	SphereComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	SphereComponent->CanCharacterStepUpOn = ECB_No;
	SphereComponent->SetShouldUpdatePhysicsVolume(true);
	SphereComponent->SetCanEverAffectNavigation(false);
	SphereComponent->bDynamicObstacle = true;
	RootComponent = SphereComponent;

	CharacterMovement = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(FName(TEXT("MovementComponent022")));
	if (CharacterMovement)
	{
		CharacterMovement->UpdatedComponent = SphereComponent;
		UE_LOG(LogTemp, Warning, TEXT("Creat MovementComponent OK"));
		//CharacterMovement->SetUpdatedComponent(SphereComponent);
	}

	Mesh = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(FName(TEXT("SkeletalMesh")));
	if (Mesh)
	{
		Mesh->AlwaysLoadOnClient = true;
		Mesh->AlwaysLoadOnServer = true;
		Mesh->bOwnerNoSee = false;
		Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
		Mesh->bCastDynamicShadow = true;
		Mesh->bAffectDynamicIndirectLighting = true;
		Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		Mesh->SetupAttachment(SphereComponent);
		static FName MeshCollisionProfileName(TEXT("CharacterMesh"));
		Mesh->SetCollisionProfileName(MeshCollisionProfileName);
		Mesh->SetGenerateOverlapEvents(false);
		Mesh->SetCanEverAffectNavigation(false);
	}

	if (CharacterMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("Creat MovementComponent OK2"));
	}
}

void ACustomPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ACustomPawn::BeginPlay()
{
	Super::BeginPlay();
	if (!CharacterMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("BeginPlay MovementComponent None"));
	}
}

void ACustomPawn::PostInitializeComponents()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Pawn_PostInitComponents);

	Super::PostInitializeComponents();

	if (!IsPendingKill())
	{
		if (Mesh)
		{
			// force animation tick after movement component updates
			if (Mesh->PrimaryComponentTick.bCanEverTick && CharacterMovement)
			{
				Mesh->PrimaryComponentTick.AddPrerequisite(CharacterMovement, CharacterMovement->PrimaryComponentTick);
			}
		}
	}
}

UPawnMovementComponent* ACustomPawn::GetMovementComponent() const
{
	if(!CharacterMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetMovementComponent None"));
	}
	return CharacterMovement;
}

void ACustomPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 设置"移动"绑定。
	PlayerInputComponent->BindAxis("MoveForward", this, &ACustomPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACustomPawn::MoveRight);

	// 设置"操作"绑定。
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACustomPawn::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACustomPawn::StopJump);
}

void ACustomPawn::MoveForward(float Value)
{
	if (Value != 0)
	{
		FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
		Direction.Normalize();
		Direction.Z = 0;
		UE_LOG(LogTemp, Warning, TEXT("Directionis %s"), *Direction.ToString());
		AddMovementInput(Direction, Value);
	}
}

void ACustomPawn::MoveRight(float Value)
{
	if (Value != 0)
	{
		FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
		Direction.Normalize();

		AddMovementInput(Direction, Value);
	}
}

void ACustomPawn::StartJump()
{
}

void ACustomPawn::StopJump()
{
}
