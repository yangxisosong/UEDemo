#include "DLGameplayAbilityBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DLAbilityActorInfo.h"
#include "DLAbilitySystemGlobal.h"
#include "DLAbilityLog.h"
#include "DLAbilityTagsDef.h"
#include "DLCalculateAbilityAttackInfo.h"
#include "DLPlayerInputCmd.h"
#include "IDLPlayerInputCmdSystem.h"
#include "IDLWeapon.h"
#include "TestAlgoLib.h"
#include "WeaponTypes.h"
#include "AbilityEventAction/DLAbilityEventBase.h"
#include "AnimNotify/DLAbilityAnimNtf.h"
#include "ASC/DLUnitAbilitySystemComponent.h"
#include "AttributeSet/DLCharacterAttributeSet.h"
#include "AttributeSet/DLUnitAttributeBaseSet.h"
#include "DLKit/Public/ConvertString.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Interface/ICharacterAttack.h"
#include "Interface/ICharacterWeapon.h"
#include "Interface/IDLVirtualCameraController.h"
#include "Interface/IDLAbilityContextDataGetter.h"
#include "Interface/IDLPrimaryPlayerAbilitySystem.h"
#include "RootMotionSource/RootMotionSourceHelper.h"
#include "TargetData/DLAbilityTargetData.h"


void UDLGameplayAbilityBase::GetSubAbilityClassArr(TArray<TSubclassOf<UDLGameplayAbilityBase>>& Out) const
{
	for (const auto& Item : SubAbilityMap)
	{
		Out.Add(Item.Value);
		if (Cast<UDLGameplayAbilityBase>(Item.Value.Get()))
		{
			Item.Value.GetDefaultObject()->GetSubAbilityClassArr(Out);
		}
	}
}

TArray<AActor*> UDLGameplayAbilityBase::CreateSubObjectWithDefName(const FGameplayTag DefName,
	UDLGameplayTargetSelectorCoordinate* CoordinateSelector, const int32 SpawnCount)
{
	return this->OnCreateSubObjectWithDefName(DefName, CoordinateSelector, SpawnCount);
}

bool UDLGameplayAbilityBase::K2_FindAbilityHitDefine(FGameplayTag DefName, FDLAbilityHitDefine& OutHitDefine)
{
	const auto Def = FindAbilityHitDefine(DefName);
	if (Def)
	{
		OutHitDefine = *Def;
		return true;
	}
	return false;
}

bool UDLGameplayAbilityBase::IsPrimaryAbility() const
{
	return AbilityTags.HasTag(FGameplayTag::RequestGameplayTag(EAbilityTagDef::EAbility::ECategory::EActivateType::Active));
}

EDLAbilityStage UDLGameplayAbilityBase::GetCurrentStage() const
{
	return CurrentStage;
}

TScriptInterface<IDLAbilityContextDataGetter> UDLGameplayAbilityBase::GetContextDataGetter() const
{
	return GetActorInfo().PlayerController.Get();
}

void UDLGameplayAbilityBase::ApplyRootMotion(UDataAsset_RootMotion* RootMotionData)
{
	if (RootMotionTask) return;
	if (!RootMotionData) return;
	RootMotionTask = URootMotionSourceHelper::ApplyRootMotionBP(this, NAME_None, RootMotionData);
	RootMotionTask->ReadyForActivation();

}

void UDLGameplayAbilityBase::EndRootMotion()
{
	if (!RootMotionTask) return;
	RootMotionTask->EndTask();
	RootMotionTask = nullptr;

}

const FDLAbilityActorInfo* UDLGameplayAbilityBase::GetAbilityActorInfo() const
{
	return static_cast<const FDLAbilityActorInfo*>(GetCurrentActorInfo());
}

bool UDLGameplayAbilityBase::AllowCostOverdraft(const FGameplayAttribute& CostAttribute) const
{
	return CostAttributeOverdraft.Contains(CostAttribute);
}


void UDLGameplayAbilityBase::ClientTryExecInputCmd(const EDLAbilityStage Stage)
{
	if (IsPredictingClient())
	{
		if (this->IsPrimaryAbility())
		{
			// 检测是否有输入缓存
			const auto InputCmdSys = UDLInputCmdSubsystem::Get(this);
			if (!InputCmdSys)
			{
				return;
			}

			// 先清空上一次的
			if (LastInputCmdEval)
			{
				InputCmdSys->RemoveCmdTags(LastInputCmdEval->AllowInputCmdTags, GetNameSafe(this));
			}

			const auto InputCmd = InputCmdSys->GetCurrentCacheCmd();
			InputCmdSys->ClearInputCmd();

			UE_VLOG_UELOG(GetActorInfo().PlayerController.Get(),
				LogDLAbility,
				Log,
				TEXT("<%s>(%s)ClientTryExecInputCmd LastState : %s  NowState : %s, Cmd %s"),
				TO_STR(this),
				NET_ROLE_STR(GetActorInfo().PlayerController.Get()),
				*ToString(CurrentStage),
				*ToString(Stage),
				InputCmd ? *InputCmd->ToString() : TEXT("None"));

			const auto Eval = StageInputCmdEvaluateArray.FindByKey(Stage);
			LastInputCmdEval = Eval;

			bool bIsHandler = false;

			if (Eval)
			{
				InputCmdSys->AddAllowCmdTags(Eval->AllowInputCmdTags, GetNameSafe(this));

				if (InputCmd)
				{
					InputCmdSys->ClearInputCmd();

					if (Eval->AllowInputCmdTags.HasAny(InputCmd->CmdTags))
					{
						InputCmd->SetContextObject(this);

						InputCmdSys->ExecCmd(
							InputCmd,
							FString::Printf(TEXT("Ability On Stage[%s] -> ExecCmd"), *ToString(Stage))
						);

						bIsHandler = true;
					}
				}
			}

			if (!bIsHandler)
			{
				UE_VLOG_UELOG(GetActorInfo().PlayerController.Get(),
					LogDLAbility,
					Log,
					TEXT("<%s>(%s)ClientTryExecInputCmd State : %s, TryCheckMovementCmd"),
					TO_STR(this),
					NET_ROLE_STR(GetActorInfo().PlayerController.Get()),
					*ToString(Stage)
				);

				InputCmdSys->TryCheckMovementCmd();
			}

		}
	}
}

FDLAbilityHitDefine* UDLGameplayAbilityBase::FindAbilityHitDefine(const FGameplayTag& DefName)
{
	return HitDefines.FindByKey(DefName);
}

FDLAbilityUnitSelectDefine* UDLGameplayAbilityBase::FindUnitSelectDefine(const FGameplayTag& DefName)
{
	return UnitSelectDefines.FindByKey(DefName);
}

const FDLAbilityVirtualCameraDefine* UDLGameplayAbilityBase::GetVirtualCameraDefMap(const FGameplayTag& DefId) const
{
	return VirtualCameraDefines.FindByKey(DefId);
}

const TArray<FDLAbilityUnitSelectDefine>& UDLGameplayAbilityBase::GetUnitSelectDefines() const
{
	return UnitSelectDefines;
}

const TArray<FDLAbilityHitDefine>& UDLGameplayAbilityBase::GetHitDefines() const
{
	return HitDefines;
}

const TArray<FDLSubObjectDef>& UDLGameplayAbilityBase::GetSubObjectDefines() const
{
	return SubObjectDefines;
}

