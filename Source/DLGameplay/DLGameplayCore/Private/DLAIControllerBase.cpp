
#include "GameplayFramwork/DLAIControllerBase.h"

#include "DLGameplayAbilityBase.h"
#include "IDLGameSavingSystem.h"
#include "IDLNPCSavingAccessor.h"
#include "IDLPlayerSavingAccessor.h"
#include "Misc/RuntimeErrors.h"
#include "GameplayCoreLog.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayFramwork/DLCharacterBase.h"
#include "GameplayFramwork/DLPlayerStateBase.h"
#include "GameplayFramwork/Component/DLCharacterBattleStateComp.h"
#include "GameplayFramwork/Component/DLCharacterStateExtensionComp.h"

ADLAIControllerBase::ADLAIControllerBase()
{
	bWantsPlayerState = true;
}

FDLGeneralHandle ADLAIControllerBase::ListenUnitTagsChange(AActor* TargetUnit, FGameplayTag ListenTag,
														   const FOnUnitTagsChange& Listener)
{
	const FDLGeneralHandle Ret;
	IAbilitySystemInterface* AbilitySystem = Cast<IAbilitySystemInterface>(TargetUnit);
	if (!ensureAlwaysMsgf(AbilitySystem, TEXT("正在监听一个不支持的 单位")))
	{
		return Ret;
	}

	const auto ASC = AbilitySystem->GetAbilitySystemComponent();
	if (!ensureAlwaysMsgf(ASC, TEXT("这个单位技能组件 不应该是 Nullptr")))
	{
		return Ret;
	}

	const auto Handle = ASC->RegisterGenericGameplayTagEvent().AddWeakLambda(this, [Listener](const FGameplayTag& Tag, int32 Count)
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		Listener.ExecuteIfBound(Tag, Count);
	});

	UnitTagsChangeHandleMap.Add(Ret, Handle);

	return Ret;
}

void ADLAIControllerBase::RemoveUnitTagsChangeListener(AActor* TargetUnit, const FDLGeneralHandle& Handle)
{
	if (UnitTagsChangeHandleMap.Contains(Handle))
	{
		IAbilitySystemInterface* AbilitySystem = Cast<IAbilitySystemInterface>(TargetUnit);
		if (!ensureAlwaysMsgf(AbilitySystem, TEXT("正在监听一个不支持的 单位")))
		{
			return;
		}

		const auto ASC = AbilitySystem->GetAbilitySystemComponent();
		if (!ensureAlwaysMsgf(ASC, TEXT("这个单位技能组件 不应该是 Nullptr")))
		{
			return;
		}

		ASC->RegisterGenericGameplayTagEvent().Remove(UnitTagsChangeHandleMap[Handle]);
	}
}

bool ADLAIControllerBase::AI_CanActivateAbility(AActor* TargetUnit,
	const TSubclassOf<UDLGameplayAbilityBase> AbilityClass)
{
	if (IsValid(AbilityClass))
	{
		IAbilitySystemInterface* AbilitySystem = Cast<IAbilitySystemInterface>(TargetUnit);
		if (!ensureAlwaysMsgf(AbilitySystem, TEXT("正在监听一个不支持的 单位")))
		{
			return false;
		}

		const auto ASC = AbilitySystem->GetAbilitySystemComponent();
		if (!ensureAlwaysMsgf(ASC, TEXT("这个单位技能组件 不应该是 Nullptr")))
		{
			return false;
		}
		auto Spec = ASC->FindAbilitySpecFromClass(AbilityClass);
		if (!Spec)
		{
			return false;
		}
		if (!Spec->GetPrimaryInstance())
		{
			return false;
		}
		const auto Handle = Spec->Handle;
		const FGameplayAbilityActorInfo Info = (Spec->GetPrimaryInstance()->GetActorInfo());
		return  Spec->GetPrimaryInstance()->CanActivateAbility(Handle, &Info);
	}
	return false;
}

bool ADLAIControllerBase::TryActiveAbility(TSubclassOf<UDLGameplayAbilityBase> AbilityClass, FDLAIActiveAbilityArg Arg)
{
	IAbilitySystemInterface* AbilitySystem = Cast<IAbilitySystemInterface>(GetPawn());
	if (!ensureAlwaysMsgf(AbilitySystem, TEXT("正在监听一个不支持的 单位")))
	{
		return false;
	}

	const auto ASC = AbilitySystem->GetAbilitySystemComponent();
	if (!ensureAlwaysMsgf(ASC, TEXT("这个单位技能组件 不应该是 Nullptr")))
	{
		return false;
	}

	return ASC->TryActivateAbilityByClass(AbilityClass.Get());
}

