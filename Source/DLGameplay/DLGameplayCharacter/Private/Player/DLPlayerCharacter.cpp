#include "DLPlayerCharacter.h"

#include "ConvertString.h"
#include "DisplayDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "DLGamePlayCharacterDef.h"
#include "DLGameplayCoreSetting.h"
#include "DLPlayerCharacterAssetDef.h"
#include "DLPlayerController.h"
#include "GameplayTagsManager.h"
#include "VCSpringArmComponent.h"
#include "Animation/DLAnimInstanceBaseV2.h"
#include "ASC/DLPlayerAbilitySysComponent.h"
#include "ASC/DLUnitAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PredictTask/UnderAttack/UnderAttackPredictTask.h"
#include "AbilityChainAsset.h"
#include "DLPlayerState.h"
#include "Component/DLCharacterMovementComponentBase.h"

ADLPlayerCharacter::ADLPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraBoom = CreateDefaultSubobject<UVCSpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	const auto Result = UGameplayTagsManager::Get().FindTagNode(TEXT("Gameplay.VirtualCameraID"));
	if (Result.IsValid())
	{
		int32 Order = 0;
		const auto VCTags = Result.Get()->GetChildTagNodes();
		for (const auto& Tag : VCTags)
		{
			const auto& Cam = VirtualCamMap.FindOrAdd(Tag->GetCompleteTag()) = CreateDefaultSubobject<UVirtualCam>(Tag->GetSimpleTagName());
			Cam->Order = Order;
			Order++;
		}
	}

}


FName ADLPlayerCharacter::GetCurrentScene()
{
	return GetPlayerStateChecked<ADLPlayerState>()->GetCurrentScene();
}

void ADLPlayerCharacter::SetEnterNewScene(FName SceneId)
{
	const auto PS = GetPlayerStateChecked<ADLPlayerState>();
	if (PS)
	{
		PS->SetNewScene(SceneId);
	}
}

float ADLPlayerCharacter::CalculateMaxSpeed(const float BaseSpeed, const float Scale)
{
	float Coefficient = 1.0f;

	static volatile bool EnabledLog = false;


	if (IsLockTargetUnit())
	{
		const float ACos = FVector::DotProduct(GetVelocity().GetSafeNormal(), GetActorRotation().Vector().GetSafeNormal());
		const float Radians = FMath::Acos(ACos);
		const float Angle = FMath::RadiansToDegrees(Radians);

		if (GetLocalRole() == ENetRole::ROLE_Authority && EnabledLog)
		{
			UE_LOG(LogTemp, Log, TEXT("Server Rotation %s  ACos %f Angle %f"), *GetActorRotation().ToString(), ACos, Angle);
		}


		if (const UCurveFloat* Curve = GetDataTable().LockSpeedCurve)
		{
			Coefficient = Curve->GetFloatValue(Angle);
		}
	}


	const float Speed = BaseSpeed * Scale * Coefficient * CurrentMoveSpeedCoefficient;


	if (IsLockTargetUnit())
	{
		if (GetLocalRole() == ENetRole::ROLE_Authority && EnabledLog)
		{
			UE_LOG(LogTemp, Log, TEXT("Server %f"), Speed);
		}
	}


	return Speed;
}

bool ADLPlayerCharacter::LoadDataTable_Implementation(FDTPlayerCharacterInfoRow& Out)
{
	const auto DT = GetCharacterDataTable<FDTPlayerCharacterInfoRow>(DRCharacter::CreatePlayerDataRegistryId(GetCharacterIdChecked()));
	if (ensureAlwaysMsgf(DT, TEXT("数据表一定能加载到")))
	{
		Out = *DT;
		return true;
	}
	return false;
}

void ADLPlayerCharacter::OnInitCharacter(const FCharacterInfoBase& InBaseInfo)
{
	Super::OnInitCharacter(InBaseInfo);

	const bool IsLoadOk = LoadDataTable(CharacterDataTable);
	ensureAlwaysMsgf(IsLoadOk, TEXT("一定能加载成功"));
	bIsInitDataTable = true;

	PredictTaskComponent->OnActivePredictTask.AddWeakLambda(this, [this](UDLGameplayPredictTask* Task)
	{
		if (ADLPlayerController* Controller = Cast<ADLPlayerController>(GetController()))
		{
			// TODO 这里需要优化 直接转为 UnderattackTask
			if (Cast<UDLUnderAttackTask>(Task))
			{
				Controller->OnActiveUnderAttack(Task);
			}
		}
	});

	PredictTaskComponent->OnEndPredictTask.AddWeakLambda(this, [this](UDLGameplayPredictTask* Task, bool Cancel)
	{
		if (ADLPlayerController* Controller = Cast<ADLPlayerController>(GetController()))
		{
			// TODO 这里需要优化 直接转为 UnderattackTask
			if (Cast<UDLUnderAttackTask>(Task))
			{
				Controller->OnEndUnderAttack(Task, Cancel);
			}
		}
	});


	const auto ASC = GetPlayerASC();

	// 仅仅主控端玩家初始化连招表
	if (ASC && GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		ASC->InitAbilityChain(GetPlayerAsset()->AbilityChainAsset.Get());
	}
}