const TArray<FDLAbilityVirtualCameraDefine>& UDLGameplayAbilityBase::GetVirtualCameraDefines() const
{
	return VirtualCameraDefines;
}

const TArray<UDLAbilityEventBase*>& UDLGameplayAbilityBase::GetEventActionArray() const
{
	return EventActionArray;
}

const FDLAbilityAnimationInfo& UDLGameplayAbilityBase::GetAnimationInfo() const
{
	return AnimationInfo;
}

FGameplayTagContainer UDLGameplayAbilityBase::GetHitDefineIds() const
{
	FGameplayTagContainer Ret;
	for (const auto& Def : GetHitDefines())
	{
		Ret.AddTag(Def.DefineId);
	}
	return Ret;
}

FGameplayTagContainer UDLGameplayAbilityBase::GetUnitSelectDefineIds() const
{
	FGameplayTagContainer Ret;
	for (const auto& Def : GetUnitSelectDefines())
	{
		Ret.AddTag(Def.DefineId);
	}
	return Ret;
}

EPrimaryInputID UDLGameplayAbilityBase::GetInputId() const
{
	return DebugInputId;
}


void UDLGameplayAbilityBase::PreActivate(const FGameplayAbilitySpecHandle Handle,
										 const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
										 FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	auto GetAllowInputCmdTags = [ActorInfo, this]()
	{
		if (ActorInfo->IsLocallyControlledPlayer())
		{
			return UDLInputCmdSubsystem::Get(this)->AllowExecCmdToString();
		}
		return FString{};
	};

	this->DebugGeneratedDebugActiveID();

	UE_VLOG_UELOG(ActorInfo->PlayerController.Get(), LogDLAbility, Log, TEXT("[======]<%s>(%s) Satrt Active "),
		TO_STR(this),
		NET_ROLE_STR(ActorInfo->PlayerController.Get())
	);

	UE_VLOG_UELOG(ActorInfo->PlayerController.Get(),
				LogDLAbility,
				Log,
				TEXT("<%s>(%s) Info:\n		InstancingPolicy: %s \n		AllowInputCmd: %s \n		DebugID : %s"),
				TO_STR(this),
				NET_ROLE_STR(ActorInfo->PlayerController.Get()),
				ENUM_TO_STR(EGameplayAbilityInstancingPolicy::Type, GetInstancingPolicy()),
				*GetAllowInputCmdTags(),
				*GetDebugActiveID()
	);

	UE_VLOG_UELOG(GetActorInfo().PlayerController.Get(),
		LogDLAbility,
		Log,
		TEXT("<%s>(%s) UDLGameplayAbilityBase::PreActivate Enter"),
		TO_STR(this),
		NET_ROLE_STR(ActorInfo->PlayerController.Get())
	);

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	CurrentStage = EDLAbilityStage::None;

	ensureAlwaysMsgf(!ActiveImmuneHitGEHandle.IsValid(), TEXT("应该不存在任何免疫效果"));

	if (GeneratedWeaponHitEventTags.IsEmpty())
	{
		if (const UAnimMontage* Montage = AnimationInfo.AnimMontage.LoadSynchronous())
		{
			for (const auto& Ntf : Montage->Notifies)
			{
				if (const UDLAnimNS_WeaponAttack* WeaponAttackNtf = Cast<UDLAnimNS_WeaponAttack>(Ntf.NotifyStateClass))
				{
					GeneratedWeaponHitEventTags.AddTag(WeaponAttackNtf->WeaponAttackInfo.EventTag);
				}
			}
		}
	}

	UE_VLOG_UELOG(GetActorInfo().PlayerController.Get(),
		LogDLAbility,
		Log,
		TEXT("<%s>(%s) UDLGameplayAbilityBase::PreActivate Leave"),
		TO_STR(this),
		NET_ROLE_STR(ActorInfo->PlayerController.Get())
	);
}

void UDLGameplayAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_VLOG_UELOG(GetActorInfo().PlayerController.Get(),
		LogDLAbility,
		Log,
		TEXT("<%s>(%s) UDLGameplayAbilityBase::EndAbility Enter"),
		TO_STR(this),
		NET_ROLE_STR(ActorInfo->PlayerController.Get())
	);

	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		WaitingEndNtfStates.Empty();

		if (AttackingWeaponInfo.IsSet())
		{
			this->EndAttack(AttackingWeaponInfo.GetValue());
		}

		if (ActorInfo->AbilitySystemComponent.IsValid())
		{
			const auto ASC = Cast<UDLUnitAbilitySystemComponent>(ActorInfo->AbilitySystemComponent);

			ASC->RemoveGameplayEventTagContainerDelegate(AbilitySpecialTags, AbilitySpecialHandle);
			AbilitySpecialHandle.Reset();

			// ~  UDLGameplayAbilityBase::TryActiveAbility  中绑定的代理，如果技能结束 需要保证清理

			ASC->OnAbilityEnded.Remove(ActiveAbilityEndDelegateHandle);
			ActiveAbilityEndDelegateHandle.Reset();

			ASC->AbilityActivatedCallbacks.Remove(ActiveAbilityDelegateHandle);
			ActiveAbilityDelegateHandle.Reset();

			//~

			ASC->ClearAbilityBeCancelAbilityTag(this);



			if (ActiveImmuneHitGEHandle.IsValid())
			{
				this->BP_RemoveGameplayEffectFromOwnerWithHandle(ActiveImmuneHitGEHandle);
				ActiveImmuneHitGEHandle.Invalidate();
				UE_LOG(LogDLAbility, Log, TEXT("Remove ImmuneHit GE In EndAbility  %s"), *GetNameSafe(this));
			}

			if (TenacityDynamicGEHandle.IsValid())
			{
				this->BP_RemoveGameplayEffectFromOwnerWithHandle(TenacityDynamicGEHandle);
				TenacityDynamicGEHandle.Invalidate();
				UE_LOG(LogDLAbility, Log, TEXT("Remove TenacityDynamic GE In OnCustomStateEvent  %s"), *GetNameSafe(this));
			}

			// 移除正在活跃的相机
			auto CopyActivatingCVDefIds = ActivatingVCDefIds;
			for (const auto& ID : CopyActivatingCVDefIds)
			{
				this->TriggerEventStopVC(ID);
			}

		}

		this->ClientTryExecInputCmd(EDLAbilityStage::None);

	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UE_VLOG_UELOG(GetActorInfo().PlayerController.Get(),
		LogDLAbility,
		Log,
		TEXT("<%s>(%s) UDLGameplayAbilityBase::EndAbility Leave"),
		TO_STR(this),
		NET_ROLE_STR(ActorInfo->PlayerController.Get())
	);

	UE_VLOG_UELOG(GetActorInfo().PlayerController.Get(), LogDLAbility, Log, TEXT("[======]<%s>(%s) End Active  IsCancel[%s]"),
					TO_STR(this),
					NET_ROLE_STR(ActorInfo->PlayerController.Get()),
					TO_STR(bWasCancelled)
	);
}


