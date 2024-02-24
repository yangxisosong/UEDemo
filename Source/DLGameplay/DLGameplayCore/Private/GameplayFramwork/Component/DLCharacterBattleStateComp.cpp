#include "GameplayFramwork/Component/DLCharacterBattleStateComp.h"

#include "AbilitySystemComponent.h"
#include "ASC/DLUnitAbilitySystemComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayFramwork/DLPlayerStateBase.h"
#include "Net/UnrealNetwork.h"
#include "GameplayCoreLog.h"
#include "GameplayFramwork/Component/DLCharacterStateExtensionComp.h"

namespace EDLGameplayTag
{
	UE_DEFINE_GAMEPLAY_TAG(Unit_State_Battle, "Gameplay.Unit.State.Battle");
}


namespace EDLGameplayMsg
{
	// NPC 进入战斗状态
	UE_DEFINE_GAMEPLAY_TAG(Character_InBattleState, "DLMsgType.Gameplay.Character.InBattleState");

	// NPC 离开战斗状态
	UE_DEFINE_GAMEPLAY_TAG(Character_OutBattleState, "DLMsgType.Gameplay.Character.OutBattleState");
}



UDLCharacterBattleStateComp::UDLCharacterBattleStateComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDLCharacterBattleStateComp::ServerSetAttackTarget(AActor* TargetActor)
{
	check(GetOwner()->GetLocalRole() == ENetRole::ROLE_Authority);

	if (TargetActor->Implements<UCharacterStateAccessor>())
	{
		if (ICharacterStateAccessor::Execute_K2_IsDied(TargetActor))
		{
			return;
		}
	}

	const auto BattleStateCom = FindBscEnsure(TargetActor);
	if (!BattleStateCom)
	{
		return;
	}

	BattleStateCom->ServerBeAttackTarget(this);
}

void UDLCharacterBattleStateComp::ServerCancelAttackTarget(AActor* TargetActor)
{
	check(GetOwner()->GetLocalRole() == ENetRole::ROLE_Authority);

	if (TargetActor->Implements<UCharacterStateAccessor>())
	{
		if (ICharacterStateAccessor::Execute_K2_IsDied(TargetActor))
		{
			return;
		}
	}

	const auto BattleStateCom = FindBscEnsure(TargetActor);
	if (!BattleStateCom)
	{
		return;
	}

	BattleStateCom->ServerRemoveAttackSelf(this);
}

void UDLCharacterBattleStateComp::ServerInjectAbilityActiveEvent(const FGameplayTagContainer& AbilityTags)
{
	if (Cast<APlayerState>(GetOwner())->IsABot())
	{
		return;
	}

	// TODO 策划配置警告  那些是 攻击类型的技能 ？？

	LastUsedAbilityTime = TimeTotal;
	bInLieState = false;
}

void UDLCharacterBattleStateComp::ServerInjectBeAttackEvent()
{
	if (Cast<APlayerState>(GetOwner())->IsABot())
	{
		return;
	}

	LastBeAttackedTime = TimeTotal;
	bInLieState = false;
}

AActor* UDLCharacterBattleStateComp::GetAsEnemyActor() const
{
	return AsEnemyUnit;
}

AActor* UDLCharacterBattleStateComp::GetEnemyActor()
{
	for (const auto& Value : AsEnemyBscArray)
	{
		if (Value.IsValid())
		{
			return Value.Get()->GetOwner();
		}
	}
	return nullptr;
}

UDLCharacterBattleStateComp* UDLCharacterBattleStateComp::FindBscEnsure(AActor* Actor) const
{
	const APlayerState* PS = Cast<APlayerState>(Actor);
	if (!PS)
	{
		if (const ACharacter* Character = Cast<ACharacter>(Actor))
		{
			PS = Character->GetPlayerState();
		}
	}

	if (!ensureAlwaysMsgf(PS, TEXT("ServerSetAttackTarget 无法获取到 PlayState")))
	{
		return nullptr;
	}

	const auto BattleStateCom = PS->FindComponentByClass<ThisClass>();
	if (!ensureAlwaysMsgf(BattleStateCom, TEXT("无法获取到 BattleStateCom")))
	{
		return nullptr;
	}

	return BattleStateCom;
}