void ADLAIControllerBase::CallCounterByName(FName CounterName, int32 Count)
{
	UFunction* Func = FindFunctionChecked(CounterName);
	if (Func)
	{
		ProcessEvent(Func, &Count);
	}
}

void ADLAIControllerBase::LockUnit(AActor* Unit)
{
	if (const auto BaseCharacter = Cast<ADLCharacterBase>(GetPawn()))
	{
		if (const auto LockUnit = Cast<IDLLockableUnit>(Unit))
		{
			const auto Index = LockUnit->GetDefaultLockPointComponent();
			const auto Comp = LockUnit->GetLockPointComponent(Index);
			if (Comp)
			{
				BaseCharacter->ServerSetTargetLock(Unit, Index);
			}
			else
			{
				UE_LOG(LogDLGameplayCoreAI, Error, TEXT("锁定了一个非法的目标 %s 不存在 Index %d"), *GetNameSafe(Unit), Index);
			}
		}
		else
		{
			UE_LOG(LogDLGameplayCoreAI, Error, TEXT("锁定了一个非法的目标 %s"), *GetNameSafe(Unit));
		}
	}
}

void ADLAIControllerBase::UnlockUnit()
{
	if (const auto BaseCharacter = Cast<ADLCharacterBase>(GetPawn()))
	{
		BaseCharacter->ServerSetTargetLock(nullptr, INDEX_NONE);
	}
}

void ADLAIControllerBase::ServerSetAttackTarget(AActor* TargetActor)
{
	const auto BattleStateComp = GetPlayerState<APlayerState>()->FindComponentByClass<UDLCharacterBattleStateComp>();
	if (BattleStateComp)
	{
		BattleStateComp->ServerSetAttackTarget(TargetActor);
	}
}

void ADLAIControllerBase::ServerCancelAttackTarget(AActor* TargetActor)
{
	const auto BattleStateComp = GetPlayerState<APlayerState>()->FindComponentByClass<UDLCharacterBattleStateComp>();
	if (BattleStateComp)
	{
		BattleStateComp->ServerCancelAttackTarget(TargetActor);
	}
}

void ADLAIControllerBase::ServerApplyGameplayEffect(AActor* TargetActor,
	TSubclassOf<UGameplayEffect> GameplayEffectClass, int32 Level)
{
	const FDLGeneralHandle Ret;
	IAbilitySystemInterface* AbilitySystem = Cast<IAbilitySystemInterface>(TargetActor);
	FGameplayEffectContextHandle effectContext = AbilitySystem->GetAbilitySystemComponent()->MakeEffectContext();
	effectContext.AddSourceObject(this);

	
	FGameplayEffectSpec Spec(GameplayEffectClass->GetDefaultObject<UGameplayEffect>(), effectContext, Level);
	AbilitySystem->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(Spec);
	
}

ADLCharacterBase* ADLAIControllerBase::GetCharacterBase() const
{
	return Cast<ADLCharacterBase>(GetPawn());
}

void ADLAIControllerBase::OnCharacterDied()
{
	if (HasAuthority())
	{
		// 标记角色死亡
		GetCharacterBase()->ServerSetCharacterDied();

		// 移除玩家的控制
		this->UnPossess();
	}
}