void UDLGameplayAbilityBase::BeginAttack(const FWeaponAttackNtfInfo& Parameter)
{
	UE_LOG(LogDLAbility, Log, TEXT("<%s>(%s)UDLGameplayAbilityBase::BeginAttack ===> [%s] [%s]"),
		TO_STR(this),
		NET_ROLE_STR(GetActorInfo().PlayerController.Get()),
		TO_STR(this),
		*Parameter.SlotMountWeapon.ToString());

	if (!IsLocallyControlled())
	{
		return;
	}

	if (!this->IsActive())
	{
		return;
	}


	const auto CharacterWeaponInterface = Cast<ICharacterWeapon>(GetActorInfo().AvatarActor.Get());
	if (!ensureAlwaysMsgf(CharacterWeaponInterface, TEXT("应该能取到获取武器的接口")))
	{
		return;
	}

	const FDLAbilityUnitSelectDefine* Def = UnitSelectDefines.FindByKey(Parameter.UnitSelectDefineId);
	ensureAlwaysMsgf(Def, TEXT("Not Find UnitSelectDef  ID : %s"), *Parameter.UnitSelectDefineId.ToString());
	if (!Def)
	{
		return;
	}

	if (!ensureAlwaysMsgf(Def->UnitSelectType == EDLAbilityUnitSelectType::Weapon, TEXT("UnitSelectType 必定是 Weapon 才行")))
	{
		return;
	}

	AttackingWeaponInfo = Parameter;

	auto WeaponArray = CharacterWeaponInterface->GetCurrentWeapons();
	for (const auto& WeaponActor : WeaponArray)
	{
		const auto Weapon = Cast<IDLWeapon>(WeaponActor);
		if (Weapon && Parameter.SlotMountWeapon.HasTag(Weapon->GetCurrentBoneSocket()))
		{

			FWeaponAttackContext WeaponAttackContext;
			WeaponAttackContext.AttackTags.AddTag(Parameter.AttackDirectionTag);
			WeaponAttackContext.OverrideWeaponSelector = Def->WeaponContext.OverrideWeaponSelector;
			WeaponAttackContext.SelectorFilterPrototype = Def->WeaponContext.UnitSelectorFilter;

			Weapon->SetAttackContext(WeaponAttackContext);

			UE_LOG(LogDLAbility, Log, TEXT("SetWeaponAttackContext [%s] [%s]"), *Weapon->ToString(), TO_STR(this));

			break;
		}
	}

	FWeaponAttackParameter AttackParameter;
	AttackParameter.EventTag = Parameter.EventTag;
	AttackParameter.SlotMountWeapon = Parameter.SlotMountWeapon;

	INotifyCharacterWeapon::Execute_BeginAttack(GetActorInfo().AvatarActor.Get(), AttackParameter);

	UE_LOG(LogDLAbility, Log, TEXT("<%s>(%s)UDLGameplayAbilityBase::BeginAttack <=== [%s] [%s]"),
		TO_STR(this),
		NET_ROLE_STR(GetActorInfo().PlayerController.Get()),
		TO_STR(this),
		*Parameter.SlotMountWeapon.ToString());
}

void UDLGameplayAbilityBase::EndAttack(const FWeaponAttackNtfInfo& Parameter)
{
	UE_LOG(LogDLAbility, Log, TEXT("<%s>(%s)UDLGameplayAbilityBase::EndAttack ==> [%s] [%s]"),
		TO_STR(this),
		NET_ROLE_STR(GetActorInfo().PlayerController.Get()),
		TO_STR(this),
		*Parameter.SlotMountWeapon.ToString());

	if (!IsLocallyControlled())
	{
		return;
	}

	if (!this->IsActive())
	{
		return;
	}

	const auto CharacterWeaponInterface = Cast<ICharacterWeapon>(GetActorInfo().AvatarActor.Get());
	if (!ensureAlwaysMsgf(CharacterWeaponInterface, TEXT("应该能取到获取武器的接口")))
	{
		return;
	}

	FWeaponAttackParameter AttackParameter;
	AttackParameter.EventTag = Parameter.EventTag;
	AttackParameter.SlotMountWeapon = Parameter.SlotMountWeapon;

	INotifyCharacterWeapon::Execute_EndAttack(GetActorInfo().AvatarActor.Get(), AttackParameter);


	auto WeaponArray = CharacterWeaponInterface->GetCurrentWeapons();
	for (const auto& Weapon : WeaponArray)
	{
		const auto WeaponInterface = Cast<IDLWeapon>(Weapon);
		if (WeaponInterface && Parameter.SlotMountWeapon.HasTag(WeaponInterface->GetCurrentBoneSocket()))
		{
			UE_LOG(LogDLAbility, Log, TEXT("ClearWeaponAttackContext [%s] [%s]"), *WeaponInterface->ToString(), TO_STR(this));
			WeaponInterface->ClearAttackContext();
		}
	}

	AttackingWeaponInfo.Reset();

	UE_LOG(LogDLAbility, Log, TEXT("<%s>(%s)UDLGameplayAbilityBase::EndAttack <== [%s] [%s]"),
		TO_STR(this),
		NET_ROLE_STR(GetActorInfo().PlayerController.Get()),
		TO_STR(this),
		*Parameter.SlotMountWeapon.ToString());

}


bool UDLGameplayAbilityBase::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{

	// Check 必须满足的 GE 层数
	if (RequireGEStack.GEClass)
	{
		bool Ok = true;
		const int32 CurrentCount = GetGEStackCount(*ActorInfo, RequireGEStack.GEClass);


		if (RequireGEStack.MaxStackCount != FAbilityRequireGEStack::InvalidStackCount)
		{
			if (CurrentCount > RequireGEStack.MaxStackCount)
			{
				Ok = false;
			}
		}

		if (Ok && RequireGEStack.MinStackCount != FAbilityRequireGEStack::InvalidStackCount)
		{
			if (CurrentCount < RequireGEStack.MinStackCount)
			{
				Ok = false;
			}
		}

		if (!Ok)
		{
			const FGameplayTag& GEStackCountTag = UDLAbilitySystemGlobal::Get().ActivateFailGEStackCountTag;

			if (OptionalRelevantTags && GEStackCountTag.IsValid())
			{
				OptionalRelevantTags->AddTag(GEStackCountTag);
			}

			return false;
		}
	}

	const auto ASC = Cast<UDLUnitAbilitySystemComponent>(ActorInfo->AbilitySystemComponent);
	if (ASC)
	{
		if (ASC->TestAbilityBlock(this))
		{
			UE_LOG(LogDLAbility, Log, TEXT("<%s>(%s) Be Blocked"),
				TO_STR(this),
				NET_ROLE_STR(ActorInfo->PlayerController.Get()), TO_STR(this));
			return false;
		}
	}


	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

const FGameplayTagContainer* UDLGameplayAbilityBase::GetCooldownTags() const
{
	return &CooldownTags;
}


void UDLGameplayAbilityBase::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CDGE = GetCooldownGameplayEffect();
	if (!CDGE)
	{
		return;
	}

	const int32 AbilityLevel = GetAbilityLevel();

	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CDGE->GetClass(), AbilityLevel);
	SpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags);

	// 玩家的冷却系数
	bool Succeed = false;
	float Coefficient = GetAttributeValue(*ActorInfo, UDLUnitAttributeBaseSet::GetAbilityCDCoefficientAttribute(), Succeed);
	if (!ensureAlwaysMsgf(Succeed, TEXT("不应该出问题，CD 系数一定能获取到")))
	{
		Coefficient = 1.f;
	}

	// 冷却计算公式
	const float CDDuration = CooldownDuration.GetValueAtLevel(AbilityLevel) * Coefficient;

	// 应用冷却
	SpecHandle.Data->SetSetByCallerMagnitude(
				FGameplayTag::RequestGameplayTag(EAbilityTagDef::EByCallerData::CooldownInterval),
				CDDuration
	);

	// ReSharper disable once CppExpressionWithoutSideEffects
	this->ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);

	// 如果是本地玩家，需要推送本地玩家 CD 事件
	if (ActorInfo->IsLocallyControlledPlayer() && ActorInfo->OwnerActor.IsValid())
	{
		auto& MsgSys = UGameplayMessageSubsystem::Get(ActorInfo->OwnerActor.Get());
		FDLLocalPlayerAbilityApplyCDMsg Msg;
		Msg.Duration = CDDuration;
		Msg.OwnerAbilityId = AbilityId;
		MsgSys.BroadcastMessage(EDLGameplayMsgType::LocalPlayer_Gameplay_AbilityCD, Msg);
	}
}

