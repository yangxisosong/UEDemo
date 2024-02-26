
#include "GameplayFramwork/DLCharacterBase.h"

#include "Engine/Classes/Components/InputComponent.h"
#include "AbilitySystemComponent.h"
#include "ConvertString.h"
#include "DisplayDebugHelpers.h"
#include "DLAbilityTagsDef.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "IDLWeapon.h"
#include "IDLWeaponSystem.h"
#include "ASC/DLUnitAbilitySystemComponent.h"
#include "AttributeSet/DLCharacterAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Component/DLCharacterMovementComponentBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "GameplayCoreLog.h"
#include "DLGameplayTagDef.h"
#include "DataRegistrySubsystem.h"
#include "DLAssetManager.h"
#include "DLGameplayCoreSetting.h"
#include "DLGameplayPredictTaskComponent.h"
#include "DLTypeCast.h"
#include "Animation/DLAnimInstanceBaseV2.h"
#include "Animation/DLCharacterAnimationAsset.h"
#include "Components/ArrowComponent.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "GameplayFramwork/DLPlayerStateBase.h"
#include "GameplayFramwork/Component/DLCharacterBattleStateComp.h"
#include "GameplayFramwork/Component/DLPlayerStateExtensionComp.h"
#include "Interface/ICharacterHeadUI.h"
#include "Interface/IDLCharacterParts.h"
#include "Interface/IPlayerAttributeAccessor.h"
#include "Net/UnrealNetwork.h"
#include "PredictTask/AnimFrameFrozenTask.h"
#include "PredictTask/DLLoopholesTask.h"
#include "PredictTask/UnderAttack/UnderAttackPredictTask.h"
#include "Types/DLCharacterAssetDef.h"


//static constexpr float TurnToComplateErrorValue = 0.05f;


ADLCharacterBase::ADLCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDLCharacterMovementComponentBase>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	const auto CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	CapsuleComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->SetIsReplicated(true);


	bReplicates = true;

	bAlwaysRelevant = true;

	const auto CharacterMesh = GetMesh();
	if (CharacterMesh)
	{
		CharacterMesh->SetIsReplicated(true);
	}

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponet"));
	WidgetComponent->SetupAttachment(RootComponent);

	const auto MoveComp = GetMoveComponent();

	FDLMoveStrategyInterface MoveStrategy;
	MoveStrategy.CheckAllowMove.BindUObject(this, &ADLCharacterBase::OnCheckAllowMove);
	MoveStrategy.MaxSpeedAttribute.BindUObject(this, &ADLCharacterBase::OnGetMaxSpeed);
	MoveComp->RegisterMoveStrategy(MoveStrategy);

	PredictTaskComponent = CreateDefaultSubobject<UDLGameplayPredictTaskComponent>(TEXT("PredictTaskComp"));
	PredictTaskComponent->SetIsReplicated(true);
	PredictTaskComponent->OnMakePredictTaskActorInfo.BindWeakLambda(this, [this]()
	{
		auto P = MakeShared<FUnderAttackTaskActorInfo>();
		P->AvatarActor = this;
		P->SkeletalMeshComp = GetMesh();
		P->MoveComp = GetMoveComponent();
		return P;
	});

#if WITH_EDITOR
	ForwardArrow = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ForwardArrow"));
	if (ForwardArrow)
	{
		ForwardArrow->SetupAttachment(GetCapsuleComponent());
		ForwardArrow->SetHiddenInGame(!bShowDebug);
	}
#endif

}

bool ADLCharacterBase::OnCheckAllowMove()
{
	const auto ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		return !ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(EAbilityTagDef::EState::BlockMove));
	}

	return true;
}

float ADLCharacterBase::OnGetMaxSpeed()
{
	const auto ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		const float SpeedBase = ASC->GetNumericAttribute(UDLPlayerAttributeSet::GetSpeedBaseAttribute());
		const float Scale = ASC->GetNumericAttribute(UDLPlayerAttributeSet::GetMaxSpeedTotalScaleAttribute());

		const float Speed = CalculateMaxSpeed(SpeedBase, Scale);

		return Speed;

	}
	return 0.f;
}


UDLCharacterMovementComponentBase* ADLCharacterBase::GetMoveComponent() const
{
	return Cast<UDLCharacterMovementComponentBase>(GetCharacterMovement());
}

bool ADLCharacterBase::IsLockTargetUnit() const
{
	return LockTargetUnit.IsValid();
}

AActor* ADLCharacterBase::GetLockTargetUnit() const
{
	return LockTargetUnit.Target;
}

UDLLockPointComponent* ADLCharacterBase::GetLockPoint() const
{
	return LockPoint;
}

ADLPlayerStateBase* ADLCharacterBase::GetPlayerStateBase() const
{
	return GetPlayerState<ADLPlayerStateBase>();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ADLCharacterBase::TurnToTargetRotation(const FRotator& InTargetRotation, const bool ImmediatelyTurn, float OverrideTurnSpeedYaw)
{
	const auto MoveComp = GetMovementComp();
	MoveComp->TurnToTargetRotation(InTargetRotation, ImmediatelyTurn, OverrideTurnSpeedYaw);
}

FRotator ADLCharacterBase::GetTurnToTargetRotation() const
{
	return GetMovementComp()->GetTurnToInfo().TargetTurnToRotation;
}


void ADLCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	ensureAlwaysMsgf(!CharacterTags.IsEmpty(), TEXT("没有数据 说明调用时机不对"));

	TagContainer.AppendTags(CharacterTags);
	TagContainer.AppendTags(GetCurrentCharacterState());

}