void ADLAIControllerBase::ServerSetupCharacter(const UDLCharacterAsset* Asset, ADLCharacterBase* InCharacter,
                                               ADLPlayerStateBase* InPlayerState)
{
	if (!InPlayerState->IsInitPS())
	{
		ADLPlayerStateBase::FPlayerBaseInfo Arg;
		Arg.Pawn = InCharacter;

		IDLGameSavingSystem* GameSavingSystem = IDLGameSavingSystem::Get(this);
		IDLNPCSavingAccessor* NPCSavingAccessor = GameSavingSystem->GetNPCSavingAccessor();

		if (ensureAlwaysMsgf(NPCSavingAccessor, TEXT("IDLNPCSavingAccessor 接口是空的, NPC丢失数据")))
		{
			FDTNPCSetupInfo SetupInfo;
			const bool IsOK = NPCSavingAccessor->ReadSetupInfo(InPlayerState->GetCharacterId(), SetupInfo);
			ensureAsRuntimeWarning(IsOK);

			for (const auto& Weapon : SetupInfo.WeaponInfoArray)
			{
				auto& WeaponInfo = Arg.CharacterWeaponInfo.AddZeroed_GetRef();
				WeaponInfo.WeaponAssetId = Weapon.WeaponAssetId;

				for (const auto& Info : Weapon.WeaponAttachInfos)
				{
					auto& AddInfo = WeaponInfo.WeaponAttachInfos.AddZeroed_GetRef();
					AddInfo.WeaponId = Info.WeaponId;
					AddInfo.BoneSocketTag = Info.BoneSocketTag;
				}
			}


			for (const auto& Value : SetupInfo.SetupAbility)
			{
				ADLPlayerStateBase::FAbilityInfo Info;
				Info.Class = Value.LoadSynchronous();
				Arg.AbilityInfos.Add(Info);
			}


			for (const auto& Value : SetupInfo.SetupGameplayEffect)
			{
				ADLPlayerStateBase::FAbilityInfo Info;
				Info.Class = Value.LoadSynchronous();
				Arg.GameplayEffectInfos.Add(Info);
			}
		}

		if (!InPlayerState->ServerInitPlayerBaseInfo(Arg))
		{
			ensureAlwaysMsgf(false, TEXT("initGamePlayState failed"));
		}
	}

	this->Possess(InCharacter);

	// 初始化角色
	InCharacter->InitCharacter(InPlayerState->GetCharacterInfoBase(), Asset);

	const auto Extension = InPlayerState->FindComponentByClass<UDLCharacterStateExtensionComp>();
	if (Extension)
	{
		Extension->OnDied_CallAndRegister(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnCharacterDied));
	}

}


void ADLAIControllerBase::TryTurnToRotation(FRotator Rotator, bool ImmediatelyTurn, float OverrideYawSpeed)
{
	if (const auto MyCharacter = GetCharacterBase())
	{
		MyCharacter->TurnToTargetRotation(Rotator, ImmediatelyTurn, OverrideYawSpeed);
	}
	else
	{
		UE_LOG(LogDLGameplayCoreAI, Error, TEXT("TryTurnToRotation 获取Character失败"));
	}
}

void ADLAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ADLAIControllerBase::InitPlayerState()
{
	if (GetNetMode() != NM_Client)
	{
		UWorld* const World = GetWorld();

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.ObjectFlags |= RF_Transient;

		TSubclassOf<APlayerState> PlayerStateClassToSpawn = PlayerStateClass;
		if (PlayerStateClassToSpawn.Get() == nullptr)
		{
			UE_LOG(LogPlayerController, Log, TEXT("ADLAIControllerBase::InitPlayerState: the PlayerStateClass  is null, falling back to APlayerState."));
			PlayerStateClassToSpawn = APlayerState::StaticClass();
		}

		PlayerState = World->SpawnActor<APlayerState>(PlayerStateClassToSpawn, SpawnInfo);
	}

}

void ADLAIControllerBase::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		FRotator NewControlRotation = GetControlRotation();

		// Look toward focus
		const FVector FocalPoint = GetFocalPoint();
		if (FAISystem::IsValidLocation(FocalPoint))
		{
			if (EnableFaceWithTime)
			{
				UCharacterMovementComponent* CMC = GetCharacter()->GetCharacterMovement();
				const FRotator CurrentRot = GetPawn()->GetActorRotation();
				const FRotator DesiredRot = (FocalPoint - GetPawn()->GetPawnViewLocation()).Rotation();
				const FRotator DeltaRot = CMC->GetDeltaRotation(DeltaTime);
				NewControlRotation = CurrentRot;
				NewControlRotation.Yaw = FMath::FixedTurn(CurrentRot.Yaw, DesiredRot.Yaw, DeltaRot.Yaw);
			}
			else
			{
				NewControlRotation = (FocalPoint - MyPawn->GetPawnViewLocation()).Rotation();
			}
		}
		else if (bSetControlRotationFromPawnOrientation)
		{
			NewControlRotation = MyPawn->GetActorRotation();
		}

		// Don't pitch view unless looking at another pawn
		if (NewControlRotation.Pitch != 0 && Cast<APawn>(GetFocusActor()) == nullptr)
		{
			NewControlRotation.Pitch = 0.f;
		}

		SetControlRotation(NewControlRotation);

		if (bUpdatePawn)
		{
			const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

			if (CurrentPawnRotation.Equals(NewControlRotation, 1e-3f) == false)
			{
				MyPawn->FaceRotation(NewControlRotation, DeltaTime);
			}
		}
	}
	
}