UGameplayEffect* UDLGameplayAbilityBase::GetCostGameplayEffect() const
{
	// TODO 本来计划用 动态 构建 CostGE 的路子，但是 在 ApplyGE 底层 是用的 CDO，所以这个路子走不通，后边在尝试简化配置方式
	//if (!InstanceGE)
	//{
	//	UDLGameplayAbilityBase* MutThis = const_cast<UDLGameplayAbilityBase*>(this);
	//
	//	MutThis->InstanceGE = NewObject<UGameplayEffect>(MutThis, TEXT("InstanceGE"));
	//
	//	for (const auto& Def : CostDefines)
	//	{
	//		if (Def.Attribute == UDLPlayerAttributeSet::GetStaminaAttribute())
	//		{
	//			InstanceGE->RemoveGameplayEffectsWithTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.GE.Category.RecoveryStamina")));
	//		}
	//
	//		auto& Ref = InstanceGE->Modifiers.AddDefaulted_GetRef();
	//		Ref.Attribute = Def.Attribute;
	//		Ref.ModifierMagnitude = Def.ModifierMagnitude;
	//		Ref.ModifierOp = EGameplayModOp::Additive;
	//	}
	//}
	//return InstanceGE;

	return Super::GetCostGameplayEffect();
}


bool UDLGameplayAbilityBase::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	const UGameplayEffect* CostGE = GetCostGameplayEffect();

	if (!CostGE)
	{
		return true;
	}

	FGameplayEffectSpec	Spec(CostGE, MakeEffectContext(Handle, ActorInfo), GetAbilityLevel(Handle, ActorInfo));
	Spec.CalculateModifierMagnitudes();

	bool IsOk = true;

	for (int32 ModIdx = 0; ModIdx < Spec.Modifiers.Num(); ++ModIdx)
	{
		const FGameplayModifierInfo& ModDef = Spec.Def->Modifiers[ModIdx];
		const FModifierSpec& ModSpec = Spec.Modifiers[ModIdx];

		if (ModDef.ModifierOp == EGameplayModOp::Additive)
		{
			if (!ModDef.Attribute.IsValid())
			{
				continue;
			}

			const auto ASC = ActorInfo->AbilitySystemComponent;
			if (!ASC->HasAttributeSetForAttribute(ModDef.Attribute))
			{
				UE_LOG(LogDLAbility, Error, TEXT("%s CheckCost 无效的属性值  %s"), TO_STR(this), *ModDef.Attribute.AttributeName);
				continue;
			}

			const float CurrentValue = ASC->GetNumericAttributeChecked(ModDef.Attribute);
			const float CostValue = ModSpec.GetEvaluatedMagnitude();

			if (CurrentValue + CostValue < 0.f)
			{
				if (!(this->AllowCostOverdraft(ModDef.Attribute) && CurrentValue > 0.f))
				{
					IsOk = false;
					break;
				}
			}
		}
	}

	if (!IsOk)
	{
		const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;

		if (OptionalRelevantTags && CostTag.IsValid())
		{
			OptionalRelevantTags->AddTag(CostTag);
		}
		return false;
	}

	return true;
}

void UDLGameplayAbilityBase::OnCustomStateEvent_Implementation(bool IsBeginState, const FGameplayTag& Tag)
{
	if (IsValid(this) && this->IsActive() && !this->bIsAbilityEnding)
	{
		if (Tag == ImmuneHitEventTag)
		{
			if (IsBeginState)
			{
				if (!ensureAlwaysMsgf(!ActiveImmuneHitGEHandle.IsValid(), TEXT("一定是无效的才对")))
				{
					this->BP_RemoveGameplayEffectFromOwnerWithHandle(ActiveImmuneHitGEHandle);
					ActiveImmuneHitGEHandle.Invalidate();
					UE_LOG(LogDLAbility, Log, TEXT("Remove ImmuneHit GE In OnCustomStateEvent  %s"), *GetNameSafe(this));
				}

				ActiveImmuneHitGEHandle = this->BP_ApplyGameplayEffectToOwner(ImmuneHitGEClass);
				UE_LOG(LogDLAbility, Log, TEXT("Apply ImmuneHit GE In OnCustomStateEvent %s"), *GetNameSafe(this));
			}
			else
			{
				if (ActiveImmuneHitGEHandle.IsValid())
				{
					this->BP_RemoveGameplayEffectFromOwnerWithHandle(ActiveImmuneHitGEHandle);
					ActiveImmuneHitGEHandle.Invalidate();
					UE_LOG(LogDLAbility, Log, TEXT("Remove ImmuneHit GE In OnCustomStateEvent  %s"), *GetNameSafe(this));
				}
			}
		}
		else if (Tag == TenacityDynamicEventTag)
		{
			if (IsBeginState)
			{
				if (!ensureAlwaysMsgf(!TenacityDynamicGEHandle.IsValid(), TEXT("一定是无效的才对")))
				{
					this->BP_RemoveGameplayEffectFromOwnerWithHandle(TenacityDynamicGEHandle);
					TenacityDynamicGEHandle.Invalidate();
					UE_LOG(LogDLAbility, Log, TEXT("Remove TenacityDynamic GE In OnCustomStateEvent  %s"), *GetNameSafe(this));
				}

				const auto GEHandle = this->MakeOutgoingGameplayEffectSpec(TenacityDynamicGEClass);
				if (ensureAlwaysMsgf(GEHandle.IsValid(), TEXT("不可能无效")))
				{
					const auto CurrentTenacityValue = GetAbilitySystemComponentFromActorInfo_Ensured()->GetNumericAttributeChecked(UDLUnitAttributeBaseSet::GetTenacityAttribute());
					const auto TenacityDy = TenacityDynamicCoefficient * CurrentTenacityValue + TenacityDynamicBase;
					GEHandle.Data->SetSetByCallerMagnitude(TenacityDynamicSetByCallerTag, TenacityDy);

					TenacityDynamicGEHandle = this->K2_ApplyGameplayEffectSpecToOwner(GEHandle);
					UE_LOG(LogDLAbility, Log, TEXT("Apply TenacityDynamic GE In OnCustomStateEvent %s DynamicTenacity Value %f"), *GetNameSafe(this), TenacityDy);
				}
			}
			else
			{
				if (TenacityDynamicGEHandle.IsValid())
				{
					this->BP_RemoveGameplayEffectFromOwnerWithHandle(TenacityDynamicGEHandle);
					TenacityDynamicGEHandle.Invalidate();
					UE_LOG(LogDLAbility, Log, TEXT("Remove TenacityDynamic GE In OnCustomStateEvent  %s"), *GetNameSafe(this));
				}
			}
		}
	}
}