void ADLCharacterBase::OnInitCharacter(const FCharacterInfoBase& InBaseInfo)
{
	// 获取锁定点
	GetComponents(LockPoints);

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		this->EquipWeapons();
	}

	const auto AnimInstance = Cast<UDLAnimInstanceBaseV2>(GetMesh()->GetAnimInstance());
	if (AnimInstance && MyAsset)
	{
		const auto AnimConfigAsset = MyAsset->CharacterAnimationAsset.Get();
		if (ensureAlwaysMsgf(AnimConfigAsset, TEXT("%s没有配置动画资产"), *MyAsset->GetName()))
		{
			const FAnimConfigBase& AnimConfig = AnimConfigAsset->GetAnimConfig();
			const FDLAnimInsInitParams AnimInsInitParams(AnimConfig);
			AnimInstance->InitAnimIns(AnimInsInitParams);
		}
		else
		{
			AnimInstance->InitAnimIns(FDLAnimInsInitParams({}));
		}
	}

	const auto ASC = Cast<UDLUnitAbilitySystemComponent>(GetAbilitySystemComponent());
	if (ASC)
	{
		// TODO 临时处理技能系统
		ASC->InitAbilityActorInfo(GetPlayerStateBase(), this);

		TagChangeListenerHandle = ASC->RegisterGenericGameplayTagEvent().AddWeakLambda(this, [this](const FGameplayTag& Tag, int32 Count)
		{
			FGameplayTagContainer AllState;
			this->GetOwnedGameplayTags(AllState);
			this->OnPlayerStateChange(AllState, Tag, Count > 0);
			this->OnCharacterStateChange.Broadcast(Tag, Count);
		});

		if (!HasAuthority())
		{
			auto CallSetHp = [this](const FOnAttributeChangeData& Data) {

				if (WidgetComponent->GetWidget())
				{
					auto Hp = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(this, UDLUnitAttributeBaseSet::GetHealthAttribute());
					auto MaxHp = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(this, UDLUnitAttributeBaseSet::GetMaxHealthAttribute());
					ICharacterHeadUI::Execute_UpdateHeath(WidgetComponent->GetWidget(), Hp, MaxHp);
				}

			};

			CallSetHp({});

			AttrChangeListenerHandle = GetPlayerStateBase()
				->OnGameplayAttributeValueChange(UDLUnitAttributeBaseSet::GetHealthAttribute())
				.AddWeakLambda(this, CallSetHp);
		}

	}

	static bool bOk = false;
	if (!bOk)
	{
		RegisterPredictTaskContextFactor(FDLUnderAttackTaskContext::GetDefault()->GetTypeId(), [](auto Id)->FDLGameplayPredictTaskContextBase*
			{
				if (Id == FDLUnderAttackTaskContext::GetDefault()->GetTypeId())
				{
					return new FDLUnderAttackTaskContext;
				}


				return nullptr;
			});

		RegisterPredictTaskContextFactor(FDLAnimFrameFrozenTaskContext::GetDefault()->GetTypeId(), [](auto Id)->FDLGameplayPredictTaskContextBase*
			{
				if (Id == FDLAnimFrameFrozenTaskContext::GetDefault()->GetTypeId())
				{
					return new FDLAnimFrameFrozenTaskContext;
				}

				return nullptr;
			});


		RegisterPredictTaskContextFactor(FDLLoopholesTaskContext::GetDefault()->GetTypeId(), [](auto Id)->FDLGameplayPredictTaskContextBase*
			{
				if (Id == FDLLoopholesTaskContext::GetDefault()->GetTypeId())
				{
					return new FDLLoopholesTaskContext;
				}

				return nullptr;
			});

		bOk = true;
	}


	PredictTaskComponent->Setup(Cast<APlayerController>(GetController()));
}

void ADLCharacterBase::OnUninitCharacter()
{
	const auto ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		ASC->RegisterGenericGameplayTagEvent().Remove(TagChangeListenerHandle);

		// TODO 临时处理技能系统
		ASC->CancelAbilities(nullptr, nullptr);
		ASC->RemoveAllGameplayCues();

		if (ASC->GetOwnerActor() != nullptr)
		{
			ASC->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			ASC->ClearActorInfo();
		}
	}

	if (GetPlayerStateBase())
	{
		GetPlayerStateBase()->OnGameplayAttributeValueChange(UDLUnitAttributeBaseSet::GetHealthAttribute()).Remove(AttrChangeListenerHandle);
	}
}

void ADLCharacterBase::InitCharacter(const FCharacterInfoBase& InBaseInfo, const UDLCharacterAsset* Asset)
{
	if (bIsInit)
	{
		return;
	}

	CharacterTags = InBaseInfo.CharacterTags;

	CharacterId = InBaseInfo.Id;

	MyAsset = Asset;

	checkf(MyAsset, TEXT("InitCharacter 资产加载失败，致命错误，请检查配置"));

	UE_LOG(LogDLGameplayCore, Log, TEXT("<%s> Begin  OnInitCharacter  ID : %s ObjName : %s"),
		NET_ROLE_STR(this), *GetCharacterIdChecked().ToString(), TO_STR(this));

	this->OnInitCharacter(InBaseInfo);


	const auto DT = GetBaseDataTableInfo();
	if (DT)
	{
		this->OnUpdateMoveInfo(DT->DefaultMovementInfo);
	}

	UE_LOG(LogDLGameplayCore, Log, TEXT("<%s> End  OnInitCharacter ID : %s ObjName : %s"),
		NET_ROLE_STR(this), *GetCharacterIdChecked().ToString(), TO_STR(this));

	bIsInit = true;


	const auto PSExt = GetPlayerState()->FindComponentByClass<UDLPlayerStateExtensionComp>();
	if (!ensureAlwaysMsgf(PSExt, TEXT("不可能没有 UDLPlayerStateExtensionComp On ADLCharacterBase::InitCharacter")))
	{
		return;
	}

	PSExt->PlayerPawnInitReady();
}


void ADLCharacterBase::BeginAttack_Implementation(const FWeaponAttackParameter& Parameter)
{

	UE_VLOG_UELOG(this, LogDLGameplayCore, Log, TEXT("BeginAttack WeaponSocket: %s"), *Parameter.SlotMountWeapon.ToStringSimple());

	FWeaponAttackArg Arg;
	Arg.EventTag = Parameter.EventTag;

	ForEachActiveWeapon(Parameter.SlotMountWeapon, [&Arg](auto Weapon)
	{
		ensureAlwaysMsgf(!Weapon->IsAttacking(), TEXT("这里理论上是不在攻击状态的，可能有时序问题"));

		Weapon->BeginAttack(Arg);
	});
}

