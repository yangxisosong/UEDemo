#include "DLUIManagerSubsystem.h"

#include "DLLocalPlayer.h"
#include "DLUIPolicyBase.h"
#include "Kismet/GameplayStatics.h"


#include "IDLLoadingProcessor.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

UDLUIManagerSubsystem* UDLUIManagerSubsystem::Get(UObject* WorldContext)
{
	auto GI = UGameplayStatics::GetGameInstance(WorldContext);
	if (GI)
	{
		return GI->GetSubsystem<UDLUIManagerSubsystem>();
	}
	return nullptr;
}

void UDLUIManagerSubsystem::RegisterLoadingProcessor(TScriptInterface<IDLLoadingProcessor> Processor)
{
	LoadingProcessors.AddUnique(Processor);

	LoadingProcessors.Sort([](const TScriptInterface<IDLLoadingProcessor>& A, const TScriptInterface<IDLLoadingProcessor>& B)
	{
		return A->GetPriority() > B->GetPriority();
	});
}

void UDLUIManagerSubsystem::UnRegisterLoadingProcessor(TScriptInterface<IDLLoadingProcessor> Processor)
{
	LoadingProcessors.Remove(Processor);

	if (Processor == LoadingProcessor)
	{
		auto UI = LoadingProcessor->GetOrCreateLoadingUI();

		// 可以极限情况关闭 PIE 时，GameViewport 已经空了
		if (IsValid(GEngine->GameViewport))
		{
			GEngine->GameViewport->RemoveViewportWidgetContent(UI);
		}

		LoadingProcessor = nullptr;
	}

}

TScriptInterface<IDLPrimaryLayout> UDLUIManagerSubsystem::GetPrimaryLayout()
{
	if (PolicyInstance)
	{
		return PolicyInstance->GetPrimaryLayout();
	}
	return nullptr;
}

UDLUIExtensionSystemInterface* UDLUIManagerSubsystem::GetUIExtensionSystem()
{
	if(UIExtensionSystem)
	{
		return UIExtensionSystem;
	}
	return nullptr;
}

void UDLUIManagerSubsystem::NotifyPrimaryLayoutReady() const
{
	OnPrimaryLayoutReady.Broadcast();
}

void UDLUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const auto GI = GetGameInstance();
	AddLocalPlayerHandle = GI->OnLocalPlayerAddedEvent.AddUObject(this, &ThisClass::OnLocalPlayerAdd);
	RemoveLocalPlayerHandle = GI->OnLocalPlayerRemovedEvent.AddUObject(this, &ThisClass::OnLocalPlayerRemove);

	checkfSlow(UIExtensionSystemClass, TEXT("没有配置UI扩展类"));
	UIExtensionSystem = NewObject<UDLUIExtensionSystemInterface>(this, UIExtensionSystemClass.LoadSynchronous());

	checkfSlow(UIPolicyClass, TEXT("没有配置UI策略，没得救了"));
	PolicyInstance = NewObject<UDLUIPolicyBase>(this, UIPolicyClass.LoadSynchronous());
	PolicyInstance->Init(this);
}

void UDLUIManagerSubsystem::Deinitialize()
{
	const auto GI = GetGameInstance();
	GI->OnLocalPlayerAddedEvent.Remove(AddLocalPlayerHandle);
	GI->OnLocalPlayerRemovedEvent.Remove(RemoveLocalPlayerHandle);

	Super::Deinitialize();
}


bool UDLUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);
		return ChildClasses.Num() == 0;
	}
	return false;
}


void UDLUIManagerSubsystem::Tick(float DeltaTime)
{
	auto CopyLoadingProcessors = LoadingProcessors;
	for (const auto& Process : CopyLoadingProcessors)
	{
		FString DebugStr;
		if(Process->IsLoading(DebugStr))
		{
			if (LoadingProcessor == nullptr)
			{
				LoadingProcessor = Process;

				auto UI = Process->GetOrCreateLoadingUI();
				GEngine->GameViewport->AddViewportWidgetContent(UI, 99999); 
				Process->OnShowLoadingUI();
			}
		}
		else
		{
			if (LoadingProcessor)
			{
				LoadingProcessor->OnRemoveLoadingUI();
				auto UI = LoadingProcessor->GetOrCreateLoadingUI();
				GEngine->GameViewport->RemoveViewportWidgetContent(UI);
				LoadingProcessor = nullptr;
			}
		}
	}
}

void UDLUIManagerSubsystem::OnLocalPlayerAdd(ULocalPlayer* LocalPlayer)
{
	if (const auto P = Cast<UDLLocalPlayer>(LocalPlayer))
	{
		PolicyInstance->OnLocalPlayerReady(P);
	}
}

void UDLUIManagerSubsystem::OnLocalPlayerRemove(ULocalPlayer* LocalPlayer)
{
	if (const auto P = Cast<UDLLocalPlayer>(LocalPlayer))
	{
		PolicyInstance->OnLocalPlayerRemove(P);
	}
}