UDLGameplayAbilityBase::UDLGameplayAbilityBase()
{
	AbilitySpecialTags.AddTag(FGameplayTag::RequestGameplayTag(EAbilityTagDef::ESpecial::CommitAbility));
}


EPrimaryInputID UDLGameplayAbilityBase::GetDebugInputID() const
{
	return DebugInputId;
}

void UDLGameplayAbilityBase::SetActiveInputKey(const EPrimaryInputID InputID)
{
	ActiveInputKey = InputID;
}

ENetRole UDLGameplayAbilityBase::GetOwnerNetRole() const
{
	if (GetActorInfo().AvatarActor.IsValid())
	{
		return GetActorInfo().AvatarActor->GetLocalRole();
	}
	return ENetRole::ROLE_None;
}

FString UDLGameplayAbilityBase::GetDebugActiveID() const
{
#if WITH_EDITOR
	return ActiveID;
#endif
	return "";
}

void UDLGameplayAbilityBase::DebugGeneratedDebugActiveID()
{
#if WITH_EDITOR
	ActiveID = FGuid::NewGuid().ToString(EGuidFormats::Short);
#endif
}

FGameplayAbilityTargetDataHandle UDLGameplayAbilityBase::MakeTargetDataByActor(AActor* Actor)
{
	FGameplayAbilityTargetDataHandle Ret;
	FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
	NewData->TargetActorArray.Add(Actor);
	Ret.Add(NewData);
	return Ret;
}

void UDLGameplayAbilityBase::SendTargetDataToServer(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
													const FGameplayTag CustomTag, const FAbilitySimpleDynamicDelegate ExecWithPrediction)
{
	UAbilitySystemComponent* ASC = this->GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		const bool IsValidForMorePrediction = ASC->ScopedPredictionKey.IsValidForMorePrediction();

		UE_LOG(LogDLAbility, Log, TEXT("<%s>(%s)IsValidForMorePrediction %d"),
			TO_STR(this),
			NET_ROLE_STR(GetActorInfo().PlayerController.Get()),
			IsValidForMorePrediction);

		FScopedPredictionWindow	ScopedPrediction(ASC, !IsValidForMorePrediction);

		ASC->CallServerSetReplicatedTargetData(
			GetCurrentAbilitySpecHandle(),
			GetCurrentActivationInfo().GetActivationPredictionKey(),
			TargetDataHandle,
			CustomTag,
			ASC->ScopedPredictionKey
		);

		bool IsExec = ExecWithPrediction.ExecuteIfBound();
	}
}



void UDLGameplayAbilityBase::GetTargetActorByTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
														TArray<AActor*>& OutActor)
{
	OutActor.Empty();

	for (auto& TargetData : TargetDataHandle.Data)
	{
		TArray<TWeakObjectPtr<AActor>> Out = TargetData->GetActors();
		for (auto& Actor : Out)
		{
			if (Actor.IsValid())
			{
				OutActor.Add(Actor.Get());
			}
		}
	}
}

bool UDLGameplayAbilityBase::FindSubObjectDefByName(const FGameplayTag DefName, FDLSubObjectDef& OutSubObjectDef) const
{
	const FDLSubObjectDef* Value = SubObjectDefines.FindByKey(DefName);
	if (Value)
	{
		OutSubObjectDef = *Value;
		return true;
	}
	return false;
}


void UDLGameplayAbilityBase::AttackActor(
	AActor* InTrigger,
	FGameplayAbilityTargetDataHandle TargetDataHandle,
	UCalculateAbilityAttackInfo* UnderAttackCalculate)
{
	if (!GetActorInfo().AvatarActor.IsValid())
	{
		return;
	}

	if (!UnderAttackCalculate)
	{
		return;
	}

	const auto Instigate = GetActorInfo().OwnerActor.Get();
	const auto  EffectCauser = GetActorInfo().AvatarActor.Get();
	const auto Trigger = InTrigger ? InTrigger : EffectCauser;

	FGameplayTagContainer AttackTags;

	TArray<FDLCalculateAttackInfoTargetInfo> Targets;
	for (const auto& Data : TargetDataHandle.Data)
	{
		for (auto Actor : Data->GetActors())
		{
			if (Actor.IsValid())
			{
				auto& Info = Targets.AddDefaulted_GetRef();
				Info.Target = Actor.Get();

				const auto MyTargetData = StructCast<FGameplayAbilityTargetData_AttackHitResult>(Data.Get());
				if (MyTargetData)
				{
					Info.Tags = MyTargetData->AttackTags;
					Info.HitResult = MyTargetData->HitResult;
				}
			}
		}
	}

	const auto OwnerInterface = Cast<ICharacterAttack>(EffectCauser);
	if (OwnerInterface)
	{
		TArray<FCharacterAttackArg> AttackArgArray;
		UnderAttackCalculate->Exec(this, Targets, Instigate, EffectCauser, Trigger, AttackArgArray);
		OwnerInterface->AttackActors(AttackArgArray);
	}
}

bool UDLGameplayAbilityBase::TryActiveAbility(
					FGameplayAbilitySpecHandle AbilityHandle,
					const bool bAllowRemoteActivation,
					const FAbilityComplateDynamicDelegate& Complate)
{
	const auto ASC = GetAbilitySystemComponentFromActorInfo_Checked();
	ensureAlwaysMsgf(AbilityHandle.IsValid(), TEXT("一个无效 GameplayAbilitySpecHandle Handle"));
	ensureAlwaysMsgf(!ActiveAbilityEndDelegateHandle.IsValid(), TEXT("目前一个技能只能同时激活另外一个技能"));
	ensureAlwaysMsgf(!ActiveAbilityDelegateHandle.IsValid(), TEXT("目前一个技能只能同时激活另外一个技能"));

	if (ActiveAbilityEndDelegateHandle.IsValid())
	{
		ASC->OnAbilityEnded.Remove(ActiveAbilityEndDelegateHandle);
	}

	if (ActiveAbilityDelegateHandle.IsValid())
	{
		ASC->AbilityActivatedCallbacks.Remove(ActiveAbilityDelegateHandle);
	}


	// 技能开始激活时 Callback 
	ActiveAbilityDelegateHandle
		= ASC->AbilityActivatedCallbacks.AddWeakLambda(
		this, [this, AbilityHandle](UGameplayAbility* Ability)
		{
			if (AbilityHandle != Ability->GetCurrentAbilitySpecHandle())
			{
				return;
			}

			UDLGameplayAbilityBase* DLAbilityBase = Cast<UDLGameplayAbilityBase>(Ability);
			if (!DLAbilityBase)
			{
				return;
			}

			// Hook Some Action 
		});

	// 技能释放完成
	ActiveAbilityEndDelegateHandle
		= ASC->OnAbilityEnded.AddWeakLambda(
		this, [this, Complate, AbilityHandle](const FAbilityEndedData& Data)
		{
			if (Data.AbilitySpecHandle != AbilityHandle)
			{
				return;
			}

			UDLGameplayAbilityBase* DLAbilityBase = Cast<UDLGameplayAbilityBase>(Data.AbilityThatEnded);
			if (!DLAbilityBase)
			{
				return;
			}

			const auto ASC = GetAbilitySystemComponentFromActorInfo_Checked();
			ASC->OnAbilityEnded.Remove(ActiveAbilityEndDelegateHandle);
			ActiveAbilityEndDelegateHandle.Reset();

			ASC->AbilityActivatedCallbacks.Remove(ActiveAbilityDelegateHandle);
			ActiveAbilityDelegateHandle.Reset();

			// ReSharper disable once CppExpressionWithoutSideEffects
			Complate.ExecuteIfBound(Data.bWasCancelled);

		});


	// 尝试激活技能
	if (!IsLocallyControlled())
	{
		return true;
	}

	const bool IsOK = ASC->TryActivateAbility(AbilityHandle, bAllowRemoteActivation);
	if (!IsOK)
	{
		ASC->OnAbilityEnded.Remove(ActiveAbilityEndDelegateHandle);
		ActiveAbilityEndDelegateHandle.Reset();

		ASC->AbilityActivatedCallbacks.Remove(ActiveAbilityDelegateHandle);
		ActiveAbilityDelegateHandle.Reset();
	}

	return IsOK;
}

