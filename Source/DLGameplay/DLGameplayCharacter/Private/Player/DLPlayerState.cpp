// Fill out your copyright notice in the Description page of Project Settings.


#include "DLPlayerState.h"

#include "DataRegistrySubsystem.h"
#include "DLAssetManager.h"
#include "DLPlayerCharacterAssetDef.h"
#include "ASC/DLPlayerAbilitySysComponent.h"
#include "AttributeSet/DLCharacterAttributeSet.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayFramwork/Component/DLPlayerStateExtensionComp.h"
#include "Interface/IDLLocalPlayerGameplayInterface.h"


ADLPlayerState::ADLPlayerState()
{
	PlayerAbilitySys = CreateDefaultSubobject<UDLPlayerAbilitySysComponent>(TEXT("AbilitySyetemComp"));

	PlayerAbilitySys->SetIsReplicated(true);
	PlayerAbilitySys->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	this->SetAbilitySystemComp(PlayerAbilitySys);


	AttributeSet = CreateDefaultSubobject<UDLPlayerAttributeSet>(TEXT("PlayerAttributeSet"));
	NetUpdateFrequency = 60.f;
}

void ADLPlayerState::SetNewScene(const FName NewSceneId)
{
	CurrentSceneId = NewSceneId;

	if (this->GetPawn()->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		FDLLocalPlayerEnterNewSceneMsgBody Body;
		Body.SceneId = CurrentSceneId;

		auto& MsgSys = UGameplayMessageSubsystem::Get(this);
		MsgSys.BroadcastMessage(EDLGameplayMsgType::LocalPlayer_Gameplay_EnterNewScene, Body);
		Body.SceneId = CurrentSceneId;
	}
}


void ADLPlayerState::BeginPlay()
{
	Super::BeginPlay();

}


bool ADLPlayerState::ServerInitPlayerBaseInfo(const FPlayerBaseInfo& Info)
{
	if (!Super::ServerInitPlayerBaseInfo(Info))
	{
		return false;
	}
	return true;
}

void ADLPlayerState::OnInitPlayerData(const FPlayerBaseInfo& Info)
{
	Super::OnInitPlayerData(Info);

	FDTPlayerCharacterInfoRow CharacterInfo;
	this->LoadDataTable(CharacterInfo);

	this->InitUnitBaseAttribute(&CharacterInfo);

	UDLPlayerAttributeSet* Attribute = CastChecked<UDLPlayerAttributeSet>(AttributeSet);

	Attribute->InitStamina(CharacterInfo.MaxStamina);
	Attribute->InitMaxStamina(CharacterInfo.MaxStamina);
	Attribute->InitFastRecoveryStaminaSpeed(CharacterInfo.FastRecoveryStaminaSpeed);
	Attribute->InitLowRecoveryStaminaSpeed(CharacterInfo.LowRecoveryStaminaSpeed);
	Attribute->InitConsumeStaminaWhenSprint(CharacterInfo.ConsumeStaminaWhenSprint);

	Attribute->InitMaxRage(CharacterInfo.MaxRage);
	Attribute->InitSoulPower(CharacterInfo.SoulPower);
	Attribute->InitMaxSoulPower(CharacterInfo.SoulPower);


	const auto& DefenseAttr = CharacterInfo.DefenseAttr;

	// 防御相关的属性值
	for (TFieldIterator<FIntProperty> It(FDLCharacterDefenseAttr::StaticStruct()); It; ++It)
	{
		const FIntProperty* P = *It;
		const int32* Value = P->ContainerPtrToValuePtr<int32>(&DefenseAttr);
		InitAttribute(Attribute, P->GetFName(), *Value);
	}

	// 部位破坏
	for (const auto& PartInfo : CharacterInfo.DestroyPartsAttrArray)
	{
		for (TFieldIterator<FProperty> It(FDLDestroyPartsAttr::StaticStruct()); It; ++It)
		{
			if ((*It)->GetFName() == TEXT("Index"))
			{
				continue;
			}

			const FName Name = *FString::Printf(TEXT("%s_Part%d"), *((*It)->GetFName().ToString()), PartInfo.Index);

			if (const FIntProperty* P = CastField<FIntProperty>(*It))
			{
				const int32* Value = P->ContainerPtrToValuePtr<int32>(&PartInfo);
				InitAttribute(Attribute, Name, *Value);
			}
			else if (const FFloatProperty* FP = CastField<FFloatProperty>(*It))
			{
				const float* Value = FP->ContainerPtrToValuePtr<float>(&PartInfo);
				InitAttribute(Attribute, Name, *Value);
			}
		}
	}
}

bool ADLPlayerState::LoadDataTable_Implementation(FDTPlayerCharacterInfoRow& OutInfo)
{
	const auto DataRegistry = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>();
	const auto CharacterInfo = DataRegistry->GetCachedItem<FDTPlayerCharacterInfoRow>(DRCharacter::CreatePlayerDataRegistryId(GetCharacterId()));
	if (!CharacterInfo)
	{
		return false;
	}

	OutInfo = *CharacterInfo;

	return true;
}

void ADLPlayerState::OnRep_CharacterBaseInfo()
{
	Super::OnRep_CharacterBaseInfo();

}

