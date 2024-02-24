// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameplayCore/Public/Animation/AnimDebug/DLAnimationDebugComponent.h"

#include "DLGameplayCore/Public/Animation/AnimDebug/DLAnimDebugInterface.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


class AnimationDebugCompManager
{
public:
	TArray<TWeakObjectPtr<UDLAnimationDebugComponent>> RegisteredDLAnimationDebugComps;
	TWeakObjectPtr<UDLAnimationDebugComponent> CurOpenedDebugView;

private:
	AnimationDebugCompManager() = default;
	~AnimationDebugCompManager()
	{
		CurOpenedDebugView.Reset();
		RegisteredDLAnimationDebugComps.Reset();
	}

public:
	static AnimationDebugCompManager* Get()
	{
		static AnimationDebugCompManager* Ins = new AnimationDebugCompManager();
		return Ins;
	}

	void RegisteredDebugComp(UDLAnimationDebugComponent* Comp);
	void UnRegisterDebugComp(UDLAnimationDebugComponent* Comp);
	void OpenDebugView(const ACharacter* Character);
	void OpenDebugView(const UWorld* World);
	void OpenDebugView(const int32 Index);
	void CloseDebugView();
	void RemoveAllInvalidComps();
	UDLAnimationDebugComponent* GetCurDebugComp();
	bool GetIsOpened(const UDLAnimationDebugComponent* Comp);
	void PrintAll();
	int32 GetCurIndex();
};

void AnimationDebugCompManager::OpenDebugView(const UWorld* World)
{
	if (!IsValid(World))
	{
		return;
	}

	const ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
	OpenDebugView(Character);
}

void AnimationDebugCompManager::OpenDebugView(const int32 Index)
{
	RemoveAllInvalidComps();
	if (!RegisteredDLAnimationDebugComps.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Warning, TEXT("<AnimationDebugCompManager::OpenDebugView> 索引无效！"));
		return;
	}

	const auto& DebugComp = RegisteredDLAnimationDebugComps[Index];
	OpenDebugView(Cast<ACharacter>(DebugComp->GetOwner()));
}

UDLAnimationDebugComponent* AnimationDebugCompManager::GetCurDebugComp()
{
	if (CurOpenedDebugView.IsValid())
	{
		return CurOpenedDebugView.Get();
	}
	return nullptr;
}

bool AnimationDebugCompManager::GetIsOpened(const UDLAnimationDebugComponent* Comp)
{
	return Comp == GetCurDebugComp();
}

void AnimationDebugCompManager::PrintAll()
{
	RemoveAllInvalidComps();
	for (int i = 0; i < RegisteredDLAnimationDebugComps.Num(); ++i)
	{
		const auto& DebugComp = RegisteredDLAnimationDebugComps[i];
		UE_LOG(LogTemp, Log, TEXT("RegisteredDLAnimationDebugComp---> Index:%d,Name:%s ")
				, i, *DebugComp->GetOwner()->GetName());
	}
}

int32 AnimationDebugCompManager::GetCurIndex()
{
	const auto* CurComp = GetCurDebugComp();
	if (!CurComp)
	{
		return -1;
	}
	return RegisteredDLAnimationDebugComps.IndexOfByKey(CurComp);
}

void AnimationDebugCompManager::RegisteredDebugComp(UDLAnimationDebugComponent* Comp)
{
	if (!IsValid(Comp))
	{
		return;
	}
	RemoveAllInvalidComps();
	RegisteredDLAnimationDebugComps.AddUnique(TWeakObjectPtr<UDLAnimationDebugComponent>(Comp));
}

void AnimationDebugCompManager::UnRegisterDebugComp(UDLAnimationDebugComponent* Comp)
{
	if (!IsValid(Comp))
	{
		return;
	}

	RegisteredDLAnimationDebugComps.RemoveAll(
		[Comp](const TWeakObjectPtr<UDLAnimationDebugComponent>& Item)
		{
			return !Item.IsValid() || Item.Get() == Comp;
		}
	);
	RemoveAllInvalidComps();
}

void AnimationDebugCompManager::OpenDebugView(const ACharacter* Character)
{
	if (CurOpenedDebugView.IsValid() && IsValid(Character) && CurOpenedDebugView.Get()->GetOwner() == Character)
	{
		return;
	}
	CloseDebugView();

	for (auto DebugComp : RegisteredDLAnimationDebugComps)
	{
		if (DebugComp.IsValid() && DebugComp.Get()->GetOwner() == Character)
		{
			DebugComp.Get()->OpenDebugView();
			CurOpenedDebugView = DebugComp;
			break;
		}
	}
}

void AnimationDebugCompManager::CloseDebugView()
{
	if (CurOpenedDebugView.IsValid())
	{
		CurOpenedDebugView.Get()->CloseDebugView();
	}
	CurOpenedDebugView.Reset();
	RemoveAllInvalidComps();
}

void AnimationDebugCompManager::RemoveAllInvalidComps()
{
	RegisteredDLAnimationDebugComps.RemoveAll(
		[](const TWeakObjectPtr<UDLAnimationDebugComponent>& Item)
		{
			return !Item.IsValid();
		}
	);
}

