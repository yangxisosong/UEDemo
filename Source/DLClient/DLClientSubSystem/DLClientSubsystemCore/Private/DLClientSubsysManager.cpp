// Fill out your copyright notice in the Description page of Project Settings.


#include "DLClientSubsysManager.h"

#include "AppFrameworkMessageDef.h"
#include "DLClientSubsystemBase.h"
#include "DLUIManagerSubsystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameplayMessageTypes2.h"
#include "Kismet/GameplayStatics.h"
#include "DLClientSubsystemLog.h"

void FClientSubsystemCollection::CollectionAllSubsystemClass(const TArray<FDirectoryPath>& BPSystemPaths)
{
	TArray<UClass*> Classes;
	GetDerivedClasses(UDLClientSubsystemBase::StaticClass(), Classes, true);

	for (const auto Class : Classes)
	{
		if (!Class->HasAllClassFlags(CLASS_Abstract))
		{
			SubsystemClasses.Add(Class);
		}
	}

	if (BPSystemPaths.Num() > 0)
	{
		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked< FAssetRegistryModule >(FName("AssetRegistry"));
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		TArray<FString> Paths;

		for (const auto& P : BPSystemPaths)
		{
			Paths.Add(P.Path);
		}
		AssetRegistry.ScanPathsSynchronous(Paths);

		for (const auto& Path : Paths)
		{
			TArray<FAssetData> Data;
			AssetRegistry.GetAssetsByPath(*Path, Data);
			for (const auto& Asset : Data)
			{
				static FName NameParentClass = TEXT("NativeParentClass");
				FString ParentClassPath;
				Asset.GetTagValue(NameParentClass, ParentClassPath);
				TSoftClassPtr<UDLClientSubsystemBase>Class(ParentClassPath);
				if (Class.LoadSynchronous())
				{
					const UBlueprint* BP = Cast<UBlueprint>(Asset.GetAsset());
					if (BP)
					{
						if (BP->GeneratedClass)
						{
							SubsystemClasses.Add(BP->GeneratedClass);
						}
					}
				}
			}
		}
	}


	WaitInitClass = SubsystemClasses;
}

UDLClientSubsystemBase* FClientSubsystemCollection::CreateAndUpdateInfo(UClass* const& Class)
{
	const auto Ins = CreateInstance(Class);
	ensureAlwaysMsgf(Ins, TEXT("不应该是创建空的系统"));
	if (Ins)
	{
		TypeMapInstance.Add(Class, Ins);

		WaitInitClass.Remove(Class);
		AlreadyInitClass.Add(Class);

		UE_LOG(LogDLCSubsystem, Log, TEXT("Create Client Subsystem %s"), *GetNameSafe(Ins));

		return Ins;
	}
	return nullptr;
}

void FClientSubsystemCollection::TryCreateSubsystemIns(const EDLClientRuntimeStage Stage)
{
	static TArray<UClass*> TempClasses;
	TempClasses.Empty();

	TGuardValue<EDLClientRuntimeStage> GuardStage(CreateInsStage, Stage);

	// Copy
	TempClasses = WaitInitClass;

	FDLClientSubsysCustomLifeArg CustomArg;
	CustomArg.CurrentStage = Stage;
	CustomArg.Manager = Manager;

	for (const auto& Class : TempClasses)
	{
		// 如果存在依赖，那么可能遍历到这个Class 已经创建了
		if (AlreadyInitClass.Contains(Class))
		{
			continue;
		}

		const auto CDO = Class->GetDefaultObject<UDLClientSubsystemBase>();
		if ((CDO->InitStage == Stage) ||
			(CDO->InitStage == EDLClientRuntimeStage::None && CDO->CustomShouldCreateInstance(CustomArg)))
		{
			this->CreateAndUpdateInfo(Class);
			continue;
		}
	}
}

void FClientSubsystemCollection::TryDestroySubsystemIns(EDLClientRuntimeStage Stage)
{
	static TArray<UClass*> TempClasses;
	TempClasses.Empty();

	TGuardValue<EDLClientRuntimeStage> GuardStage(CreateInsStage, Stage);

	// Copy
	TempClasses = AlreadyInitClass;

	FDLClientSubsysCustomLifeArg CustomArg;
	CustomArg.CurrentStage = Stage;
	CustomArg.Manager = Manager;

	for (const auto& Class : TempClasses)
	{
		const auto CDO = Class->GetDefaultObject<UDLClientSubsystemBase>();
		if ((CDO->DestroyStage == Stage) ||
			(CDO->DestroyStage == EDLClientRuntimeStage::None && CDO->CustomShouldDestroyInstance(CustomArg)))
		{
			const auto Ins = TypeMapInstance.FindChecked(Class);
			Ins->UninitSubsystem();
			TypeMapInstance.Remove(Class);
			AlreadyInitClass.Remove(Class);

			UE_LOG(LogDLCSubsystem, Log, TEXT("Destroy Client Subsystem %s"), *GetNameSafe(Ins));

			continue;
		}
	}
}

