// Fill out your copyright notice in the Description page of Project Settings.


#include "DLNPCCharacter.h"

#include "Component/DLCharacterMovementComponentBase.h"


ADLNPCCharacter::ADLNPCCharacter(const FObjectInitializer& ObjectInitializer)
	:ADLCharacterBase(ObjectInitializer)
{
}

void ADLNPCCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ADLNPCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADLNPCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

const FLockableUnitConfig& ADLNPCCharacter::GetLockableUnitConfig() const
{
	if (!LockableUnitConfig.IsSet())
	{
		FLockableUnitConfig Config;
		Config.BeLockedPriority = DTInfo.BeLockedPriority;
		Config.PlayerCanLockDistanceSquared = DTInfo.PlayerCanLockDistance * DTInfo.PlayerCanLockDistance;
		Config.PlayerCanLockMaxDistanceSquared = DTInfo.PlayerCanLockMaxDistance * DTInfo.PlayerCanLockMaxDistance;
		Config.FixedLockPointIndex = DTInfo.FixedLockPointIndex;
		Config.PlayerCanFreeLockMaxDistanceSquared = DTInfo.PlayerCanFreeLockMaxDistance * DTInfo.PlayerCanFreeLockMaxDistance;

		const_cast<ADLNPCCharacter*>(this)->LockableUnitConfig = Config;
	}

	return LockableUnitConfig.GetValue();
}

FGameplayTag ADLNPCCharacter::GetPartByBodyName(const FName BodyName)
{
	if (BodyNameMapParts.Contains(BodyName))
	{
		return BodyNameMapParts[BodyName];
	}
	return {};
}

FGameplayTag ADLNPCCharacter::GetPartByBodyIndex(const int32 Index)
{
	const FName BodyName = GetBodyNameByIndex(Index);
	if (BodyName.IsNone())
	{
		return {};
	}

	return GetPartByBodyName(BodyName);
}

FName ADLNPCCharacter::GetBodyNameByIndex(const int32 Index)
{
	const auto& Array = GetMesh()->GetPhysicsAsset()->SkeletalBodySetups;
	if (Index != INDEX_NONE && Array.Num() > Index)
	{
		return Array[Index]->BoneName;
	}
	return NAME_None;
}

int32 ADLNPCCharacter::GetPartAttrIndexByTag(const FGameplayTag& PartTag)
{
	const auto Value = DTInfo.PartsAttributeArray.FindByKey(PartTag);
	if (Value)
	{
		return Value->PartIndex;
	}
	return INDEX_NONE;
}

int32 ADLNPCCharacter::GetDefaultPartAttrIndex() const
{
	return DTInfo.DefaultPartIndex;
}

float ADLNPCCharacter::CalculateMaxSpeed(const float BaseSpeed, const float Scale)
{
	// TODO  NPC 可能有不同的计算策略

	if (GetAISpeed(AISpeed)) return AISpeed;

	return Super::CalculateMaxSpeed(BaseSpeed, Scale);
}

void ADLNPCCharacter::OnInitCharacter(const FCharacterInfoBase& InBaseInfo)
{
	Super::OnInitCharacter(InBaseInfo);

	const auto Data = GetCharacterDataTable<FDTNPCCharacterInfoRow>(DRCharacter::CreateNPCDataRegistryId(InBaseInfo.Id));

	if (ensureAlwaysMsgf(Data, TEXT("NPC 的配置表读取失败，ID: %s"), *InBaseInfo.Id.ToString()))
	{
		bIsInitDT = true;
		DTInfo = *Data;
	}

	BodyNameMapParts = DTInfo.BodyNameMapParts;
}


void ADLNPCCharacter::NetMulticast_ChangeTag_Implementation(FGameplayTagContainer InRemoveGameplayTags,
	const FGameplayTagContainer AddGameplayTag)
{
	if (InRemoveGameplayTags.IsValid())
	{
		RemoveOwnerTags(InRemoveGameplayTags);
	}
	if (AddGameplayTag.IsValid())
	{
		AddOwnerTags(AddGameplayTag);
	}
}
void ADLNPCCharacter::OnUninitCharacter()
{
	Super::OnUninitCharacter();


}

void ADLNPCCharacter::OnUpdateMoveInfo(const FCharacterMovementInfo& Info)
{
	Super::OnUpdateMoveInfo(Info);
}

const FDTCharacterInfoBaseRow* ADLNPCCharacter::GetBaseDataTableInfo(bool EnsureNotData)
{
	if (EnsureNotData)
	{
		ensure(bIsInitDT);
	}

	if (bIsInitDT)
	{
		return &DTInfo;
	}

	return nullptr;
}

void ADLNPCCharacter::SwitchBattleStageTag(FGameplayTag NewStageTag)
{
	if (NewStageTag != CurrentStageTag)
	{
		NetMulticast_ChangeTag(FGameplayTagContainer(CurrentStageTag),FGameplayTagContainer(NewStageTag));
		CurrentStageTag = NewStageTag;
	}
}

void ADLNPCCharacter::SwitchMovementTag(FGameplayTag NewMovementTag)
{
	if (NewMovementTag != CurrentMovementTag)
	{
		NetMulticast_ChangeTag(FGameplayTagContainer(CurrentMovementTag),FGameplayTagContainer(NewMovementTag));
		CurrentMovementTag = NewMovementTag;
	}
}

void ADLNPCCharacter::NetMulticast_SwitchMesh_Implementation(USkeletalMesh* NewMesh,bool bReinitPose)
{
	GetMesh()->SetSkeletalMesh(NewMesh,bReinitPose);
}


bool ADLNPCCharacter::GetAISpeed_Implementation(float& Speed)
{
	return false;
}

