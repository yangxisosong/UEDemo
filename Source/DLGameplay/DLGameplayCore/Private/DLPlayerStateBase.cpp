#include "GameplayFramwork/DLPlayerStateBase.h"

#include "ConvertString.h"
#include "DLGameplayAbilityBase.h"
#include "DLGameplayCoreSetting.h"
#include "ASC/DLUnitAbilitySystemComponent.h"
#include "Misc/RuntimeErrors.h"
#include "Net/UnrealNetwork.h"
#include "GameplayCoreLog.h"
#include "AttributeSet/DLUnitAttributeBaseSet.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "GameplayFramwork/DLCharacterBase.h"
#include "GameplayFramwork/DLGameState.h"
#include "GameplayFramwork/Component/DLCharacterBattleStateComp.h"
#include "GameplayFramwork/Component/DLCharacterStateExtensionComp.h"
#include "GameplayFramwork/Component/DLLocalPlayerAbilitySysComp.h"
#include "GameplayFramwork/Component/DLPlayerStateExtensionComp.h"
#include "Kismet/GameplayStatics.h"
#include "Types/CharacterWeaponDef.h"

void ADLPlayerStateBase::InitUnitBaseAttribute(const FDTCharacterInfoBaseRow* const CharacterInfo) const
{
	UDLUnitAttributeBaseSet* Attribute = CastChecked<UDLUnitAttributeBaseSet>(AttributeSet);
	Attribute->InitHealth(CharacterInfo->MaxHealth);
	Attribute->InitMaxHealth(CharacterInfo->MaxHealth);
	Attribute->InitAttack(CharacterInfo->Attack);
	Attribute->InitSpeedBase(CharacterInfo->SpeedBase);

	Attribute->InitMaxTenacity(CharacterInfo->MaxTenacity);
	Attribute->InitTenacity(CharacterInfo->MaxTenacity);
	Attribute->InitLowRecoveryTenacitySpeed(CharacterInfo->LowRecoveryTenacitySpeed);
	Attribute->InitFastRecoveryTenacitySpeed(CharacterInfo->FastRecoveryTenacitySpeed);

	Attribute->InitMaxSpeedTotalScale(1.0f);
	Attribute->InitAbilityCDCoefficient(1.0f);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ADLPlayerStateBase::OnCharacterDied()
{
	if (GetASC())
	{
		GetASC()->AddGameplayTags(UDLGameplayCoreSetting::Get()->UnitDiedStateTag.GetSingleTagContainer());
	}

	this->Extension->PlayerPawnRemove();
}

ADLPlayerStateBase::ADLPlayerStateBase()
{
	Extension = CreateDefaultSubobject<UDLPlayerStateExtensionComp>(TEXT("PS Extension"));

	CharacterBattleStateComp = CreateDefaultSubobject<UDLCharacterBattleStateComp>(TEXT("CharacterBattleState"));
	CharacterBattleStateComp->SetIsReplicated(true);

	CharacterStateExtension = CreateDefaultSubobject<UDLCharacterStateExtensionComp>(TEXT("CharacterStateExtension"));
	CharacterStateExtension->SetIsReplicated(true);

	CharacterStateExtension->OnDied_CallAndRegister(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnCharacterDied));
}