void ADLPlayerCharacter::OnUninitCharacter()
{
	Super::OnUninitCharacter();


}

void ADLPlayerCharacter::UpdateLockTargetState()
{
	Super::UpdateLockTargetState();

	if (IsLockTargetUnit())
	{
		CameraBoom->LockTarget(GetLockPoint());
		CameraBoom->LockTargetArmLengthOffset(GetLockPoint()->LockArmLengthOffset);
		OnCamLocked();
	}
	else
	{
		CameraBoom->LockTarget(nullptr);
		CameraBoom->LockTargetArmLengthOffset(0);
		OnCamUnLocked();
	}
}

void ADLPlayerCharacter::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

}

void ADLPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ADLPlayerCharacter::OnPlayerStateChange(const FGameplayTagContainer& AllState, const FGameplayTag& Tag, bool IsAdd)
{
	Super::OnPlayerStateChange(AllState, Tag, IsAdd);

	const auto MyPlayerAsset = GetPlayerAsset();
	if (IsLocallyControlled() && MyPlayerAsset)
	{
		bool HasState = false;
		for (const auto& Def : MyPlayerAsset->MovementVCDefines)
		{
			if (AllState.HasTag(Def.MoveState))
			{
				HasState = true;

				if (!VCMoveStateActive.MatchesTag(Def.MoveState))
				{
					VCMoveStateActive = Def.MoveState;
					this->ActiveVirtualCamera(MyPlayerAsset->MovementVirtualCameraChannelTag, Def.VCDataAssetArray, Def.InTimeOutAction);
				}

				break;
			}
		}

		if (VCMoveStateActive.IsValid())
		{
			if (!HasState)
			{
				VCMoveStateActive = {};
				this->StopVirtualCamera(MyPlayerAsset->MovementVirtualCameraChannelTag);
			}
		}
	}
}

void ADLPlayerCharacter::OnUpdateMoveInfo(const FCharacterMovementInfo& Info)
{
	Super::OnUpdateMoveInfo(Info);

	this->SetCurrentMoveSpeedCoefficient(Info.MoveSpeedCoefficient);
}

const FDTCharacterInfoBaseRow* ADLPlayerCharacter::GetBaseDataTableInfo(const bool EnsureNotData)
{
	if (EnsureNotData)
	{
		ensure(bIsInitDataTable);
	}

	if (bIsInitDataTable)
	{
		return &GetDataTable();
	}
	return nullptr;
}

void ADLPlayerCharacter::ActiveVirtualCamera(const FGameplayTag& VirtualCameraID, TArray<UVCDataAssetDef*> DataArray,
                                             const EVCTimeOutAction InTimeOutAction)
{
	if (!ensureAlwaysMsgf(VirtualCamMap.Contains(VirtualCameraID), TEXT("没有找到虚拟相机，%s"), *VirtualCameraID.ToString()))
	{
		return;
	}
	VirtualCamMap[VirtualCameraID]->VCSetProcessorData(this);
	VirtualCamMap[VirtualCameraID]->VCActiveArrayWithTimer(DataArray, InTimeOutAction);

	UE_LOG(LogDLGamePlayChracter, Log, TEXT("Active Virtual Camera %s"), *VirtualCameraID.ToString());
}


void ADLPlayerCharacter::StopVirtualCamera(const FGameplayTag& VirtualCameraID)
{
	if (!ensureAlwaysMsgf(VirtualCamMap.Contains(VirtualCameraID), TEXT("没有找到虚拟相机，%s"), *VirtualCameraID.ToString()))
	{
		return;
	}

	VirtualCamMap[VirtualCameraID]->VCStopActive();

	UE_LOG(LogDLGamePlayChracter, Log, TEXT("Stop Virtual Camera %s"), *VirtualCameraID.ToString());
}

void ADLPlayerCharacter::SetIsMouseMoved(bool BisMoved)
{
	BisMouseMoved = BisMoved;
}

bool ADLPlayerCharacter::CheckIsMouseMoved()
{
	return BisMouseMoved;
}

USceneComponent* ADLPlayerCharacter::GetDLSpringArm()
{
	return  this->CameraBoom;
}

const FDTPlayerCharacterInfoRow& ADLPlayerCharacter::GetDataTable()
{
	return CharacterDataTable;
}

void ADLPlayerCharacter::SetCurrentMoveSpeedCoefficient(const float Coefficient)
{
	CurrentMoveSpeedCoefficient = Coefficient;
}

const UDLPlayerCharacterAsset* ADLPlayerCharacter::GetPlayerAsset() const
{
	return Cast<UDLPlayerCharacterAsset>(MyAsset);
}

void ADLPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UDLPlayerAbilitySysComponent* ADLPlayerCharacter::GetPlayerASC() const
{
	return Cast<UDLPlayerAbilitySysComponent>(GetAbilitySystemComponent());
}
