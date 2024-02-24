#include "DLGameplaySubObjectBase.h"

#include "ConvertString.h"
#include "DLGameplayAbilityBase.h"
#include "DLSubObjectLogDef.h"
#include "ASC/DLUnitAbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "DLAbilitySystem/private/TargetData/DLAbilityTargetData.h"
#include "Interface/ICharacterStateAccessor.h"
#include "Interface/IDLCharacterParts.h"
#include "Kismet/KismetSystemLibrary.h"


void ADLGameplaySubObjectBase::ActionGenerate()
{
	UE_LOG(LogDLSubObj, Log, TEXT("<%s> ActionGenerate Begin  =======  [%s]"), NET_ROLE_STR(this), *GetFName().ToString());

	for (const auto& Action : this->GenerateActions)
	{
		if (Action)
		{
			Action->Trigger(this);
		}
	}

	this->OnGenerateAction();

	UE_LOG(LogDLSubObj, Log, TEXT("<%s> ActionGenerate End  =======  [%s]"), NET_ROLE_STR(this), *GetFName().ToString());
}

void ADLGameplaySubObjectBase::ActionTimeOut()
{
	UE_LOG(LogDLSubObj, Log, TEXT("<%s> ActionTimeOut Begin  =======  [%s]"), NET_ROLE_STR(this), *GetFName().ToString());

	for (const auto& Action : this->TimeOutActions)
	{
		if (Action)
		{
			Action->Trigger(this);
		}
	}

	UE_LOG(LogDLSubObj, Log, TEXT("<%s> ActionTimeOut End  =======  [%s]"), NET_ROLE_STR(this), *GetFName().ToString());
}

void ADLGameplaySubObjectBase::ActionDestroy()
{
	UE_LOG(LogDLSubObj, Log, TEXT("<%s> ActionDestroy Begin  =======  [%s]"), NET_ROLE_STR(this), *GetFName().ToString());

	for (const auto& Action : this->DestroyActions)
	{
		if (Action)
		{
			Action->Trigger(this);
		}
	}

	for (const auto Task : ActiveTaskArray)
	{
		Task->TaskOwnerEnded();
	}
	ActiveTaskArray.Empty();

	UE_LOG(LogDLSubObj, Log, TEXT("<%s> ActionDestroy End  =======  [%s]"), NET_ROLE_STR(this), *GetFName().ToString());
}


void ADLGameplaySubObjectBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TArray<UShapeComponent*> ShapeComps;
		GetComponents<UShapeComponent>(ShapeComps);
		if (ShapeComps.Num())
		{
			ensureAlwaysMsgf(ShapeComps.Num() == 1, TEXT("应该只有一个碰撞框"));

			ShapeComps[0]->OnComponentBeginOverlap.AddDynamic(this, &ADLGameplaySubObjectBase::OnBeginOverlap);
		}
	}

}

void ADLGameplaySubObjectBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (TempData.DeferDestroy)
	{
		this->Destroy();
	}
}

bool ADLGameplaySubObjectBase::UpdateHitCount(const FHitResult& HitUnit)
{
	// 超过最大数量
	if (TempData.CurrentOverlapActorCount >= MaxOverlapActorCount)
	{
		return false;
	}

	// 检查是否合法
	if (TempData.TempOverlapActor.Contains(HitUnit.GetActor()))
	{
		return false;
	}

	TempData.TempOverlapActor.Add(HitUnit.GetActor());
	TempData.CurrentOverlapActorCount++;

	if (TempData.CurrentOverlapActorCount >= MaxOverlapActorCount)
	{
		TempData.DeferDestroy = bExceededMaxHitCountDestroy;

		if (bExceededMaxHitCountDestroy)
		{
			UE_LOG(LogDLSubObj, Log, TEXT("<ADLGameplaySubObjectBase::UpdateHitCount> MarkDestroy,HitUnit:%s")
				, *HitUnit.GetActor()->GetName());
			this->MarkDestroy();
		}

		this->OnExceededMaxHitCount();
	}

	return true;
}


void ADLGameplaySubObjectBase::OnTimeOut()
{
	ensureAlwaysMsgf(GetLocalRole() == ENetRole::ROLE_Authority, TEXT("只能在服务器有可能触发"));
	UE_LOG(LogDLSubObj, Log, TEXT("<ADLGameplaySubObjectBase::OnTimeOut> MarkDestroy"));

	this->MarkDestroy();

	this->ActionTimeOut();

	this->OnLifeTimeoutAction();
}