static FAutoConsoleCommandWithWorldAndArgs CVarDLAnimationDebugView(
TEXT("DL.Animation.DebugView.Open")
, TEXT("显示动画debug信息（默认显示当前角色的）,格式：DL.Animation.DebugView.Open 角色索引")
, FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
	[](const TArray<FString>& Args, UWorld* World)
	{
		auto* Ins = AnimationDebugCompManager::Get();
		if (!Ins)
		{
			return;
		}
		if (Args.Num())
		{
			const auto& Arg0 = Args[0];
			if (!Arg0.IsEmpty() && Arg0.IsNumeric())
			{
				Ins->OpenDebugView(FCString::Atoi(*Arg0));
			}
		}
		else
		{
			Ins->OpenDebugView(World);
		}

	}), ECVF_Cheat
);

static FAutoConsoleCommand CVarDLAnimationPrint(
TEXT("DL.Animation.DebugView.PrintAll")
, TEXT("显示所有可用的动画调试控件信息")
, FConsoleCommandDelegate::CreateLambda(
	[]()
	{
		auto* Ins = AnimationDebugCompManager::Get();
		if (Ins)
		{
			Ins->PrintAll();
		}
	})
, ECVF_Cheat);

static FAutoConsoleCommand CVarDLAnimationHide(
TEXT("DL.Animation.DebugView.Hide")
, TEXT("关闭动画debug信息")
, FConsoleCommandDelegate::CreateLambda(
	[]()
	{
		auto* Ins = AnimationDebugCompManager::Get();
		if (Ins)
		{
			Ins->CloseDebugView();
		}
	})
, ECVF_Cheat);

static FAutoConsoleCommand CVarDLAnimationPrevTarget(
TEXT("DL.Animation.DebugView.PrevTarget")
, TEXT("切换到前一个调试目标")
, FConsoleCommandDelegate::CreateLambda(
	[]()
	{
		auto* Ins = AnimationDebugCompManager::Get();
		if (Ins)
		{
			const int32 Index = (Ins->GetCurIndex() - 1) % Ins->RegisteredDLAnimationDebugComps.Num();
			Ins->OpenDebugView(Index);
		}
	})
, ECVF_Cheat);

static FAutoConsoleCommand CVarDLAnimationNextTarget(
TEXT("DL.Animation.DebugView.NextTarget")
, TEXT("切换到后一个调试目标")
, FConsoleCommandDelegate::CreateLambda(
	[]()
	{
		auto* Ins = AnimationDebugCompManager::Get();
		if (Ins)
		{
			const int32 Index = (Ins->GetCurIndex() + 1) % Ins->RegisteredDLAnimationDebugComps.Num();
			Ins->OpenDebugView(Index);
		}
	})
, ECVF_Cheat);

// Sets default values for this component's properties
UDLAnimationDebugComponent::UDLAnimationDebugComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
#if !UE_BUILD_SHIPPING
	PrimaryComponentTick.bCanEverTick = true;

#else
	PrimaryComponentTick.bCanEverTick = false;
#endif

	// ...
}


// Called when the game starts
void UDLAnimationDebugComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...


#if !UE_BUILD_SHIPPING
	if (GetOwner()->GetLocalRole() == ENetRole::ROLE_Authority)
	{
		return;
	}

	const bool bImplements = GetOwner()->Implements<UDLAnimDebugInterface>();
	if (!bImplements)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s角色有动画调试控件，却没有实现IDLAnimDebugInterface"), *GetOwner()->GetName());
		SetComponentTickEnabled(false);
		return;
	}

	auto* Ins = AnimationDebugCompManager::Get();
	if (Ins)
	{
		Ins->RegisteredDebugComp(this);

		if (GetDebugTarget())
		{
			SetDynamicMaterials();
			SetResetColors();
			ACharacter* Character = Cast<ACharacter>(GetOwner());
			if (Character)
			{
				AddTickPrerequisiteActor(GetOwner());
				AddTickPrerequisiteComponent(Character->GetMesh());
			}
		}
	}
#endif

}


// Called every frame
void UDLAnimationDebugComponent::TickComponent(float DeltaTime, ELevelTick TickType,
											   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
#if !UE_BUILD_SHIPPING


	if (!GetOwner())
	{
		return;
	}

	auto* Ins = AnimationDebugCompManager::Get();

	if (Ins && Ins->GetIsOpened(this))
	{
		DrawDebugSpheres();
		UpdateDebugView();
	}
#endif
}

void UDLAnimationDebugComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	// LastDebugValue = false;
	auto* Ins = AnimationDebugCompManager::Get();
	if (Ins)
	{
		Ins->UnRegisterDebugComp(this);
	}
}

UDLAnimationDebugComponent* UDLAnimationDebugComponent::GetCurrentActiveAnimationDebugComp()
{
	auto* Ins = AnimationDebugCompManager::Get();
	if (Ins)
	{
		return Ins->GetCurDebugComp();
	}
	return nullptr;
}

TScriptInterface<IDLAnimDebugInterface> UDLAnimationDebugComponent::GetDebugTarget() const
{
	return TScriptInterface<IDLAnimDebugInterface>(GetOwner());;
}