bool UDLGameplayAbilityBase::CanBeExecuted(const bool AllowPlayerAutonomous, const bool AllowAIAutonomous) const
{
	const auto Actor = GetAvatarActorFromActorInfo();
	if (Actor)
	{
		if (AllowPlayerAutonomous)
		{
			if (Actor->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
			{
				return true;
			}
		}

		if (AllowAIAutonomous)
		{
			if (Actor->GetLocalRole() == ENetRole::ROLE_Authority && Actor->GetRemoteRole() == ENetRole::ROLE_SimulatedProxy)
			{
				return true;
			}
		}
	}
	return false;
}

FGameplayAbilitySpecHandle UDLGameplayAbilityBase::GetSubAbilitySpecHandleByName(FName Name)
{
	if (SubAbilityMap.Contains(Name))
	{
		const auto ASC = GetAbilitySystemComponentFromActorInfo_Checked();
		const auto Spec = ASC->FindAbilitySpecFromClass(SubAbilityMap[Name]);
		if (Spec)
		{
			return Spec->Handle;
		}
	}
	return {};
}

const FGameplayTagContainer& UDLGameplayAbilityBase::GetAbilityTag() const
{
	return AbilityTags;
}

int32 UDLGameplayAbilityBase::GetGEStackCount(const FGameplayAbilityActorInfo& ActorInfo, const TSubclassOf<UGameplayEffect> GEClass)
{
	UDLUnitAbilitySystemComponent* ASC = Cast<UDLUnitAbilitySystemComponent>(ActorInfo.AbilitySystemComponent);
	if (ASC)
	{
		const auto Handle = ASC->FindActiveGEHandle(GEClass);
		if (Handle.IsValid())
		{
			return ASC->GetCurrentStackCount(Handle);
		}
	}

	return 0;
}

float UDLGameplayAbilityBase::GetAttributeValue(const FGameplayAbilityActorInfo& ActorInfo,
												const FGameplayAttribute Attribute, bool& bSuccessfullyFoundAttribute)
{
	if (ActorInfo.AbilitySystemComponent.IsValid())
	{
		return UAbilitySystemBlueprintLibrary::GetFloatAttributeFromAbilitySystemComponent(ActorInfo.AbilitySystemComponent.Get(), Attribute, bSuccessfullyFoundAttribute);
	}
	bSuccessfullyFoundAttribute = false;
	return 0.f;
}


void UDLGameplayAbilityBase::TriggerEventStopVC(FGameplayTag VCDefId)
{
	const auto Def = GetVirtualCameraDefMap(VCDefId);
	if (!ensureAlwaysMsgf(Def, TEXT("无法找到虚拟相机的定义，%s"), *VCDefId.ToString()))
	{
		return;
	}

	const auto VCController = Cast<IDLVirtualCameraController>(GetActorInfo().AvatarActor);
	if (VCController)
	{
		VCController->StopVirtualCamera(Def->VirtualCameraChannelTag);
	}

	ActivatingVCDefIds.Remove(VCDefId);
}

void UDLGameplayAbilityBase::TriggerEventExecInputCmd()
{
	this->ClientTryExecInputCmd(CurrentStage);
}

void UDLGameplayAbilityBase::TriggerCustomStateEvent(bool IsBeginState, const FGameplayTag& Tag, bool NeedClientPrediction)
{
	auto DoAction = [Tag, this, IsBeginState] {
		this->OnCustomStateEvent(IsBeginState, Tag);
		const UDLUnitAbilitySystemComponent* ASC = Cast<UDLUnitAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo_Checked());
		ASC->OnNtfStateChange.Broadcast(this, IsBeginState, Tag);
	};

	if (NeedClientPrediction)
	{
		const auto AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();

		FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent, IsPredictingClient());

		if (IsPredictingClient())
		{
			UE_LOG(LogDLAbility, Log, TEXT("TriggerCustomStateEvent -> ServerSetReplicatedEvent SpecHandle:%s Key:%s"),
				*GetCurrentAbilitySpecHandle().ToString(),
				*GetCurrentActivationInfo().GetActivationPredictionKey().ToString());

			AbilitySystemComponent->ServerSetReplicatedEvent(
				EAbilityGenericReplicatedEvent::GameCustom3,
				GetCurrentAbilitySpecHandle(),
				GetCurrentActivationInfo().GetActivationPredictionKey(),
				AbilitySystemComponent->ScopedPredictionKey);

			DoAction();
		}
		else
		{
			UE_LOG(LogDLAbility, Log, TEXT("TriggerCustomStateEvent -> CallOrAddReplicatedDelegate SpecHandle:%s Key:%s"),
				*GetCurrentAbilitySpecHandle().ToString(),
				*GetCurrentActivationInfo().GetActivationPredictionKey().ToString());

			AbilitySystemComponent->CallOrAddReplicatedDelegate(
				EAbilityGenericReplicatedEvent::GameCustom3,
				GetCurrentAbilitySpecHandle(),
				GetCurrentActivationInfo().GetActivationPredictionKey(),
				FSimpleMulticastDelegate::FDelegate::CreateWeakLambda(this, [this, DoAction]
					{
						const auto AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();

						AbilitySystemComponent->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom3,
								GetCurrentAbilitySpecHandle(),
								GetCurrentActivationInfo().GetActivationPredictionKey());

						UE_LOG(LogDLAbility, Log, TEXT("TriggerCustomStateEvent -> ConsumeGenericReplicatedEvent SpecHandle:%s Key:%s"),
							*GetCurrentAbilitySpecHandle().ToString(),
							*GetCurrentActivationInfo().GetActivationPredictionKey().ToString());

						DoAction();

						// TODO 这种修复办法非常冒险，可能直接误伤 同时触发的事件，比较好的做法时使用 不同的 EAbilityGenericReplicatedEvent
						// 绑定的回调需要解绑，否则会被重复调用
						AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom3,
							GetCurrentAbilitySpecHandle(),
							GetCurrentActivationInfo().GetActivationPredictionKey()).Clear();
					})
			);
		}
	}
	else
	{
		DoAction();
	}
}

void UDLGameplayAbilityBase::AddWittingEndNtfState(UObject* AnimNtfState)
{
	ensureAlwaysMsgf(!WaitingEndNtfStates.Contains(AnimNtfState), TEXT("这种事件理论上一定不会被重复添加"));
	WaitingEndNtfStates.Add(AnimNtfState);
}

void UDLGameplayAbilityBase::RemoveWittingEndNtfState(UObject* AnimNtfState)
{
	WaitingEndNtfStates.Remove(AnimNtfState);
}

