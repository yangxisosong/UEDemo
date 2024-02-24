#include "DLWeaponBase.h"


FName ADLWeaponBase::GetWeaponId() const
{
	return WeaponId;
}

void ADLWeaponBase::OnHitUnit(const TArray<FHitResult>& HitRets)
{
	for (const auto& Ret : HitRets)
	{
		UE_LOG(LogDLWeapon, Log, TEXT("Weapon %s HitUnit %s"), *ToString(), *Ret.ToString());
	}

	this->K2_OnHitUnits(HitRets);
}

void ADLWeaponBase::OnBeginAttack_Implementation(const FWeaponAttackArg& Parameter)
{
	if (!SelectorInstance)
	{
		SelectorInstance = NewObject<UDLUnitSelectorInstance>(this, *FString::Printf(TEXT("%s Selector"), *this->ToString()));
		SelectorInstance->OnSelectUnit.AddDynamic(this, &ADLWeaponBase::OnHitUnit);
		SelectorInstance->HitDetectionType = EUnitSelectorDetectionType::Duration;
	}

	const auto& Context = GetAttackContext();
	if (!Context.SelectorFilterPrototype)
	{
		UE_LOG(LogDLWeapon, Error, TEXT("配错了，忘记配置 Filter"));
		return;
	}

	SelectorInstance->Filter = Context.SelectorFilterPrototype->CloneInstance(this);
	if (Context.OverrideWeaponSelector)
	{
		SelectorInstance->UnitSelector = Context.OverrideWeaponSelector;
	}
	else
	{
		SelectorInstance->UnitSelector = DefaultUnitSelector;
	}

	FDLUnitSelectorInstanceContext InsContext;
	InsContext.Instigator = GetInstigator();
	SelectorInstance->StartSelectUnit(InsContext, this);
}

void ADLWeaponBase::OnEndAttack_Implementation()
{
	if (!ensureAlwaysMsgf(SelectorInstance, TEXT("理论上必须有 SelectInstance , 否则就是时序问题")))
	{
		return;
	}

	SelectorInstance->EndSelectUnit();
}

void ADLWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		
	DOREPLIFETIME_CONDITION_NOTIFY(ADLWeaponBase, CurrentBoneSocket, COND_OwnerOnly, REPNOTIFY_Always);
}

void ADLWeaponBase::AppendBaseWeaponTags(const FGameplayTagContainer& InTags)
{
	WeaponTags.AppendTags(InTags);
}

void ADLWeaponBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(WeaponTags);
	TagContainer.AddTag(CurrentBoneSocket);
}

bool ADLWeaponBase::IsAttacking() const
{
	return bIsAttacking;
}

void ADLWeaponBase::SetAttackContext(const FWeaponAttackContext& InAttackContext)
{
	ensureAlwaysMsgf(!bAttackContextIsValid, TEXT("确保之前是无效的，否则 Context 就会被覆盖"));
	bAttackContextIsValid = true;
	AttackContext = InAttackContext;
}

void ADLWeaponBase::ClearAttackContext()
{
	bAttackContextIsValid = false;
}

void ADLWeaponBase::BeginAttack(const FWeaponAttackArg& Parameter)
{
	if (!IsLocallyController())
	{
		return;
	}

	UE_LOG(LogDLWeapon, Log, TEXT("[%s] %s BeginAttack"), NET_ROLE_STR(this), *ToString());
	bIsAttacking = true;
	this->OnBeginAttack(Parameter);
}

void ADLWeaponBase::EndAttack()
{
	if (!IsLocallyController())
	{
		return;
	}

	this->OnEndAttack();
	bIsAttacking = false;

	UE_LOG(LogDLWeapon, Log, TEXT("[%s] %s EndAttack"), NET_ROLE_STR(this), *ToString());
}

TOptional<FWeaponAttachInfo> ADLWeaponBase::GetWeaponAttachInfo(const FGameplayTag& SocketTag)
{
	const auto Value = AttachOptionsInfo.FindByKey(SocketTag);
	if (Value)
	{
		return *Value;
	}
	return {};
}

AActor* ADLWeaponBase::CastToActor()
{
	return this;
}

UPhysicalMaterial* ADLWeaponBase::GetPhysicalMaterial() const
{
	return PhysicalMaterial;
}

FString ADLWeaponBase::ToString() const
{
	return GetNameSafe(this);
}

void ADLWeaponBase::SetCurrentBoneSocket(const FGameplayTag& BoneSocket)
{
	CurrentBoneSocket = BoneSocket;
}

FGameplayTag ADLWeaponBase::GetCurrentBoneSocket() const
{
	return CurrentBoneSocket;
}

bool ADLWeaponBase::AttachToCharacter(ACharacter* InOwnerCharacter, const FGameplayTag& SocketTag,
	const FName& SocketName)
{
	if (!(InOwnerCharacter && InOwnerCharacter->GetMesh()))
	{
		return false;
	}

	const auto& StaticAttachInfo = this->GetWeaponAttachInfo(SocketTag);
	if (!ensureAlwaysMsgf(StaticAttachInfo.IsSet(), TEXT("在武器上 应该能找到 对应的 挂载信息才对")))
	{
		return false;
	}

	const FAttachmentTransformRules Rules{
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		true
	};

	if(!ensureAlwaysMsgf(InOwnerCharacter->GetMesh()->GetSocketByName(SocketName), TEXT("无法Attach Weapon，%s 不存在，请检查角色的骨架"), *SocketName.ToString()))
	{
		return false;
	}
		
	this->SetActorTransform(StaticAttachInfo.GetValue().Transform);
	this->AttachToComponent(InOwnerCharacter->GetMesh(), Rules, SocketName);
	this->SetOwner(InOwnerCharacter);
	this->SetInstigator(InOwnerCharacter);
	this->SetCurrentBoneSocket(SocketTag);

	return true;
}

void ADLWeaponBase::DetachToCharacter()
{
	this->K2_DetachFromActor();

	// 销毁碰撞的实例
	SelectorInstance->MarkPendingKill();
	SelectorInstance = nullptr;
}

const FWeaponAttackContext& ADLWeaponBase::GetAttackContext() const
{
	ensureAlwaysMsgf(bAttackContextIsValid, TEXT("系统出现异常，AttackContext 没有被设置"));
	return AttackContext;
}

bool ADLWeaponBase::IsLocallyController() const
{
	if (const auto Pawn = Cast<APawn>(GetOwner()))
	{
		return Pawn->IsLocallyControlled();
	}
	return false;
}