bool ADLPlayerStateBase::ServerInitPlayerBaseInfo(const FPlayerBaseInfo& Info)
{
	if (this->GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystem))
	{
		ensureAlwaysMsgf(false, TEXT("客户端不可能执行这一步，上层逻辑有问题"));
		return false;
	}

	if (bInit)
	{
		return true;
	}

	if (!ensureAsRuntimeWarning(Info.Pawn.IsValid()))
	{
		return false;
	}

	// TODO  这里应该发出装上装备的通知等 ，目前进战斗不能换武器 ，先没写
	CharacterWeaponInfo = Info.CharacterWeaponInfo;

	AbilitySystem->InitAbilityActorInfo(this, Info.Pawn.Get());

	this->OnInitPlayerData(Info);

	// create effect context
	FGameplayEffectContextHandle effectContext = AbilitySystem->MakeEffectContext();
	effectContext.AddSourceObject(this);

	// init effect
	for (const auto& GameplayEffect : Info.GameplayEffectInfos)
	{
		if (!ensureAsRuntimeWarning(GameplayEffect.Class.IsValid()))
		{
			continue;
		}

		FGameplayEffectSpec Spec(GameplayEffect.Class->GetDefaultObject<UGameplayEffect>(), effectContext, GameplayEffect.Level);
		AbilitySystem->ApplyGameplayEffectSpecToSelf(Spec);
	}


	// init ability
	TArray<FGameplayAbilitySpec> InitGaArray;
	TArray<FGameplayAbilitySpec> InitGaAndActiveArray;

	for (const auto& AbilityInfo : Info.AbilityInfos)
	{
		if (!ensureAsRuntimeWarning(AbilityInfo.Class.IsValid()))
		{
			continue;
		}

		if (AbilityInfo.Class->IsChildOf<UDLGameplayAbilityBase>())
		{
			const auto AbilityDefaultObj = AbilityInfo.Class->GetDefaultObject<UDLGameplayAbilityBase>();
			if (AbilityDefaultObj)
			{
				FGameplayAbilitySpec Spec(AbilityDefaultObj, AbilityInfo.Level, static_cast<int32>(AbilityDefaultObj->GetInputId()));
				Spec.bActivateOnce = AbilityInfo.IsActiveWhenGive;
				InitGaArray.Add(Spec);
			}
		}
		else
		{
			FGameplayAbilitySpec Spec(AbilityInfo.Class->GetDefaultObject<UGameplayAbility>(), AbilityInfo.Level, 0);
			Spec.bActivateOnce = AbilityInfo.IsActiveWhenGive;
			
			InitGaArray.Add(Spec);
		}
	}

	const bool InitAbilitySysOk = AbilitySystem->GrantAbility(InitGaArray);
	if (!ensureAsRuntimeWarning(InitAbilitySysOk))
	{
		return false;
	}

	

	bInit = true;

	return true;
}

void ADLPlayerStateBase::OnInitPlayerData(const FPlayerBaseInfo& Info)
{

}

// 先临时用这个接口，后边需要修改
float ADLPlayerStateBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	//return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HasAuthority())
	{
		if (!CharacterStateExtension->IsDied())
		{
			const auto CurrentHp = GetASC()->GetNumericAttributeChecked(UDLUnitAttributeBaseSet::GetHealthAttribute());
			if (CurrentHp <= 0.9999f)
			{
				CharacterStateExtension->ServerSetCharacterDied();
			}
		}
	}

	return DamageAmount;
}

UAbilitySystemComponent* ADLPlayerStateBase::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

FOnGameplayAttributeValueChange& ADLPlayerStateBase::OnGameplayAttributeValueChange(const FGameplayAttribute& Attribute)
{
	static FOnGameplayAttributeValueChange Temp;
	if (ensureAlwaysMsgf(AbilitySystem, TEXT("一定存在 ASC")))
	{
		return AbilitySystem->GetGameplayAttributeValueChangeDelegate(Attribute);
	}
	return Temp;
}

UDLUnitAbilitySystemComponent* ADLPlayerStateBase::GetASC() const
{
	return Cast<UDLUnitAbilitySystemComponent>(GetAbilitySystemComponent());
}

void ADLPlayerStateBase::SetCharacterInfoBase(const FCharacterInfoBase& InCharacterInfo)
{
	CharacterBaseInfo = InCharacterInfo;

	bIsLoadCharacterBaseInfo = true;

	Extension->PlayerBaseInfoReady(CharacterBaseInfo);
}