void ADLCharacterBase::EndAttack_Implementation(const FWeaponAttackParameter& Parameter)
{
	ForEachActiveWeapon(Parameter.SlotMountWeapon, [](auto Weapon)
	{
		ensureAlwaysMsgf(Weapon->IsAttacking(), TEXT("这里理论上是在攻击状态的，可能有时序问题"));
		Weapon->EndAttack();
	});

	UE_VLOG_UELOG(this, LogDLGameplayCore, Log, TEXT("EndAttack WeaponSocket: %s"), *Parameter.SlotMountWeapon.ToStringSimple());

}



TArray<AActor*> ADLCharacterBase::GetCurrentWeapons()
{
	return CurrentActiveWeaponArray;
}

AActor* ADLCharacterBase::GetWeaponWithAttachSocketTag(FGameplayTag SocketTag)
{
	AActor* Ret = nullptr;

	ForEachActiveWeapon(SocketTag.GetSingleTagContainer(), [&Ret](auto Weapon)
		{
			Ret = Weapon->CastToActor();
		}
	);

	return Ret;
}

UDLLockPointComponent* ADLCharacterBase::GetLockPointComponent(int32 Index)
{
	const auto Comp = LockPoints.FindByPredicate([Index](const auto& Comp) {return Comp->Index == Index; });
	if (Comp)
	{
		return *Comp;
	}
	return nullptr;
}

int32 ADLCharacterBase::GetDefaultLockPointComponent()
{
	return DefaultLockPoint;
}

const TArray<UDLLockPointComponent*>& ADLCharacterBase::GetAllLockPoint() const
{
	return LockPoints;
}

const FLockableUnitConfig& ADLCharacterBase::GetLockableUnitConfig() const
{
	static FLockableUnitConfig Default;
	return Default;
}

UAbilitySystemComponent* ADLCharacterBase::GetAbilitySystemComponent() const
{
	const auto Interface = Cast<IAbilitySystemInterface>(GetPlayerState());
	if (Interface)
	{
		return Interface->GetAbilitySystemComponent();
	}
	return nullptr;
}

void ADLCharacterBase::GrabDebugSnapshot(FVisualLogEntry* Snapshot) const
{
#if ENABLE_VISUAL_LOG

	FGameplayTagContainer CurrentTags;

	if (bIsInit)
	{
		this->GetOwnedGameplayTags(CurrentTags);
	}

	auto& Data = Snapshot->Status.AddZeroed_GetRef();
	Data.Category = TEXT("DLCharacterState");
	Data.Add(TEXT("Tags"), CurrentTags.ToStringSimple());

#endif
}

void ADLCharacterBase::FrameFrozen(const FCharacterFrameFrozenArg Arg)
{
	UE_LOG(LogDLGameplayCore, Log, TEXT("FrameFrozen Self %s"), *GetFName().ToString());

	UClass* TaskClass = UDLAssetLib::FetchSoftClassPtr(MyAsset->FrameFrozenTaskClass);
	if (!TaskClass)
	{
		return;
	}

	FDLNetPredictTaskContext NetPredictTaskContext;
	const auto Context = MakeShared<FDLAnimFrameFrozenTaskContext>();
	Context->FrozenDuration = Arg.Duration;
	Context->Parts = Arg.Parts;
	Context->FrameFrozenType = Arg.FrameFrozenType;
	Context->AnimSpeedCurve = Arg.AnimSpeedCurve;
	Context->TaskClass = TaskClass;
	Context->UnderAttackTags = Arg.UnderAttackTags;

	NetPredictTaskContext.ContextData = Context;
	NetPredictTaskContext.Target = this;
	NetPredictTaskContext.TaskContextTypeId = Context->GetTypeId();
	NetPredictTaskContext.TaskId = FGameplayPredictTaskId::Create();

	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		this->ServerFrameFrozen(NetPredictTaskContext);
	}

	if (HasAuthority())
	{
		PredictTaskComponent->AuthorityActiveTask(NetPredictTaskContext.ContextData, NetPredictTaskContext.TaskId);
	}
	else
	{
		PredictTaskComponent->ProxyActiveTask(NetPredictTaskContext.ContextData, NetPredictTaskContext.TaskId);
	}
}

void ADLCharacterBase::ForwardTargetCharacterFrameFrozen(AActor* Target, FCharacterFrameFrozenArg Arg)
{
	UE_LOG(LogDLGameplayCore, Log, TEXT("Forward FrameFrozen Self %s Target %s"), *GetFName().ToString(), *Target->GetName());

	if (!HasAuthority())
	{
		this->ServerForwardTargetCharacterFrameFrozen(Target, Arg);
	}

	INotifyCharacterFrameFrozen* Interface = Cast<INotifyCharacterFrameFrozen>(Target);
	if (Interface)
	{
		Interface->FrameFrozen(Arg);
	}
}

void ADLCharacterBase::ServerForwardTargetCharacterFrameFrozen_Implementation(AActor* Target,
	const FCharacterFrameFrozenArg& Arg)
{
	this->ForwardTargetCharacterFrameFrozen(Target, Arg);
}

void ADLCharacterBase::ServerFrameFrozen_Implementation(FDLNetPredictTaskContext Arg)
{
	if (HasAuthority())
	{
		PredictTaskComponent->AuthorityActiveTask(Arg.ContextData, Arg.TaskId);
	}
	else
	{
		PredictTaskComponent->ProxyActiveTask(Arg.ContextData, Arg.TaskId);
	}
}


void ADLCharacterBase::MyTestRunTask(TSubclassOf<UDLGameplayPredictTask> Task)
{
	const auto Context = MakeShared<FDLUnderAttackTaskContext>();
	Context->TaskClass = Task;
	Context->ClientStartTaskTicks = FDateTime::Now().GetTicks();

	FDLNetPredictTaskContext Data;
	Data.ContextData = Context;
	Data.TaskContextTypeId = Context->GetTypeId();
	Data.TaskId = FGameplayPredictTaskId::Create();

	PredictTaskComponent->ProxyActiveTask(Context, Data.TaskId);

	this->ServerMyTestRunTask(Data);
}