void FClientSubsystemCollection::Init(UDLClientSubsysManager* InManager, const TArray<FDirectoryPath>& BPSystemPaths)
{
	Manager = InManager;

	this->CollectionAllSubsystemClass(BPSystemPaths);
}

UDLClientSubsystemBase* FClientSubsystemCollection::InitializeDependency(
	const TSubclassOf<UDLClientSubsystemBase> SubsystemClass)
{
	if (!ensureAlwaysMsgf(CreateInsStage != EDLClientRuntimeStage::None, TEXT("不应该是空的")))
	{
		return nullptr;
	}

	FDLClientSubsysCustomLifeArg CustomArg;
	CustomArg.CurrentStage = CreateInsStage;
	CustomArg.Manager = Manager;

	const auto CDO = SubsystemClass->GetDefaultObject<UDLClientSubsystemBase>();
	if ((CDO->InitStage == CreateInsStage) ||
		(CDO->InitStage == EDLClientRuntimeStage::None && CDO->CustomShouldCreateInstance(CustomArg)))
	{
		return this->CreateAndUpdateInfo(SubsystemClass);
	}

	ensureAlwaysMsgf(false, TEXT("无法创建实例，不满足创建的条件"));
	return nullptr;
}

UDLClientSubsystemBase* FClientSubsystemCollection::CreateInstance(const UClass* ClassType) const
{
	if (ClassType != nullptr)
	{
		const auto Ins = NewObject<UDLClientSubsystemBase>(Manager, ClassType);
		Ins->InitSubsystem(*this);
		return Ins;
	}
	return nullptr;
}


bool UDLClientSubsysManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		return true;
	}
	return false;
}

void UDLClientSubsysManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 可以预见的是，子系统会大量的依赖 UI模块，所以确保改系统已经启动
	const auto UIManager = Collection.InitializeDependency<UDLUIManagerSubsystem>();
	ensureAlwaysMsgf(UIManager, TEXT("UI 系统一定能够启动"));


	UGameplayMessageSubsystem* MsgSystem = Collection.InitializeDependency<UGameplayMessageSubsystem>();
	ensureAlwaysMsgf(MsgSystem, TEXT("MsgSystem 系统一定能够启动"));


	SubSysCollection.Init(this, BPSubsystemLoadDirectoryArray);

	if (MsgSystem)
	{
		TWeakObjectPtr<UDLClientSubsysManager> WeakObject(this);
		auto CallMegHandle = [WeakObject](const FGameplayTag Channel, const FDLAppFrameworkMsg& Payload)
		{
			if (UDLClientSubsysManager* StrongObject = WeakObject.Get())
			{
				StrongObject->ProcessAppFrameworkMsg(Channel, Payload);
			}
		};

		for (const auto& ChannelTag : ListenerMsgChannelTags)
		{
			auto&& Handle = MsgSystem->RegisterListener<FDLAppFrameworkMsg>(ChannelTag.Key, CallMegHandle);
			ListenerHandles.Add(Handle);
		}
	}
}

void UDLClientSubsysManager::Deinitialize()
{
	for (auto& Handle : ListenerHandles)
	{
		Handle.Unregister();
	}
	ListenerHandles.Empty();

	SubSysCollection.TryDestroySubsystemIns(EDLClientRuntimeStage::None);

	Super::Deinitialize();
}

void UDLClientSubsysManager::InjectClientRuntimeStage(EDLClientRuntimeStage Stage)
{
	SubSysCollection.TryCreateSubsystemIns(Stage);

	SubSysCollection.TryDestroySubsystemIns(Stage);
}

UDLClientSubsysManager* UDLClientSubsysManager::Get(const UObject* WorldContext)
{
	if (const auto Ins = UGameplayStatics::GetGameInstance(WorldContext))
	{
		return UGameInstance::GetSubsystem<UDLClientSubsysManager>(Ins);
	}
	return nullptr;
}

void UDLClientSubsysManager::ProcessAppFrameworkMsg(const FGameplayTag Channel, const FDLAppFrameworkMsg& Msg)
{
	const auto Stage = ListenerMsgChannelTags.Find(Channel);
	if (Stage)
	{
		InjectClientRuntimeStage(*Stage);
	}
}