FHitResult ADLGameplaySubObjectBase::GetCollisionResult(UPrimitiveComponent* OverlappedComponent,
	const FHitResult& HitResult) const
{
	TArray<TEnumAsByte<EObjectTypeQuery>> TargetObjectType;
	TargetObjectType.Add(static_cast<EObjectTypeQuery>(ECC_Pawn));
	TargetObjectType.Add(static_cast<EObjectTypeQuery>(ECC_WorldStatic));

	TArray<FHitResult> TempHitResults;
	TempHitResults.Empty();

	if (Cast<USphereComponent>(OverlappedComponent))
	{
		//球
		const FVector StartPos = Cast<USphereComponent>(OverlappedComponent)->GetComponentTransform().GetLocation();
		const auto Radius = Cast<USphereComponent>(OverlappedComponent)->GetScaledSphereRadius();

		if (UKismetSystemLibrary::SphereTraceMultiForObjects(
			GetWorld(),
			StartPos,
			StartPos,
			Radius,
			TargetObjectType,
			false,
			TArray<AActor*>{},
			EDrawDebugTrace::None,
			TempHitResults,
			true
		))
		{
			for (const auto& V : TempHitResults)
			{
				if (V.GetActor() == HitResult.GetActor())
				{
					return V;
				}
			}
		}
	}
	else if(Cast<UBoxComponent>(OverlappedComponent))
	{
		//盒子
		const FVector StartPos = Cast<UBoxComponent>(OverlappedComponent)->GetComponentTransform().GetLocation();
		const FVector HalfSize = Cast<UBoxComponent>(OverlappedComponent)->GetScaledBoxExtent();
		const FRotator Rotator = Cast<UBoxComponent>(OverlappedComponent)->GetComponentTransform().Rotator();
		if (UKismetSystemLibrary::BoxTraceMultiForObjects(
			GetWorld(),
			StartPos,
			StartPos,
			HalfSize,
			Rotator,
			TargetObjectType,
			false,
			TArray<AActor*>{},
			EDrawDebugTrace::None,
			TempHitResults,
			true
		))
		{
			for (const auto& V : TempHitResults)
			{
				if (V.GetActor() == HitResult.GetActor())
				{
					return V;
				}
			}
		}
	}

	return  HitResult;
}




void ADLGameplaySubObjectBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Type::Destroyed)
	{
		if (GetLocalRole() == ENetRole::ROLE_Authority)
		{
			this->ActionDestroy();
		}

		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}

	//销毁数据
	TempData = {};

	Super::EndPlay(EndPlayReason);
}

AActor* ADLGameplaySubObjectBase::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	return const_cast<ADLGameplaySubObjectBase*>(this);
}

AActor* ADLGameplaySubObjectBase::GetGameplayTaskOwner(const UGameplayTask* Task) const
{
	return const_cast<ADLGameplaySubObjectBase*>(this);
}

UGameplayTasksComponent* ADLGameplaySubObjectBase::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	return GameplayTasksComponent;
}

void ADLGameplaySubObjectBase::OnGameplayTaskActivated(UGameplayTask& Task)
{
	ActiveTaskArray.Add(&Task);
}

void ADLGameplaySubObjectBase::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	ActiveTaskArray.Remove(&Task);
}

TArray<FActiveGameplayEffectHandle> ADLGameplaySubObjectBase::ApplyGameplayEffectWithHitResult(const FHitResult& HitResult,
																							FGameplayTag OutgoingHitDataDefName)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;


	const auto* HitData = OutgoingHitDataArray.FindByKey(OutgoingHitDataDefName);
	if (!HitData)
	{
		return AllEffects;
	}

	// 构建 TargetData
	FGameplayAbilityTargetDataHandle TargetDataHandle;

	const auto Data = new FGameplayAbilityTargetData_AttackHitResult;
	Data->HitResult = HitResult;
	Data->AttackTags = HitData->HitTags;
	TargetDataHandle.Data.Add(MakeShareable(Data));


	// 受击的表现
	if (InstanceAbility.IsValid())
	{
		if (const auto Ability = Cast<UDLGameplayAbilityBase>(InstanceAbility.Get()))
		{
			Ability->AttackActor(this, TargetDataHandle, HitData->CalculateAbilityAttackInfoClass.GetDefaultObject());
		}
	}

	// 应用GE
	for (const auto& GESpecHandle : HitData->OutgoingGESpecHandleArray)
	{
		// 添加 GE 的触发者
		const auto* GESpec = GESpecHandle.Data.Get();
		if (GESpec)
		{
			if (auto* GEContext = StructCast<FDLGameplayEffectContext>(GESpec->GetContext().Get()))
			{
				GEContext->GameplayEffectTrigger = this;
			}
		}

		// Apply GE  
		AllEffects.Append(Data->ApplyGameplayEffectSpec(*GESpecHandle.Data.Get()));
	}

	return AllEffects;
}