void ADLCharacterBase::ServerMyTestRunTask_Implementation(const FDLNetPredictTaskContext& Arg)
{
	PredictTaskComponent->AuthorityActiveTask(Arg.ContextData, Arg.TaskId);
}

void ADLCharacterBase::AttackActors(const TArray<FCharacterAttackArg>& AttackArgArr)
{
	UE_LOG(LogDLGameplayCore, Log, TEXT("AttackActors Self %s"), *GetFName().ToString());

	TArray<FDLNetPredictTaskContext> Contexts;
	for (const auto& AttackedArg : AttackArgArr)
	{
		const auto Target = Cast<ADLCharacterBase>(AttackedArg.Target);
		if (!Target)
		{
			continue;
		}

		UClass* TaskClass = nullptr;

		const float CurrentTenacityValue = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(AttackedArg.Target, UDLUnitAttributeBaseSet::GetTenacityAttribute());

		FGameplayTagContainer QueryUnderAttackTaskTags;
		QueryUnderAttackTaskTags.AppendTags(AttackedArg.AttackTags);

		if (CurrentTenacityValue <= 0.1f)
		{
			// 如果已经在 破绽状态，尝试查找 破绽情况下播放的 受击表现
			if (ICharacterStateAccessor::Execute_k2_GetCurrentCharacterState(AttackedArg.Target).HasTag(UDLGameplayCoreSetting::Get()->CharacterLoopholesStateTag))
			{
				QueryUnderAttackTaskTags.AddTag(UDLGameplayCoreSetting::Get()->UnderAttackTaskSpasticityTag);
			}
			// 韧性为 0 并且不在大 破绽状态，则进入 大破绽
			else
			{
				TaskClass = Target->GetCharacterLoopholesTask();

				UE_LOG(LogDLGameplayCore, Log, TEXT("GetCharacterLoopholesTask %s  TaskClass: %s "), TO_STR(Target), TO_STR(TaskClass));
			}
		}

		if (!TaskClass)
		{
			TaskClass = Target->FindUnderAttackEffectTask(QueryUnderAttackTaskTags);

			UE_LOG(LogDLGameplayCore, Log, TEXT("FindUnderAttackEffectTask %s  QueryUnderAttackTaskTags: %s TaskClass: %s"),
												TO_STR(Target),
												*QueryUnderAttackTaskTags.ToStringSimple(),
												TO_STR(TaskClass)
			);
		}

		if (!TaskClass)
		{
			UE_LOG(LogDLGameplayCore, Warning, TEXT("被打的任务配置有问题，无法找到对应Tag 的任务 %s"), *AttackedArg.AttackTags.ToString());
			continue;
		}

		TSharedPtr<FDLGameplayPredictTaskContextBase> ContextData;

		// 填充 受击任务的 Task Context
		if (TaskClass->IsChildOf<UDLUnderAttackTask>())
		{
			const auto Context = MakeShared<FDLUnderAttackTaskContext>();
			ContextData = Context;

			Context->AttackTags = AttackedArg.AttackTags;

			IDLCharacterParts* PartsInterface = Cast<IDLCharacterParts>(Target);
			if (PartsInterface)
			{
				// 尝试获取攻击的部位

				const int32 CharacterBoneIndex = AttackedArg.HitResult.Item;
				UE_LOG(LogDLUnderAttack, Log, TEXT("<ADLCharacterBase::AttackActors> 受击部位:%s"), *AttackedArg.HitResult.BoneName.ToString());

				FGameplayTag&& PartTag = PartsInterface->GetPartByBodyIndex(CharacterBoneIndex);
				if (PartTag.IsValid())
				{
					// 将攻击的部位加入 AttackTag
					Context->AttackTags.AddTag(PartTag);
				}
				else
				{
					if (AttackedArg.HitResult.GetComponent())
					{
						UE_LOG(LogDLUnderAttack, Log, TEXT("<ADLCharacterBase::AttackActors> 未找到受击部位,Component:%s"), *AttackedArg.HitResult.GetComponent()->GetFullName());
					}
					UE_LOG(LogDLUnderAttack, Log, TEXT("<ADLCharacterBase::AttackActors> 未找到受击部位:%s"), *AttackedArg.HitResult.BoneName.ToString());
				}
			}

			Context->EffectCauser = AttackedArg.EffectCauser;
			Context->HitForceWorldRotation = AttackedArg.HitForceDirection;
			Context->HitRet = AttackedArg.HitResult;

			Context->ClientStartTaskTicks = FDateTime::Now().GetTicks();
		}
		// 填充 大破绽任务 的 Task Context
		else if (TaskClass->IsChildOf<UDLLoopholesTask>())
		{
			const auto Context = MakeShared<FDLLoopholesTaskContext>();
			ContextData = Context;
		}
		else
		{
			ensureAlwaysMsgf(false, TEXT("未定义的任务行为 %s"), *TaskClass->GetName());
			return;
		}

		ContextData->TaskClass = TaskClass;

		// 打包数据
		FDLNetPredictTaskContext Data;
		Data.Target = AttackedArg.Target;
		Data.TaskId = FGameplayPredictTaskId::Create();
		Data.ContextData = ContextData;
		Data.TaskContextTypeId = ContextData->GetTypeId();

		Contexts.Add(Data);
	}

	// 告知服务器 被攻击
	if (!HasAuthority())
	{
		this->ServerAttackActors(Contexts);
	}

	this->DispatchUnderAttack(Contexts);
}

void ADLCharacterBase::SetMeshVisibility(const FString& Reason, bool bPropagateToChildren, bool bVisible)
{
	if (!bVisible)
	{
		UE_LOG(LogDLGameplayCore, Log, TEXT("%s隐藏Mesh,理由:%s"), *this->GetName(), *Reason);
	}
	else
	{
		UE_LOG(LogDLGameplayCore, Log, TEXT("%s显示Mesh,理由:%s"), *this->GetName(), *Reason);
	}
	GetMesh()->SetVisibility(bVisible);
}