void ADLPlayerStateBase::ClientLoadComplate_Implementation()
{
	auto GS = Cast<ADLGameState>(UGameplayStatics::GetGameState(this));
	if (GS)
	{
		UE_LOG(LogDLGameplayCore, Log, TEXT("ClientLoadComplate PlayerId: %d"), GetPlayerId());
		GS->ServerSetPlayerLoadComplate(GetPlayerId());
	}
}

const TArray<FDLCharacterWeaponInfo>& ADLPlayerStateBase::GetCurrentWeaponInfo()
{
	return CharacterWeaponInfo;
}

bool ADLPlayerStateBase::IsLoadCharacterBaseInfo() const
{
	return bIsLoadCharacterBaseInfo;
}

void ADLPlayerStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME_CONDITION_NOTIFY(ADLPlayerStateBase, CharacterBaseInfo, COND_InitialOnly, REPNOTIFY_Always);
}


ADLCharacterBase* ADLPlayerStateBase::GetCharacterBase() const
{
	return Cast<ADLCharacterBase>(GetPawn());
}


const FName& ADLPlayerStateBase::GetCharacterId() const
{
	ensureAlwaysMsgf(!CharacterBaseInfo.Id.IsNone(), TEXT("GetCharacterId 不应该为空 如果是  说明初始化时机有问题"));
	return CharacterBaseInfo.Id;
}

const FCharacterInfoBase& ADLPlayerStateBase::GetCharacterInfoBase() const
{
	ensureAlwaysMsgf(bIsLoadCharacterBaseInfo, TEXT("数据一定是加载了的，否则就会出错"));
	return CharacterBaseInfo;
}

void ADLPlayerStateBase::OnRep_CharacterBaseInfo()
{
	const auto Character = GetPawn<ADLCharacterBase>();

	UE_LOG(LogDLGameplayCore, Log, TEXT("<%s> ADLPlayerStateBase::OnRep_CharacterBaseInfo  %s  Pawn %s"), NET_ROLE_STR(this), TO_STR(this), TO_STR(Character));

	bIsLoadCharacterBaseInfo = true;

	Extension->PlayerBaseInfoReady(CharacterBaseInfo);
}

void ADLPlayerStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AbilitySystem)
	{
		AbilitySystem->StopAbility();
		AbilitySystem->AbilityActivatedCallbacks.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ADLPlayerStateBase::OnAbilityActive(UGameplayAbility* Ability)
{
	if (HasAuthority())
	{
		// 给服务端的 注入事件
		CharacterBattleStateComp->ServerInjectAbilityActiveEvent(Ability->AbilityTags);
	}
}

void ADLPlayerStateBase::SetAbilitySystemComp(UDLUnitAbilitySystemComponent* ASC)
{
	AbilitySystem = ASC;

	AbilitySystem->OnInitActorInfo.BindUObject(this, &ThisClass::OnAbilitySystemActorInfoInit);
	AbilitySystem->OnClearActorInfo.BindUObject(this, &ThisClass::OnAbilitySystemActorInfoClear);
	AbilitySystem->AbilityActivatedCallbacks.AddUObject(this, &ThisClass::OnAbilityActive);
}

void ADLPlayerStateBase::OnAbilitySystemActorInfoInit()
{
	if (GetPawn() && GetPawn()->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		LocalPlayerAbilitySys = NewObject<UDLLocalPlayerAbilitySysComp>(this, TEXT("LocalPlayerAbilitySysComp"));
		LocalPlayerAbilitySys->RegisterComponent();

		LocalPlayerAbilitySys->AbilitySystemReady(AbilitySystem);
	}

	Extension->PlayerAbilitySystemInit();
}

void ADLPlayerStateBase::OnAbilitySystemActorInfoClear()
{
	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		if (LocalPlayerAbilitySys)
		{
			LocalPlayerAbilitySys->MarkPendingKill();
			LocalPlayerAbilitySys = nullptr;
		}
	}
}