bool ADLGameplaySubObjectBase::IsExceededMaxOverlapActorCount() const
{
	return TempData.CurrentOverlapActorCount >= MaxOverlapActorCount;
}

void ADLGameplaySubObjectBase::MarkDestroy()
{
	TempData.DeferDestroy = true;
}


bool ADLGameplaySubObjectBase::SetupSubObject(const FDLSubObjectSetupData& Data)
{
	if (GetLocalRole() != ENetRole::ROLE_Authority)
	{
		UE_LOG(LogDLSubObj, Warning, TEXT("SetupSubObject 必须是服务端创建的 Actor"))
			return false;
	}

	MaxLifeTime = Data.LifeTime;
	OutgoingHitDataArray = Data.OutgoingHitData;
	InstanceAbility = Data.InstanceAbility;

	// 重置数据
	TempData = {};

	if (ensureAlwaysMsgf(!FMath::IsNearlyZero(MaxLifeTime), TEXT("MaxLifeTime 这个填0要翻车，因为存在 子物体一直在场景中乱飞的无法 Destroy")))
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateUObject(this, &ADLGameplaySubObjectBase::OnTimeOut), MaxLifeTime, false);
		TempData.LifeTimeHandle = Handle;
	}

	this->ActionGenerate();

	return true;
}


bool ADLGameplaySubObjectBase::IsBeginDestroy() const
{
	return TempData.DeferDestroy;
}


// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ADLGameplaySubObjectBase::OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult)
{
	if (!bOwnerActorCanOverlap)
	{
		if (OtherActor == this->GetOwner())
		{
			return;
		}
	}

	// 如果实现了部位接口，那么只有正真打到部位才算
	if (OtherActor->Implements<UDLCharacterParts>())
	{
		if (OtherBodyIndex == INDEX_NONE)
		{
			return;
		}
	}

	if (OtherActor->Implements<UCharacterStateAccessor>())
	{
		if (ICharacterStateAccessor::Execute_K2_IsDied(OtherActor))
		{
			return;
		}

		const FGameplayTagContainer State = ICharacterStateAccessor::Execute_k2_GetCurrentCharacterState(OtherActor);
		//  TODO  有机会再提出配置吧
		if (State.HasTag(FGameplayTag::RequestGameplayTag("Gameplay.Unit.State.ImmuneHit")))
		{
			return;
		}
	}

	if (!UpdateHitCount(SweepResult))
	{
		return;
	}
	const auto Result = GetCollisionResult(OverlappedComponent, SweepResult);
	// 结算打到单位的事件
	this->OnHitUnitAction(Result);
}


void UDLSubObjectHitUnitAction::OnTrigger_Implementation(ADLGameplaySubObjectBase* Object,
														 const FHitResult& HitResult)
{
}

void UDLSubObjectNormalAction::OnTrigger_Implementation(ADLGameplaySubObjectBase* Object)
{
}

void UDLSubObjectHitUnitAction::Trigger(ADLGameplaySubObjectBase* Object, const FHitResult& HitResult)
{
	if (!IsEnabled)
	{
		return;
	}

	UE_LOG(LogDLSubObj, Log, TEXT("Trigger SubObj Actio [%s]"), *GetFName().ToString());

	this->OnTrigger(Object, HitResult);
}

void UDLSubObjectNormalAction::Trigger(ADLGameplaySubObjectBase* Object)
{
	if (!IsEnabled)
	{
		return;
	}

	UE_LOG(LogDLSubObj, Log, TEXT("Trigger SubObj Actio [%s]"), *GetFName().ToString());

	this->OnTrigger(Object);
}



ADLGameplaySubObjectBase::ADLGameplaySubObjectBase()
{
	GameplayTasksComponent = CreateDefaultSubobject<UGameplayTasksComponent>(TEXT("GameplayTasksComponent"));
}