void ADLCharacterBase::ServerAttackActors_Implementation(const TArray<FDLNetPredictTaskContext>& Arg)
{
	this->DispatchUnderAttack(Arg);
}

void ADLCharacterBase::DispatchUnderAttack(const TArray<FDLNetPredictTaskContext>& Arg) const
{
	for (auto AttackedArg : Arg)
	{
		const auto Target = Cast<ADLCharacterBase>(AttackedArg.Target);
		if (Target)
		{
			UE_LOG(LogDLGameplayCore, Log, TEXT("DispatchUnderAttack Target %s"), *Target->GetFName().ToString());

			Target->OnUnderAttack(AttackedArg);
		}
	}
}

void ADLCharacterBase::OnUnderAttack(const FDLNetPredictTaskContext& Arg) const
{
	if (HasAuthority())
	{
		PredictTaskComponent->AuthorityActiveTask(Arg.ContextData, Arg.TaskId);

		const auto BattleStateComp = GetPlayerStateBase()->FindComponentByClass<UDLCharacterBattleStateComp>();
		if (BattleStateComp)
		{
			BattleStateComp->ServerInjectBeAttackEvent();
		}

	}
	else
	{
		PredictTaskComponent->ProxyActiveTask(Arg.ContextData, Arg.TaskId);
	}

	// TODO 受击处理 还需要继续 优化，支持 可被受击的接口，基于事件机制
	if (const auto Data = CastPredictTaskContext<FDLUnderAttackTaskContext>(Arg.ContextData.Get()))
	{
		FDLUnitUnderAttackInfo Info;
		Info.AttackTags = Data->AttackTags;
		Info.EffectCauser = Data->EffectCauser;
		OnUnderAttackEvent.Broadcast(Info);
	}
}

UClass* ADLCharacterBase::FindUnderAttackEffectTask(const FGameplayTagContainer& AttackTags) const
{
	if (!ensureAlwaysMsgf(MyAsset, TEXT("不可能为空")))
	{
		return nullptr;
	}

	for (const auto& TaskData : MyAsset->UnderAttackEffectTaskArray)
	{
		if ((!TaskData.RequirementsTag.IsEmpty()) && TaskData.RequirementsTag.RequirementsMet(AttackTags))
		{
			const auto ClassPtr = TaskData.TaskClass;
			return ClassPtr;
		}
	}

	return nullptr;
}


UClass* ADLCharacterBase::GetCharacterLoopholesTask() const
{
	if (!ensureAlwaysMsgf(MyAsset, TEXT("不可能为空")))
	{
		return nullptr;
	}

	return MyAsset->LoopholesTaskClass.Get();
}


void ADLCharacterBase::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	if (DebugDisplay.IsDisplayOn(FName(TEXT("AbilityChain"))))
	{
		if (GetAbilitySystemComponent())
		{
			Cast<UDLUnitAbilitySystemComponent>(GetAbilitySystemComponent())->DisplayDebugInfo(Canvas, DebugDisplay, YL, YPos);
		}
		return;
	}
	else if (DebugDisplay.IsDisplayOn(FName(TEXT("Movement"))))
	{
		const auto Movement = Cast<UDLCharacterMovementComponentBase>(GetMoveComponent());
		if (Movement)
		{
			Movement->ShowDebugInfo(Canvas, DebugDisplay, YL, YPos);
		}

		return;
	}

	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);
}

FName ADLCharacterBase::GetBoneSocketNameByTag(const FGameplayTag& BoneSocketTag)
{
	const auto Setting = UDLGameplayCoreSetting::Get();

	const auto BoneTypeTag = CharacterTags.Filter(FGameplayTagContainer{ Setting->CharacterBoneTypeRootTag });
	ensureAlwaysMsgf(BoneTypeTag.Num() == 1, TEXT("一定有骨骼类型的Tag"));
	const auto Info = Setting->CharacterBoneTypeConfigs.FindByKey(BoneTypeTag.First());
	if (ensureAlwaysMsgf(Info, TEXT("无法查询到骨骼信息")))
	{
		ensureAlwaysMsgf(BoneSocketTag.MatchesTag(Info->BoneSocketRootTag), TEXT("这里输入的Tag 应该和角色的骨架匹配"));
	}

	const auto Name = Setting->TagMapCharacterBoneSocketName.FindByKey(BoneSocketTag);
	if (Name)
	{
		return Name->BoneSocketName;
	}
	return FName{};
}


void ADLCharacterBase::TryTurnToTargetRotation(const float Dt)
{
	//const auto& TurnToInfo = GetTurnToInfo();
	//if (TurnToInfo.bIsTurnToComplate)
	//{
	//	return;
	//}
	//
	//FRotator NewRotation = FRotator::ZeroRotator;
	//auto&& MyRotation = GetActorRotation();
	//
	//if (TurnToInfo.bImmediatelyTurn)
	//{
	//	NewRotation = TurnToInfo.TargetTurnToRotation;
	//}
	//else
	//{
	//	NewRotation = UKismetMathLibrary::RInterpTo_Constant(MyRotation, TurnToInfo.TargetTurnToRotation, Dt, TurnToInfo.TurnSpeedYaw);
	//}
	//
	//NewRotation.Pitch = MyRotation.Pitch;
	//NewRotation.Roll = MyRotation.Roll;
	//
	//UE_LOG(LogDLGameplayCore, Verbose, TEXT("(%s)<%s> DeltaTime %2f, TargetRotation %s, PreRotation %s NewRotation %s SpeedYaw %2f"),
	//	TO_STR(this), NET_ROLE_STR(this),
	//	Dt,
	//	*TurnToInfo.TargetTurnToRotation.ToString(),
	//	*MyRotation.ToString(),
	//	*NewRotation.ToString(),
	//	TurnToInfo.TurnSpeedYaw);
	//
	//this->SetActorRotation(NewRotation);
	//
	//
	//
	//// 检查是否转到位了 如果转到位 就标记完成
	//auto&& TurnToMyRotation = GetActorRotation();
	//if (FMath::IsNearlyEqual(FRotator::ClampAxis(TurnToMyRotation.Yaw), FRotator::ClampAxis(TurnToInfo.TargetTurnToRotation.Yaw), TurnToComplateErrorValue))
	//{
	//	GetTurnToInfo_Mutable().bIsTurnToComplate = true;
	//	UE_LOG(LogDLGameplayCore, Verbose, TEXT("[%s] TryTurnToTargetRotation %s  Complate"), NET_ROLE_STR(this), *NewRotation.ToString());
	//}
}

