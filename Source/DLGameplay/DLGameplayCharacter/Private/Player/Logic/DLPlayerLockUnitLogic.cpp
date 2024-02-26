#include "DLPlayerLockUnitLogic.h"

#include "GameplayFramwork/DLCharacterBase.h"
#include "Interface/IDLLockableUnit.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DLPlayerController.h"
#include "Player/DLPlayerCharacter.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDLPlayerLockUnitLogic, Log, All)
DEFINE_LOG_CATEGORY(LogDLPlayerLockUnitLogic);


TAutoConsoleVariable<int32> CVarEnabledLockDebug(
	TEXT("DL.Gameplay.EnabledLockTargetDebug"),
	0,
	TEXT("0 : 关闭调试功能")
	TEXT("1 : 开启调试功能")
);

void UDLPlayerLockUnitLogic::Init(ADLPlayerController* PC)
{
	PlayerController = PC;
	bInit = true;

	const auto Character = PlayerController->GetPlayerCharacter();
	if (ensureAlwaysMsgf(Character, TEXT("一定需要在初始化角色后，初始化这里")))
	{
		Character->OnUnderAttackEvent.AddDynamic(this, &UDLPlayerLockUnitLogic::OnUnderAttack);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UDLPlayerLockUnitLogic::Tick(float Dt)
{
	if (bInit)
	{
		if (PlayerController && PlayerController->GetPlayerCharacter())
		{
			const auto Character = PlayerController->GetPlayerCharacter();
			if (Character->GetLockPoint())
			{
				// 检查当前是否合法
				if (!this->CheckLockPoint(PlayerController, Character, Character->GetLockPoint()))
				{
					UE_LOG(LogDLPlayerLockUnitLogic, Log, TEXT("CheckLockPoint 触发打断的条件"));
					this->UnLockUnit();
				}
			}
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
UDLLockPointComponent* UDLPlayerLockUnitLogic::FindBastLockPoint(const TArray<AActor*>& AlternativeActor)
{
	const bool Debug = static_cast<bool>(CVarEnabledLockDebug.GetValueOnGameThread());

	// 获取全部的备选的 Point
	TArray<UDLLockPointComponent*> LockPointComps;
	for (AActor* LockActor : AlternativeActor)
	{
		// 不锁定本地控制的Actor
		if (LockActor->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
		{
			continue;
		}

		const auto Interface = Cast<IDLLockableUnit>(LockActor);
		LockPointComps.Append(Interface->GetAllLockPoint());
	}

	const FVector SelfLocation = PlayerController->GetPlayerCharacter()->GetActorLocation();

	struct LockPointInfo
	{
		UDLLockPointComponent* CurrentMinComp = nullptr;
		float Len = 0.f;
		int32 Priority = 0.f;
	};


	TArray<LockPointInfo> TempLockPointInfos;

	// 剔除距离太远的单位
	for (const auto& LockUnit : LockPointComps)
	{
		if (LockUnit)
		{
			FVector Local = LockUnit->GetComponentLocation();

			const float Size = (Local - SelfLocation).SizeSquared2D();

			const auto& Config = LockUnit->GetLockableUnit()->GetLockableUnitConfig();

			if (Config.PlayerCanLockDistanceSquared <= Size)
			{
				continue;
			}

			TempLockPointInfos.Add({ LockUnit, Size, Config.BeLockedPriority });
		}
	}

	// 按距离 优先级 距离 排序，
	TempLockPointInfos.Sort([](const LockPointInfo& A, const LockPointInfo& B)
	{
		if (A.Priority == B.Priority)
		{
			return A.Len < B.Len;
		}
		return A.Priority > B.Priority;
	});


	// 在屏幕空间进行过滤
	TArray<UDLLockPointComponent*> SortPassSceneTest;

	const ULocalPlayer* const LocalPlayer = PlayerController->GetLocalPlayer();
	if (LocalPlayer && LocalPlayer->ViewportClient)
	{
		FSceneViewProjectionData ProjectionData;
		if (LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, /*out*/ ProjectionData))
		{
			FMatrix const ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();

			for (const auto LockComp : TempLockPointInfos)
			{
				const FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(LockComp.CurrentMinComp->GetComponentLocation(), 1.f));
				if (Result.W > 0.0f)
				{
					const float RHW = 1.0f / Result.W;
					const FPlane PosInScreenSpace = FPlane(Result.X * RHW, Result.Y * RHW, Result.Z * RHW, Result.W);

					const float NormalizedX = (PosInScreenSpace.X / 2.f) + 0.5f;
					const float NormalizedY = 1.f - (PosInScreenSpace.Y / 2.f) - 0.5f;

					if (NormalizedX > LockRectInSceneMin.X
						&& NormalizedX < LockRectInSceneMax.X
						&& NormalizedY > LockRectInSceneMin.Y
						&& NormalizedY < LockRectInSceneMax.Y)
					{
						SortPassSceneTest.Add(LockComp.CurrentMinComp);
					}
				}
			}
		}
	}

	if (Debug)
	{
		for (const auto& LockUnit : SortPassSceneTest)
		{
			UKismetSystemLibrary::DrawDebugBox(this, LockUnit->GetComponentLocation(), FVector(50.f), FLinearColor::Red, FRotator::ZeroRotator, 3.f, 2.f);
		}
	}

	UDLLockPointComponent* RetComp = nullptr;

	ADLPlayerCharacter* Character = PlayerController->GetPlayerCharacter();

	// 测试遮挡
	TArray<UDLLockPointComponent*> PassShieldTest;

	FVector EyeLocation;
	FRotator EyeRotator;
	Character->GetActorEyesViewPoint(EyeLocation, EyeRotator);

	for (const auto& LockUnit : SortPassSceneTest)
	{
		FHitResult HitRet;
		UKismetSystemLibrary::LineTraceSingle(
			Character,
			EyeLocation,
			LockUnit->GetComponentLocation(),
			static_cast<ETraceTypeQuery>(ECollisionChannel::ECC_Visibility),
			false,
			{},
			Debug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			HitRet,
			true
		);

		if ((!HitRet.GetActor()) || HitRet.GetActor() == LockUnit->GetOwner())
		{
			RetComp = LockUnit;
			break;
		}
	}

	// 回溯找是否距离达到锁定默认点的值
	bool IsUsedFixedPoint = false;
	if (RetComp)
	{
		auto Unit = RetComp->GetLockableUnit();
		const auto& Config = Unit->GetLockableUnitConfig();

		FVector Local = RetComp->GetOwner()->GetActorLocation();
		const float Size = (Local - SelfLocation).SizeSquared2D();
		if (Size > Config.PlayerCanFreeLockMaxDistanceSquared)
		{
			auto Temp = Unit->GetLockPointComponent(Config.FixedLockPointIndex);
			if (Temp)
			{
				RetComp = Temp;

				IsUsedFixedPoint = true;
			}
		}
	}


	if (Debug && RetComp)
	{
		UKismetSystemLibrary::DrawDebugString(this,
						RetComp->GetComponentLocation(),
						IsUsedFixedPoint ? TEXT("UsedFixedPoint") : TEXT("UsedBastPoint"),
						nullptr, FLinearColor::Red, 2.f
		);
	}

	return RetComp;
}

UWorld* UDLPlayerLockUnitLogic::GetWorld() const
{
	if (GetOuter())
	{
		return GetOuter()->GetWorld();
	}
	return nullptr;
}

bool UDLPlayerLockUnitLogic::TryLockUnit()
{
	// 获取支持锁定的对象 TODO 需要优化
	TArray<AActor*> AllLockActor;
	UGameplayStatics::GetAllActorsWithInterface(this, UDLLockableUnit::StaticClass(), AllLockActor);

	const auto RetComp = this->FindBastLockPoint(AllLockActor);

	if (RetComp)
	{
		UE_LOG(LogDLPlayerLockUnitLogic, Log, TEXT("Find Bast Lock Target %s->%s Index %d"),
			*GetNameSafe(RetComp->GetOwner()), *GetNameSafe(RetComp), RetComp->Index);

		if (ensureAlwaysMsgf(PlayerController->GetPlayerCharacter(), TEXT("PlayerCharacter 一定是有值的")))
		{
			PlayerController->GetPlayerCharacter()->ServerSetTargetLock(RetComp->GetOwner(), RetComp->Index);
			LockedPoint = RetComp;
		}

		return true;
	}

	UE_LOG(LogDLPlayerLockUnitLogic, Log, TEXT("Not Find Bast Lock Target "));
	return false;
}

bool UDLPlayerLockUnitLogic::TryLockNextUnit()
{
	const auto Interface = Cast<IDLLockableUnit>(LockedPoint->GetOwner());
	if (!Interface) return  false;

	TArray<UDLLockPointComponent*> LockPointComps;
	LockPointComps.Append(Interface->GetAllLockPoint());
	if (LockPointComps.Num() == 0) return false;

	//查找下一个
	auto newLockPoint = LockPointComps[0];
	int32 Index = LockPointComps.Find(LockedPoint);
	if (Index != LockPointComps.Num() - 1)
	{
		newLockPoint = LockPointComps[Index + 1];
	}
	if (!newLockPoint) return  false;

	if (ensureAlwaysMsgf(PlayerController->GetPlayerCharacter(), TEXT("PlayerCharacter 一定是有值的")))
	{
		PlayerController->GetPlayerCharacter()->ServerSetTargetLock(newLockPoint->GetOwner(), newLockPoint->Index);
		LockedPoint = newLockPoint;
	}
	return  true;

}

void UDLPlayerLockUnitLogic::ForceLockUnit(UDLLockPointComponent* Comp)
{

}

void UDLPlayerLockUnitLogic::UnLockUnit()
{
	if (PlayerController->GetPlayerCharacter())
	{
		PlayerController->GetPlayerCharacter()->ServerSetTargetLock(nullptr, INDEX_NONE);
		LockedPoint = nullptr;
	}
}

void UDLPlayerLockUnitLogic::OnUnderAttack(const FDLUnitUnderAttackInfo& Info)
{
	if (PlayerController->GetCharacterBase() && !PlayerController->GetCharacterBase()->IsLockTargetUnit())
	{
		const auto Comp = this->FindBastLockPoint({ Info.EffectCauser });

		UE_LOG(LogDLPlayerLockUnitLogic, Log, TEXT("UDLPlayerLockUnitLogic::OnUnderAttack EffectCauser %s  Find %s"),
							*GetNameSafe(Info.EffectCauser), *GetNameSafe(Comp));

		if (Comp)
		{
			this->ForceLockUnit(Comp);
		}
	}
}