void UDLCharacterBattleStateComp::ServerBeAttackTarget(UDLCharacterBattleStateComp* Bsc)
{
	AsEnemyBscArray.AddUnique(Bsc);

	AsEnemyUnit = GetEnemyActor();
}

void UDLCharacterBattleStateComp::ServerRemoveAttackSelf(UDLCharacterBattleStateComp* Bsc)
{
	AsEnemyBscArray.Remove(Bsc);

	AsEnemyUnit = GetEnemyActor();
}

void UDLCharacterBattleStateComp::CheckBattleState()
{
	bool TempBattleState = false;

	if (!bInLieState)
	{
		TempBattleState = true;
	}
	else
	{
		if (AsEnemyBscArray.Num() > 0)
		{
			TempBattleState = true;
		}
	}

	// 经过一系列复杂的判断 最终来更新 实际的状态
	SetBattleState(TempBattleState);
}

void UDLCharacterBattleStateComp::SetBattleState(const bool InBattle)
{
	static FGameplayTagContainer BattleState{EDLGameplayTag::Unit_State_Battle.GetTag()};

	if (bInBattleState != InBattle)
	{
		// 更新值
		bInBattleState = InBattle;

		// 修改玩家的State
		const auto PS = Cast<ADLPlayerStateBase>(GetOwner());
		const auto ASC = PS->GetASC();
		if (ASC)
		{
			if (bInBattleState)
			{
				ASC->AddGameplayTags(BattleState);
			}
			else
			{
				ASC->RemoveGameplayTags(BattleState);
			}
		}

		this->BroadcastBattleState();
	}
}

void UDLCharacterBattleStateComp::BroadcastBattleState()
{
	// 推送广播事件
	OnCharacterBattleStateChange.Broadcast(bInBattleState);

	FDLGameplayBattleStateMsgBody Body;
	Body.TargetPS = Cast<APlayerState>(GetOwner());

	auto& MsgSys = UGameplayMessageSubsystem::Get(this);

	MsgSys.BroadcastMessage(bInBattleState ? EDLGameplayMsg::Character_InBattleState : EDLGameplayMsg::Character_OutBattleState, Body);

	UE_LOG(LogDLGameplayCore, Log, TEXT("UDLCharacterBattleStateComp::BroadcastBattleState  bInBattleState %d"), bInBattleState);
}

// Called when the game starts
void UDLCharacterBattleStateComp::BeginPlay()
{
	Super::BeginPlay();

	//检查自己是不是挂了
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		const auto Ext = Cast<APlayerState>(GetOwner())->FindComponentByClass<UDLCharacterStateExtensionComp>();
		if (Ext)
		{
			Ext->OnDied_CallAndRegister(FSimpleMulticastDelegate::FDelegate::CreateWeakLambda(this,[ this]()
			{
				this->bInBattleState = false;
			}));
		}
	}

}

void UDLCharacterBattleStateComp::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	TimeTotal += DeltaTime;

	// 检测躺平转态
	if (!bInLieState)
	{
		const float NearlyTime = FMath::Max(LastBeAttackedTime, LastUsedAbilityTime);
		if ((NearlyTime + MinLeaveLieStateDuration) < TimeTotal)
		{
			bInLieState = true;
		}
	}


	// 进行 Check 结算
	this->CheckBattleState();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UDLCharacterBattleStateComp::OnRep_InBattleState()
{
	this->BroadcastBattleState();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UDLCharacterBattleStateComp::OnRep_AsEnemyUnit()
{
	OnChangeAsEnemyUnit.Broadcast(AsEnemyUnit);
}


void UDLCharacterBattleStateComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDLCharacterBattleStateComp, bInBattleState, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDLCharacterBattleStateComp, AsEnemyUnit, COND_None, REPNOTIFY_Always);
}