void ADLCharacterBase::UnPossessed()
{
	this->OnUninitCharacter();

	Super::UnPossessed();
}

float ADLCharacterBase::GetCurrentAttributeValue_Implementation(const FGameplayAttribute& Attribute) const
{
	if (const auto ASC = GetAbilitySystemComponent())
	{
		return ASC->GetNumericAttribute(Attribute);
	}
	return 0.f;
}



FGameplayTagContainer ADLCharacterBase::GetCurrentCharacterState() const
{
	FGameplayTagContainer Ret;

	if (const auto ASC = GetAbilitySystemComponent())
	{
		ASC->GetOwnedGameplayTags(Ret);
	}

	Ret.AppendTags(PrivateCharacterTags);

	return Ret;
}

void ADLCharacterBase::ServerSetCharacterDied()
{
	bIsDied = true;
}

UDLCharacterMovementComponentBase* ADLCharacterBase::GetMovementComp() const
{
	return Cast<UDLCharacterMovementComponentBase>(GetMoveComponent());
}

FGameplayTagContainer ADLCharacterBase::k2_GetCurrentCharacterState_Implementation() const
{
	return GetCurrentCharacterState();
}

bool ADLCharacterBase::K2_IsTurnToComplate_Implementation() const
{
	return GetMovementComp()->GetTurnToInfo().bIsTurnToComplate;
}

bool ADLCharacterBase::K2_IsLockTargetUnit_Implementation() const
{
#if 0
	FGameplayTagContainer&& CharacterTags = ICharacterStateAccessor::Execute_GetCurrentCharacterState(this);
	if (CharacterTags.HasTag(EDLGameplay::ECharacter::EState::LockTarget))
	{
		return true;
	}
#endif

	return IsLockTargetUnit();
}



void ADLCharacterBase::OnRep_LockTargetUnit()
{
	this->UpdateLockTargetState();

	UE_LOG(LogDLGameplayCore, Log, TEXT("LockTargetUnit  %s"), TO_STR(LockTargetUnit.Target));
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void ADLCharacterBase::AddOwnerTags(const FGameplayTagContainer InTags)
{
	const auto ASC = Cast<UDLUnitAbilitySystemComponent>(GetAbilitySystemComponent());
	if (ASC)
	{
		UE_LOG(LogDLGameplayCore, Verbose, TEXT("<%s> %s  AddOwnerTags %s"), TO_STR(this), NET_ROLE_STR(this), *InTags.ToStringSimple());
		ASC->AddGameplayTags(InTags);
	}
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void ADLCharacterBase::RemoveOwnerTags(const FGameplayTagContainer InTags)
{
	const auto ASC = Cast<UDLUnitAbilitySystemComponent>(GetAbilitySystemComponent());
	if (ASC)
	{
		UE_LOG(LogDLGameplayCore, Verbose, TEXT("<%s> %s   RemoveOwnerTags %s"), TO_STR(this), NET_ROLE_STR(this), *InTags.ToStringSimple());
		ASC->RemoveGameplayTags(InTags);
	}
}

void ADLCharacterBase::OnDebugShowChange_Implementation(bool Show)
{
#if WITH_EDITOR

	ForwardArrow->SetHiddenInGame(!bShowDebug);

#endif
}

AActor* ADLCharacterBase::K2_GetLockTargetUnit_Implementation() const
{
	return GetLockTargetUnit();
}

void ADLCharacterBase::K2_GetTurnToTargetRotation_Implementation(FRotator& OutTargetRotation) const
{
	OutTargetRotation = GetTurnToTargetRotation();;
}

UDLLockPointComponent* ADLCharacterBase::K2_GetLockPoint_Implementation() const
{
	return GetLockPoint();
}

bool ADLCharacterBase::K2_IsDied_Implementation() const
{
	return bIsDied;
}

void ADLCharacterBase::ServerTurnToTargetRotation_Implementation(const FRotator& InTargetRotation, bool ImmediatelyTurn,
	float OverrideTurnSpeedYaw)
{
	this->TurnToTargetRotation(InTargetRotation, ImmediatelyTurn, OverrideTurnSpeedYaw);
}


void ADLCharacterBase::UpdateLockTargetState()
{
	if (!LockTargetUnit.IsValid())
	{
		LockPoint = nullptr;
	}
	else
	{
		const auto LockUnit = CastChecked<IDLLockableUnit>(LockTargetUnit.Target);
		LockPoint = LockUnit->GetLockPointComponent(LockTargetUnit.LockPointIndex);
	}
}

FName ADLCharacterBase::GetCharacterIdChecked() const
{
	ensureAlwaysMsgf(!CharacterId.IsNone(), TEXT("无效的ID"));
	return CharacterId;
}


void ADLCharacterBase::ServerSetTargetLock_Implementation(AActor* Unit, int32 LockPointIndex)
{
	if (!Unit)
	{
		LockTargetUnit = {};
		this->UpdateLockTargetState();
		return;
	}

	if (const auto LockTarget = Cast<IDLLockableUnit>(Unit))
	{
		const auto Point = LockTarget->GetLockPointComponent(LockPointIndex);
		if (Point)
		{
			LockTargetUnit.Target = Unit;
			LockTargetUnit.LockPointIndex = LockPointIndex;
			this->UpdateLockTargetState();
		}
		else
		{
			UE_LOG(LogDLGameplayCore, Error, TEXT("ServerSetTargetLock Not Suc! Unit %s, LockPointIndex %d  无法获取到锁定点"), *GetNameSafe(Unit), LockPointIndex);
		}
	}
	else
	{
		UE_LOG(LogDLGameplayCore, Error, TEXT("ServerSetTargetLock Not Suc! Unit %s, LockPointIndex %d  不是可锁定的对象 "), *GetNameSafe(Unit), LockPointIndex);
	}
}


void ADLCharacterBase::OnDebugTick_Implementation(float DeltaSeconds)
{
}

void ADLCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if WITH_EDITOR
	if (bEnabledDebugTick)
	{
		this->OnDebugTick(DeltaSeconds);
	}
#endif

	//this->TurnToCharacterRotation(DeltaSeconds);
}


void ADLCharacterBase::TurnToCharacterRotation(const float DtTime)
{
	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		//this->TryTurnToTargetRotation(DtTime);
	}
}


