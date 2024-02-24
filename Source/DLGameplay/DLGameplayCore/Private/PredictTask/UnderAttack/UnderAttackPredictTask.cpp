#include "PredictTask/UnderAttack/UnderAttackPredictTask.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "DLGameplayCoreSetting.h"
#include "IDLPlayerInputCmdSystem.h"
#include "GameplayFramwork/DLCharacterBase.h"
#include "PredictTask/AnimFrameFrozenTask.h"

DEFINE_LOG_CATEGORY(LogDLUnderAttack);



struct AutoExec
{
	AutoExec(TFunction<void()> Call) { Call(); };
};


static AutoExec AutoRegPredictTaskContext([]()
{


});

bool FDLUnderAttackTaskContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!Super::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	Ar << EffectCauser;

	if (!AttackTags.NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	Ar << HitForceWorldRotation;

	if (!HitRet.NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	bOutSuccess = true;
	return true;
}

bool UDLUnderAttackTask::CanCancelTaskByOther_Implementation(UDLGameplayPredictTask* Other) const
{

	//定帧任务能够打断受击任务
	// if (Other)
	// {
	// 	const FGameplayTag& FrameFrozenTag = FGameplayTag::RequestGameplayTag("Gameplay.PredictTask.FrameFrozen");
	// 	if (Other->TaskTags.HasTag(FrameFrozenTag))
	// 	{
	// 		return true;
	// 	}
	// }

	if (const auto Task = Cast<UDLUnderAttackTask>(Other))
	{
		return Task->TaskPriority >= this->TaskPriority;
	}

	return false;
}

bool UDLUnderAttackTask::OnInitTask(const TSharedPtr<FPredictTaskActorInfo> InActorInfo,
									const FDLGameplayPredictTaskContextBase* InContext)
{
	if (const auto* UnderAttackData = CastPredictTaskContext<FDLUnderAttackTaskContext>(InContext))
	{
		Context = *UnderAttackData;
		ActorInfo = *static_cast<FUnderAttackTaskActorInfo*>(InActorInfo.Get());
		AnimInstance = ActorInfo.SkeletalMeshComp->GetAnimInstance();

		ensureAlwaysMsgf(AnimInstance, TEXT("动画实例不应该是空的"));

		if (!AnimInstance)
		{
			return false;
		}

		return true;
	}

	return false;
}

void UDLUnderAttackTask::OnBlockOtherTask(UDLGameplayPredictTask* Other)
{
	Super::OnBlockOtherTask(Other);


	UE_LOG(LogDLUnderAttack, Log, TEXT("%s[%s] %s Block %s")
		, *PredictTaskComponent->GetOwner()->GetName(), *PredictTaskComponent->NetRoleToString(), *ToString(), *Other->ToString());

	if (const auto Task = Cast<UDLUnderAttackTask>(Other))
	{
		this->K2_OnBlockOtherTask(Task);
	}
}

void UDLUnderAttackTask::ActiveTask()
{
	if (bIsConditionInterruptAbility)
	{
		if (HasAuthority())
		{
			// TODO 这么做不太好  等需求彻底 踩坑后  使用 状态来控制 打断逻辑
			const auto AbilitySysInterface = Cast<IAbilitySystemInterface>(ActorInfo.AvatarActor);
			if (AbilitySysInterface && AbilitySysInterface->GetAbilitySystemComponent())
			{
				const auto Ability = AbilitySysInterface->GetAbilitySystemComponent()->GetAnimatingAbility();
				if (Ability)
				{
					AbilitySysInterface->GetAbilitySystemComponent()->CancelAbility(Ability);
				}
			}

			// 开始回精力
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(ActorInfo.AvatarActor,
														UDLGameplayCoreSetting::Get()->StartRecoveryStaminaEventTag,
														FGameplayEventData {}
			);

		}
	}

	Super::ActiveTask();
}

void UDLUnderAttackTask::EndTask(const bool Cancel)
{
	Super::EndTask(Cancel);

	// 确保在任务结束时，Tag 被全部移除 ，这里是保底机制
	if (!ensureAlwaysMsgf(AddTags.Num() == 0, TEXT("应该由逻辑来驱动 Tag 正确的移除，请检查逻辑是不是对的")))
	{
		this->RemoveGameplayTagsToOwner(AddTags);
	}

	this->ClearAllowInputCmdDef();
}

FDLGameplayPredictTaskContextBase* UDLUnderAttackTask::GetContext()
{
	return &Context;
}

bool UDLUnderAttackTask::NeedRunRootMotionSrc() const
{
	if (ActorInfo.AvatarActor)
	{
		const auto Role = ActorInfo.AvatarActor->GetLocalRole();
		return Role > ENetRole::ROLE_SimulatedProxy;
	}
	return false;
}

float UDLUnderAttackTask::GetCurrentMontageSectionLength(const FName SectionName, bool& Suc) const
{
	float Ret = 0.f;
	Suc = false;

	const auto MyCurrentMontage = GetCurrentMontageChecked();
	if (MyCurrentMontage)
	{
		const int32 Index = MyCurrentMontage->GetSectionIndex(SectionName);
		if (ensureAlwaysMsgf(Index != INDEX_NONE, TEXT("没有配置正确的 SectionName")))
		{
			Ret = MyCurrentMontage->GetSectionLength(Index);
			Suc = true;
		}
	}

	return Ret;
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void UDLUnderAttackTask::SetCurrentMontagePlayRate(const float Rate)
{
	const auto MyCurrentMontage = GetCurrentMontageChecked();
	if (MyCurrentMontage)
	{
		const auto MontageIns = AnimInstance->GetActiveInstanceForMontage(MyCurrentMontage);
		if (MontageIns)
		{
			MontageIns->SetPlayRate(Rate);
		}
	}
}

UAnimMontage* UDLUnderAttackTask::GetCurrentMontageChecked() const
{
	ensureAlwaysMsgf(CurrentMontage, TEXT("一定存在当前的 Montage"));
	return CurrentMontage;
}

void UDLUnderAttackTask::SetCurrentMontage(UAnimMontage* Montage)
{
	CurrentMontage = Montage;
}

void UDLUnderAttackTask::AddGameplayTagsToOwner(const FGameplayTagContainer Tags)
{
	const auto Character = Cast<ADLCharacterBase>(ActorInfo.AvatarActor);
	ensureAlwaysMsgf(Character, TEXT("AvatarActor 一定是 ADLCharacterBase"));

	if (Character)
	{
		Character->AddOwnerTags(Tags);

		AddTags.AppendTags(Tags);
	}
}

void UDLUnderAttackTask::RemoveGameplayTagsToOwner(const FGameplayTagContainer Tags)
{
	const auto Character = Cast<ADLCharacterBase>(ActorInfo.AvatarActor);
	ensureAlwaysMsgf(Character, TEXT("AvatarActor 一定是 ADLCharacterBase"));

	if (Character)
	{
		Character->RemoveOwnerTags(Tags);

		AddTags.RemoveTags(Tags);
	}
}

bool UDLUnderAttackTask::IsLocalPlayerController() const
{
	if (const APawn* Pawn = Cast<APawn>(ActorInfo.AvatarActor))
	{
		const auto Controller = Pawn->GetController();
		if (Controller)
		{
			return Controller->IsLocalPlayerController();
		}
	}
	return false;
}

void UDLUnderAttackTask::SwitchAllowInputCmdDef(const FName DefId)
{
	if (!IsLocalPlayerController())
	{
		return;
	}


	UDLInputCmdSubsystem* InputCmdSys = UDLInputCmdSubsystem::Get(this);
	if (!InputCmdSys)
	{
		return;
	}

	// 先释放之前的状态
	if (!CurrentApplyAllowInputDefId.IsNone())
	{
		const auto Def = AllowInputCmdDefine.FindByKey(CurrentApplyAllowInputDefId);
		if (Def)
		{
			InputCmdSys->RemoveCmdTags(Def->AllowInputCmdTags, GetNameSafe(this));
		}

		CurrentApplyAllowInputDefId = {};
	}

	const auto Def = AllowInputCmdDefine.FindByKey(DefId);
	if (!ensureAlwaysMsgf(Def, TEXT("一定能够找到 DefID %s"), *DefId.ToString()))
	{
		return;
	}

	const auto InputCmd = InputCmdSys->GetCurrentCacheCmd();
	InputCmdSys->ClearInputCmd();

	InputCmdSys->AddAllowCmdTags(Def->AllowInputCmdTags, GetNameSafe(this));

	UE_LOG(LogDLUnderAttack, Verbose, TEXT("InputCmdSys->AddAllowCmdTags  %s"), *Def->AllowInputCmdTags.ToString());

	if (InputCmd)
	{
		InputCmdSys->ExecCmd(
			InputCmd,
			FString::Printf(TEXT("DLUnderAttackTask %s"), *TaskTags.ToString())
		);
	}
	else
	{
		InputCmdSys->TryCheckMovementCmd();
	}

	CurrentApplyAllowInputDefId = DefId;
}

void UDLUnderAttackTask::ClearAllowInputCmdDef()
{
	if (!IsLocalPlayerController())
	{
		return;
	}


	UDLInputCmdSubsystem* InputCmdSys = UDLInputCmdSubsystem::Get(this);
	if (!InputCmdSys)
	{
		return;
	}

	// 先释放之前的状态
	if (!CurrentApplyAllowInputDefId.IsNone())
	{
		const auto Def = AllowInputCmdDefine.FindByKey(CurrentApplyAllowInputDefId);
		if (Def)
		{
			InputCmdSys->RemoveCmdTags(Def->AllowInputCmdTags, GetNameSafe(this));
		}

		CurrentApplyAllowInputDefId = {};
	}
}


void UDLUnderAttackTask::DrawDebugLine(const FVector LineStart, const FVector LineEnd, const FLinearColor LineColor,
									   const float Duration, const float Thickness) const
{
	UKismetSystemLibrary::DrawDebugLine(this, LineStart, LineEnd, LineColor, Duration, Thickness);
}