bool UDLGameplayAbilityBase::CanTriggerEventByAnimNtf(UAnimationAsset* Anim, UAnimNotifyState* EndAnimNtfStateObj)
{
	if (GetCurrentMontage() != Anim)
	{
		return false;
	}

	if (EndAnimNtfStateObj)
	{
		if (!WaitingEndNtfStates.Contains(EndAnimNtfStateObj))
		{
			return false;
		}
	}

	return true;
}

TArray<UDLAbilityEventBase*> UDLGameplayAbilityBase::FindAbilityEvent(const TSubclassOf<UDLAbilityEventBase> EventClass) const
{
	TArray<UDLAbilityEventBase*> Ret;
	for (auto Event : EventActionArray)
	{
		if (Event && Event->GetClass() == EventClass)
		{
			Ret.Add(Event);
		}
	}
	return Ret;
}


void UDLGameplayAbilityBase::TriggerEventHitUnit(const FGameplayTag& HitEventTag, const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	auto EventArr = FindAbilityEvent(UDLAbilityEventWeaponHitActor::StaticClass());

	FDLAbilityActionContext Context;
	Context.Ability = this;

	for (const auto Event : EventArr)
	{
		UDLAbilityEventWeaponHitActor* E = Cast<UDLAbilityEventWeaponHitActor>(Event);
		if (E && E->HitEventTags.HasAny(FGameplayTagContainer{ HitEventTag }))
		{
			E->TriggerAction(Context, TargetDataHandle);
		}
	}
}


void UDLGameplayAbilityBase::TriggerEventSimple(const FGameplayTag EventTag, bool NeedClientPrediction)
{
	auto DoAction = [EventTag, this] {

		auto EventArr = FindAbilityEvent(UDLAbilitySimpleEvent::StaticClass());

		FScopedPredictionWindow PredictionWindow(GetAbilitySystemComponentFromActorInfo(), true);

		FDLAbilityActionContext Context;
		Context.Ability = this;

		for (const auto Event : EventArr)
		{
			UDLAbilitySimpleEvent* E = Cast<UDLAbilitySimpleEvent>(Event);
			if (E && E->EventTags.HasAny(FGameplayTagContainer{ EventTag }))
			{
				E->TriggerAction(Context);
			}
		}

		this->OnSimpleEvent(EventTag);

		const UDLUnitAbilitySystemComponent* ASC = Cast<UDLUnitAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo_Checked());
		ASC->OnNtfChange.Broadcast(this, EventTag);
	};


	if (NeedClientPrediction)
	{
		const auto AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();

		FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent, IsPredictingClient());

		if (IsPredictingClient())
		{
			AbilitySystemComponent->ServerSetReplicatedEvent(
				EAbilityGenericReplicatedEvent::GameCustom1,
				GetCurrentAbilitySpecHandle(),
				GetCurrentActivationInfo().GetActivationPredictionKey(),
				AbilitySystemComponent->ScopedPredictionKey);

			DoAction();
		}
		else
		{
			AbilitySystemComponent->CallOrAddReplicatedDelegate(
				EAbilityGenericReplicatedEvent::GameCustom1,
				GetCurrentAbilitySpecHandle(),
				GetCurrentActivationInfo().GetActivationPredictionKey(),
				FSimpleMulticastDelegate::FDelegate::CreateWeakLambda(this, [this, DoAction]
					{
						GetAbilitySystemComponentFromActorInfo_Checked()
							->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1,
							GetCurrentAbilitySpecHandle(),
							GetCurrentActivationInfo().GetActivationPredictionKey());

						DoAction();
					})
			);
		}
	}
	else
	{
		DoAction();
	}
}



void UDLGameplayAbilityBase::TriggerEventAbilityStageChange(const EDLAbilityStage NewStage, bool NeedClientPrediction)
{
	if (CurrentStage == NewStage)
	{
		return;
	}

	EDLAbilityStageChangeType EnterChangeType = EDLAbilityStageChangeType::None;

	if (NewStage == EDLAbilityStage::Pre)
	{
		EnterChangeType = EDLAbilityStageChangeType::EnterPre;
	}
	else if (NewStage == EDLAbilityStage::Spell)
	{
		EnterChangeType = EDLAbilityStageChangeType::EnterSpell;
	}
	else if (NewStage == EDLAbilityStage::Post)
	{
		EnterChangeType = EDLAbilityStageChangeType::EnterPost;
	}
	else if (NewStage == EDLAbilityStage::Channel)
	{
		EnterChangeType = EDLAbilityStageChangeType::EnterChannel;
	}


	EDLAbilityStageChangeType EndChangeType = EDLAbilityStageChangeType::None;

	if (CurrentStage == EDLAbilityStage::Pre)
	{
		EndChangeType = EDLAbilityStageChangeType::EndPre;
	}
	else if (CurrentStage == EDLAbilityStage::Spell)
	{
		EndChangeType = EDLAbilityStageChangeType::EndSpell;
	}
	else if (CurrentStage == EDLAbilityStage::Post)
	{
		EndChangeType = EDLAbilityStageChangeType::EndPost;
	}
	else if (CurrentStage == EDLAbilityStage::Channel)
	{
		EndChangeType = EDLAbilityStageChangeType::EndChannel;
	}


	auto DoAction = [EnterChangeType, EndChangeType, this, NewStage]
	{
		UE_VLOG_UELOG(this->GetActorInfo().PlayerController.Get(),
			LogDLAbility,
			Log,
			TEXT("<%s>(%s)TriggerEventAbilityStageChange CurrentState %s NowState %s"),
			TO_STR(this),
			NET_ROLE_STR(GetActorInfo().PlayerController.Get()),
			*ToString(CurrentStage), *ToString(NewStage)
		);

		this->OnAbilityStageChange(NewStage);

		// Spell Action

		auto EventArr = FindAbilityEvent(UDLAbilityStageChangeEvent::StaticClass());

		FDLAbilityActionContext Context;
		Context.Ability = this;

		for (const auto Event : EventArr)
		{
			UDLAbilityStageChangeEvent* E = Cast<UDLAbilityStageChangeEvent>(Event);
			if (E && (E->EventType == EnterChangeType || E->EventType == EndChangeType))
			{
				E->TriggerAction(Context);
			}
		}

		this->ClientTryExecInputCmd(NewStage);


		UE_VLOG_UELOG(this->GetActorInfo().PlayerController.Get(),
			LogDLAbility,
			Log,
			TEXT("<%s>(%s)TriggerEventAbilityStageChange Updata State  CurrentState %s NowState %s"),
			TO_STR(this),
			NET_ROLE_STR(GetActorInfo().PlayerController.Get()),
			*ToString(CurrentStage),
			*ToString(NewStage)
		);

		CurrentStage = NewStage;
	};

	if (NeedClientPrediction)
	{
		const auto AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();

		FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent, IsPredictingClient());

		if (IsPredictingClient())
		{
			AbilitySystemComponent->ServerSetReplicatedEvent(
				EAbilityGenericReplicatedEvent::GameCustom2,
				GetCurrentAbilitySpecHandle(),
				GetCurrentActivationInfo().GetActivationPredictionKey(),
				AbilitySystemComponent->ScopedPredictionKey);

			DoAction();
		}
		else
		{
			AbilitySystemComponent->CallOrAddReplicatedDelegate(
				EAbilityGenericReplicatedEvent::GameCustom2,
				GetCurrentAbilitySpecHandle(),
				GetCurrentActivationInfo().GetActivationPredictionKey(),
				FSimpleMulticastDelegate::FDelegate::CreateWeakLambda(this, [this, DoAction]()
					{
						GetAbilitySystemComponentFromActorInfo_Checked()
							->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom2,
							GetCurrentAbilitySpecHandle(),
							GetCurrentActivationInfo().GetActivationPredictionKey());

						DoAction();
					})
			);
		}
	}
	else
	{
		DoAction();
	}
}