void ADLCharacterBase::OnPlayerStateInitDataReady(ADLPlayerStateBase* PS, const FPlayerInitData& Data)
{
	const auto Ext = PS->FindComponentByClass<UDLPlayerStateExtensionComp>();
	if (Ext)
	{
		Ext->OnPlayerInitDataReady_Unregister(this);
	}

	this->InitCharacter(Data.InfoBase, Data.CharacterAsset);
}

void ADLCharacterBase::OnPlayerStateChange(const FGameplayTagContainer& AllState, const FGameplayTag& Tag, bool IsAdd)
{
	PredictTaskComponent->UpdateTags(AllState);

	// 根据当前的 Tag 来匹配速度相关的数据
	const auto Data = GetBaseDataTableInfo();
	if (Data)
	{
		const auto& MoveInfo = Data->CharacterMovementInfos;

		bool HasBastInfo = false;

		for (const auto& Info : MoveInfo)
		{
			if (AllState.HasTag(Info.MoveState))
			{
				this->OnUpdateMoveInfo(Info);
				HasBastInfo = true;
				break;
			}
		}

		if (!HasBastInfo)
		{
			this->OnUpdateMoveInfo(Data->DefaultMovementInfo);
		}
	}
}

void ADLCharacterBase::OnUpdateMoveInfo(const FCharacterMovementInfo& Info)
{
	GetMovementComp()->UpdateMoveInfo(Info);
}

void ADLCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}


bool CheckIsCharacterBlocked(ACharacter* Character, TArray<AActor*>& OverlappedActors)
{
	const UWorld* WorldContext = Character->GetWorld();
	const auto* Capsule = Character->GetCapsuleComponent();
	const FVector& CharacterLocation = Character->GetActorLocation();

	UKismetSystemLibrary::CapsuleOverlapActors(WorldContext, CharacterLocation
		, Capsule->GetScaledCapsuleRadius(), Capsule->GetScaledCapsuleHalfHeight()
		, { EObjectTypeQuery::ObjectTypeQuery3 }, APawn::StaticClass()
		, { Character }, OverlappedActors);

	return OverlappedActors.Num() > 0;
}

//防止角色被卡住
void CheckIsInBlockedZone(ACharacter* Character)
{
	if (!Character->HasAuthority())
	{
		return;
	}

	const auto* Capsule = Character->GetCapsuleComponent();
	const auto& CollisionType = Capsule->GetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn);
	if (CollisionType != ECollisionResponse::ECR_Block)
	{
		return;
	}
	TArray<AActor*> OverlappedActors;
	if (!CheckIsCharacterBlocked(Character, OverlappedActors))
	{
		return;
	}

	Character->SetActorLocation(Character->GetActorLocation() + Character->GetActorForwardVector() * Capsule->GetScaledCapsuleHalfHeight());

	CheckIsInBlockedZone(Character);

	// UWorld* WorldContext = Character->GetWorld();
	// FVector AdjustedLocation = Character->GetActorLocation();
	// FRotator AdjustedRotation = Character->GetActorRotation();
	// if (WorldContext->FindTeleportSpot(Character, AdjustedLocation, AdjustedRotation))
	// {
	// 	Character->SetActorLocationAndRotation(AdjustedLocation, AdjustedRotation, false, nullptr, ETeleportType::TeleportPhysics);
	// }
}


void ADLCharacterBase::K2_ChangeCollisionPreset(const FName& CollisionPreset)
{
	UCapsuleComponent* LocCapsuleComponent = GetCapsuleComponent();
	if (!ensureAlwaysMsgf(LocCapsuleComponent, TEXT("%s没有胶囊体！！"), *GetName()))
	{
		return;
	}
	UE_LOG(LogDLGameplayCore, Warning, TEXT("<ADLCharacterBase::K2_ChangeCollisionPreset> %s将碰撞预设从%s变更为%s"), *GetName()
		, *LocCapsuleComponent->GetCollisionProfileName().ToString(), *CollisionPreset.ToString());

	LocCapsuleComponent->SetCollisionProfileName(CollisionPreset);


	CheckIsInBlockedZone(this);
}

void ADLCharacterBase::K2_ChangeCollisionResponseToChannel(ECollisionChannel Channel, ECollisionResponse NewResponse)
{
	UCapsuleComponent* LocCapsuleComponent = GetCapsuleComponent();
	if (!ensureAlwaysMsgf(LocCapsuleComponent, TEXT("%s没有胶囊体！！"), *GetName()))
	{
		return;
	}
	UE_LOG(LogDLGameplayCore, Warning, TEXT("<ADLCharacterBase::K2_ChangeCollisionResponseToChannel> %s将%s通道的碰撞设置从%s变更为%s")
		, *GetName(), ENUM_TO_STR(ECollisionChannel, Channel)
		, ENUM_TO_STR(ECollisionResponse, LocCapsuleComponent->GetCollisionResponseToChannel(Channel))
		, ENUM_TO_STR(ECollisionResponse, NewResponse));

	LocCapsuleComponent->SetCollisionResponseToChannel(Channel, NewResponse);


	CheckIsInBlockedZone(this);
}

