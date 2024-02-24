#include "DLNPCState.h"

#include "DataRegistrySubsystem.h"
#include "ASC/DLNPCAbilitySysComponent.h"
#include "AttributeSet/DLCharacterAttributeSet.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"


ADLNPCState::ADLNPCState()
{
	NPCAbilitySys = CreateDefaultSubobject<UDLNPCAbilitySysComponent>(TEXT("NPCAbilitySys"));
	NPCAbilitySys->SetIsReplicated(true);
	NPCAbilitySys->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	this->SetAbilitySystemComp(NPCAbilitySys);

	AttributeSet = CreateDefaultSubobject<UDLNPCAttributeSet>(TEXT("NPCAttributeSet"));
}

bool ADLNPCState::ServerInitPlayerBaseInfo(const FPlayerBaseInfo& Info)
{
	if (!Super::ServerInitPlayerBaseInfo(Info))
	{
		return false;
	}

	return true;
}

void ADLNPCState::OnInitPlayerData(const FPlayerBaseInfo& Info)
{
	Super::OnInitPlayerData(Info);

	const auto DataRegistry = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>();
	const auto CharacterInfo = DataRegistry->GetCachedItem<FDTNPCCharacterInfoRow>(DRCharacter::CreateNPCDataRegistryId(GetCharacterId()));
	if (!CharacterInfo)
	{
		return;
	}

	this->InitUnitBaseAttribute(CharacterInfo);

	UDLNPCAttributeSet* Attribute = CastChecked<UDLNPCAttributeSet>(AttributeSet);

	Attribute->InitStrength(CharacterInfo->Strength);
	Attribute->InitMaxStrength(CharacterInfo->Strength);
	Attribute->InitStrengthRecoverSpeed(CharacterInfo->StrengthRecoverSpeed);
	Attribute->InitStrengthCostSpeed(CharacterInfo->StrengthCostSpeed);

	const auto& PartAttrArray = CharacterInfo->PartsAttributeArray;
	for (const auto& PartAttr : PartAttrArray)
	{
		InitAttribute(Attribute, *FString::Printf(TEXT("DestroyValue_Part%d"), PartAttr.PartIndex), PartAttr.DestroyMaxValue);
		InitAttribute(Attribute, *FString::Printf(TEXT("MutilationDefense_Part%d"), PartAttr.PartIndex), PartAttr.MutilationDefense);

		const auto& DefenseAttr = PartAttr.DefenseAttr;

		// 防御相关的属性值
		for (TFieldIterator<FIntProperty> It(FDLCharacterDefenseAttr::StaticStruct()); It; ++It)
		{
			const FIntProperty* P = *It;
			const int32* Value = P->ContainerPtrToValuePtr<int32>(&DefenseAttr);
			InitAttribute(Attribute, *FString::Printf(TEXT("%s_Part%d"), *P->GetFName().ToString(), PartAttr.PartIndex), *Value);
		}
	}
}