void UDLGameplayAbilityBase::TriggerEventActiveVC(const FGameplayTag VCDefId, const EVCTimeOutAction VCTimeOutAction)
{
	const auto Def = GetVirtualCameraDefMap(VCDefId);
	if (!ensureAlwaysMsgf(Def, TEXT("无法找到虚拟相机的定义，%s"), *VCDefId.ToString()))
	{
		return;
	}

	const auto VCController = Cast<IDLVirtualCameraController>(GetActorInfo().AvatarActor);
	if (VCController)
	{
		VCController->ActiveVirtualCamera(Def->VirtualCameraChannelTag, Def->VCDataAssetArray, VCTimeOutAction);
	}

	// 跟踪需要确保技能结束一定要停止的 正在活跃的 VC 
	if (Def->bWhenAbilityEndStopVC)
	{
		ActivatingVCDefIds.Add(VCDefId);
	}
}

void UDLGameplayAbilityBase::OnAbilityStageChange(EDLAbilityStage NewStage)
{
	this->K2_OnAbilityStageChange(CurrentStage, NewStage);

	if (NewStage == EDLAbilityStage::Spell)
	{
		this->OnSpellStart();
	}
	else if (NewStage == EDLAbilityStage::Pre)
	{
		this->OnPreStart();
	}
	else if (NewStage == EDLAbilityStage::Post)
	{
		this->OnPostStart();
	}
	else if (NewStage == EDLAbilityStage::Channel)
	{
		this->OnChannelStart();
	}

	const UDLUnitAbilitySystemComponent* ASC = Cast<UDLUnitAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo_Checked());
	ASC->OnStageChange.Broadcast(this, CurrentStage, NewStage);
}

void UCalculateAbilityAttackInfo::Exec_Implementation(UDLGameplayAbilityBase* Ability,
		const TArray<FDLCalculateAttackInfoTargetInfo>& TargetActorArray,
		AActor* Instigate,
		AActor* EffectCauser,
		AActor* EffectTrigger,
		TArray<FCharacterAttackArg>& OutInfo)
{
	OutInfo.Empty();

	for (const auto& Target : TargetActorArray)
	{
		if (Target.Target)
		{
			FCharacterAttackArg& Arg = OutInfo.AddZeroed_GetRef();

			Arg.Target = Target.Target;
			Arg.EffectCauser = EffectCauser;
			Arg.AttackTags = Target.Tags;
		}
	}
}



#if WITH_EDITOR

void UDLGameplayAbilityBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	this->CheckEditorState();

	static FName EventActionArrayName(TEXT("EventActionArray"));
	if (PropertyChangedEvent.GetPropertyName() == EventActionArrayName)
	{

	}
}

void UDLGameplayAbilityBase::PostInitProperties()
{
	Super::PostInitProperties();
}



bool UDLGameplayAbilityBase::Modify(const bool bAlwaysMarkDirty)
{
	return Super::Modify(bAlwaysMarkDirty);
}

void UDLGameplayAbilityBase::CheckEditorState()
{
	// 技能是否需要用到武器
	//bUsedWeapon = AbilityTags.HasTag(EAbilityTagDef::EAbility::ECategory::ELogicType::WeaponAttack);
}

#endif

UDLChannelAbilityBase::UDLChannelAbilityBase()
{
}

void UDLChannelAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
											const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
											const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UDLChannelAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const bool bReplicateEndAbility, const bool bWasCancelled)
{
	// 技能结束 也需要结束 GE，考虑到打断的情况
	this->EndAutoActiveGE();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

TArray<FName> UDLChannelAbilityBase::GetActiveStartSections()
{
	TArray<FName> Ret;
	if (UAnimMontage* Montage = Cast<UAnimMontage>(AnimationInfo.AnimMontage.LoadSynchronous()))
	{
		for (const auto& Sec : Montage->CompositeSections)
		{
			Ret.Add(Sec.SectionName);
		}
	}
	return Ret;
}

FDLChannelTimeSpanStageInfo UDLChannelAbilityBase::FindBastChannelTimeSpanStageInfo(float CurrentTimeSpan)
{
	ChannelTimeSpanStageInfoArray.Sort([](const FDLChannelTimeSpanStageInfo& A, const FDLChannelTimeSpanStageInfo& B) {return A.MinTimeSpan > B.MinTimeSpan; });
	for (const auto& Info : ChannelTimeSpanStageInfoArray)
	{
		if (CurrentTimeSpan >= Info.MinTimeSpan)
		{
			return Info;
		}
	}

	UE_LOG(LogDLAbility, Error, TEXT("FindBastChannelTimeSpanStageInfo  失败，CurrentTimeSpan %f"), CurrentTimeSpan);
	return FDLChannelTimeSpanStageInfo{};
}

void UDLChannelAbilityBase::EndAutoActiveGE()
{
	for (const auto& AutoActiveGEHandle : AutoEndGEHandleArray)
	{
		if (AutoActiveGEHandle.IsValid())
		{
			UE_LOG(LogDLAbility,
				   Log,
				   TEXT("RemoveAutoActiveEffect suc[%s] handle<%s>"),
				   TO_STR(AutoActiveGEHandle.WasSuccessfullyApplied()),
				   *AutoActiveGEHandle.ToString());

			this->BP_RemoveGameplayEffectFromOwnerWithHandle(AutoActiveGEHandle);
		}
	}
}

void UDLChannelAbilityBase::OnAbilityStageChange(EDLAbilityStage NewStage)
{
	Super::OnAbilityStageChange(NewStage);

	if (CurrentStage == EDLAbilityStage::None && NewStage == EDLAbilityStage::Channel)
	{
		AutoEndGEHandleArray.Empty();

		for (const auto& AutoActiveEffectInfo : AutoActiveEffectInfoArray)
		{
			if (AutoActiveEffectInfo.EffectClass)
			{
				const auto AutoActiveGEHandle = this->BP_ApplyGameplayEffectToOwner(AutoActiveEffectInfo.EffectClass, AutoActiveEffectInfo.Level, AutoActiveEffectInfo.StackCount);

				if (AutoActiveEffectInfo.NeedStopWhenAbilityEnd)
				{
					AutoEndGEHandleArray.AddDefaulted_GetRef() = AutoActiveGEHandle;
				}

				UE_LOG(LogDLAbility,
					Log,
					TEXT("AutoActiveEffectInfo suc[%s] handle<%s> %s"),
					TO_STR(AutoActiveGEHandle.IsValid() && AutoActiveGEHandle.WasSuccessfullyApplied()),
					*AutoActiveGEHandle.ToString(),
					TO_STR(AutoActiveEffectInfo.EffectClass));
			}
		}
	}
	else if (CurrentStage == EDLAbilityStage::Channel && NewStage == EDLAbilityStage::Pre)
	{
		this->EndAutoActiveGE();
		AutoEndGEHandleArray.Empty();
	}
}