void ADLCharacterBase::K2_ChangeCollisionResponseToAllChannels(ECollisionResponse NewResponse)
{
	UCapsuleComponent* LocCapsuleComponent = GetCapsuleComponent();
	if (!ensureAlwaysMsgf(LocCapsuleComponent, TEXT("%s没有胶囊体！！"), *GetName()))
	{
		return;
	}

	UE_LOG(LogDLGameplayCore, Warning, TEXT("<ADLCharacterBase::K2_ChangeCollisionResponseToAllChannels> %s将所有通道的碰撞设置变更为%s")
	, *GetName(), ENUM_TO_STR(ECollisionResponse, NewResponse));

	LocCapsuleComponent->SetCollisionResponseToAllChannels(NewResponse);

	CheckIsInBlockedZone(this);
}


void ADLCharacterBase::ForEachActiveWeapon(const FGameplayTagContainer& MatchWeaponTags,
										   const TFunction<void(TScriptInterface<IDLWeapon> Weapon)>& Visitor)
{
	for (const auto& Weapon : CurrentActiveWeaponArray)
	{
		const TScriptInterface<IDLWeapon> IWeapon(Weapon);

		if (!ensureAlwaysMsgf(IWeapon, TEXT("存在无效的武器")))
		{
			continue;
		}

		if (IWeapon->HasAllMatchingGameplayTags(MatchWeaponTags))
		{
			Visitor(IWeapon);
		}
	}
}



void ADLCharacterBase::InitHeadBar(TScriptInterface<ICharacterStateAccessor> AttributeAccessor, TScriptInterface<ICharacterStateListener> Listener)
{
	if (!AttributeAccessor)
	{
		return;
	}


	auto AttributeChangeCall = [this, AttributeAccessor](const FOnAttributeChangeData&)
	{
		const auto CurrentHp = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(AttributeAccessor.GetObject(), UDLPlayerAttributeSet::GetHealthAttribute());
		const auto MaxHp = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(AttributeAccessor.GetObject(), UDLPlayerAttributeSet::GetMaxHealthAttribute());

		const auto CurrentMana = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(AttributeAccessor.GetObject(), UDLPlayerAttributeSet::GetSoulPowerAttribute());
		const auto MaxMana = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(AttributeAccessor.GetObject(), UDLPlayerAttributeSet::GetMaxSoulPowerAttribute());

		UObject* WObj = WidgetComponent->GetUserWidgetObject();
		if (WObj && WObj->Implements<UCharacterHeadUI>())
		{
			ICharacterHeadUI::Execute_UpdateHeath(WObj, CurrentHp, MaxHp);
			ICharacterHeadUI::Execute_UpdateMana(WObj, CurrentMana, MaxMana);
		}

	};

	AttributeChangeCall({});

	Listener->OnGameplayAttributeValueChange(UDLPlayerAttributeSet::GetHealthAttribute()).AddWeakLambda(this, AttributeChangeCall);
}

void ADLCharacterBase::EquipWeapons()
{
	const auto& WeaponInfoArray = GetPlayerStateBase()->GetCurrentWeaponInfo();

	for (const auto& WeaponInfo : WeaponInfoArray)
	{
		auto&& WeaponArr = IDLWeaponSystem::Get().CreateDLWeaponsByAssetId(GetWorld(), WeaponInfo.WeaponAssetId);

		for (const auto& WeaponAttachInfo : WeaponInfo.WeaponAttachInfos)
		{
			auto ID = WeaponAttachInfo.WeaponId;
			const auto WeaponPtr = WeaponArr.FindByPredicate([ID](const TScriptInterface<IDLWeapon>& Value) {return Value->GetWeaponId() == ID; });

			if (WeaponPtr == nullptr || WeaponPtr->GetInterface() == nullptr)
			{
				continue;
			}

			const auto Weapon = *WeaponPtr;

			const auto AttachInfo = WeaponInfo.WeaponAttachInfos.FindByKey(Weapon->GetWeaponId());
			if (!ensureAlwaysMsgf(AttachInfo, TEXT("应该能找到 对应的 挂载信息才对")))
			{
				continue;
			}

			FName SocketName = GetBoneSocketNameByTag(AttachInfo->BoneSocketTag);
			ensureAlwaysMsgf(!SocketName.IsNone(), TEXT("挂载武器 应该可以拿到一个正确的插槽名字，否则就会出问题. 是不是 武器忘记配置Soeket，是否角色的 Tag->Socket %s 的配置错误"), *AttachInfo->BoneSocketTag.ToString());
			this->CurrentActiveWeaponArray.Add(Weapon->CastToActor());

			Weapon->AttachToCharacter(this, AttachInfo->BoneSocketTag, SocketName);
		}
	}
}


void ADLCharacterBase::BeginPlay()
{
	Super::BeginPlay();


}

void ADLCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (GetPlayerState())
	{
		const auto PSExt = GetPlayerState()->FindComponentByClass<UDLPlayerStateExtensionComp>();
		if (!ensureAlwaysMsgf(PSExt, TEXT("不可能没有 UDLPlayerStateExtensionComp On ADLCharacterBase::OnRep_PlayerState")))
		{
			return;
		}

		// 根据 PS 来初始化 Pawn 比较合理，因为 Pawn 是可以替换的
		PSExt->OnPlayerInitDataReady_CallAndRegister(FOnPlayerInitDataReady::FDelegate::CreateUObject(this, &ThisClass::OnPlayerStateInitDataReady));
	}
}


void ADLCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADLCharacterBase, CurrentActiveWeaponArray);

	// TODO 优化为 PushModel
	DOREPLIFETIME_CONDITION_NOTIFY(ADLCharacterBase, LockTargetUnit, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ADLCharacterBase, bIsDied, COND_None, REPNOTIFY_Always);
}

void ADLCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR

void ADLCharacterBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	static FName DebugShowName(TEXT("bShowDebug"));
	if (PropertyChangedEvent.GetPropertyName() == DebugShowName)
	{
		this->OnDebugShowChange(bShowDebug);
	}
}

#endif
