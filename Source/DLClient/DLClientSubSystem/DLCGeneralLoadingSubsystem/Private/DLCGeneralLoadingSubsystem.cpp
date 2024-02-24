// Fill out your copyright notice in the Description page of Project Settings.


#include "DLCGeneralLoadingSubsystem.h"

#include "DLUIManagerSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameStateBase.h"
#include "GameplayFramwork/DLGameState.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SConstraintCanvas.h"

#if UE_BUILD_SHIPPING

#define SET_DEBUG_REASON(Text)
#define SET_DEBUG_REASON_STR(Str)
#else

#define SET_DEBUG_REASON(Text) DebugReason = TEXT(##Text)
#define SET_DEBUG_REASON_STR(Str) DebugReason = Str

#endif



void UDLCGeneralLoadingSubsystem::Tick(float DeltaTime)
{
	NeedLoading = this->CheckNeedLoading();
}

bool UDLCGeneralLoadingSubsystem::IsLoading(FString& OutDebugLoadingReason) const
{
	OutDebugLoadingReason = DebugReason;
	return NeedLoading;
}

const FString& UDLCGeneralLoadingSubsystem::GetProcessorName() const
{
	static FString ProcessorName = TEXT("DLCGeneralLoadingSubsystem");
	return ProcessorName;
}

TSharedRef<SWidget> UDLCGeneralLoadingSubsystem::GetOrCreateLoadingUI()
{
	if (!LoadingWidget)
	{
		LoadingWidget = CreateWidget(GetGameInstance(), LoadingWidgetClass.LoadSynchronous());

		LoadingSWidget = LoadingWidget->TakeWidget();
	}

	return LoadingSWidget.ToSharedRef();
}

void UDLCGeneralLoadingSubsystem::OnShowLoadingUI()
{
	
}

void UDLCGeneralLoadingSubsystem::OnRemoveLoadingUI()
{

}

EDLLoadingProcessorPriority UDLCGeneralLoadingSubsystem::GetPriority() const
{
	return Normal;
}

bool UDLCGeneralLoadingSubsystem::CheckNeedLoading()
{
	SET_DEBUG_REASON("Unknown");

	const UGameInstance* GI = GetGameInstance();
	const FWorldContext* Context = GI->GetWorldContext();
	if (Context == nullptr)
	{
		SET_DEBUG_REASON("The game instance has a null WorldContext");
		return true;
	}

	UWorld* World = Context->World();
	if (World == nullptr)
	{
		SET_DEBUG_REASON("We have no world (FWorldContext's World() is null)");
		return true;
	}

	// GS 在战斗场景 是 Rep 下来的，需要等待数据就位
	const AGameStateBase* GameState = World->GetGameState<AGameStateBase>();
	if (GameState == nullptr)
	{
		SET_DEBUG_REASON("GameState hasn't yet replicated (it's null)");
		return true;
	}

	if (bCurrentlyInLoadMap)
	{
		SET_DEBUG_REASON_STR(FString::Printf(TEXT("Load Map %s"), *LoadingMapName));
		return true;
	}

	// 正在 Travel 
	if (!Context->TravelURL.IsEmpty())
	{
		SET_DEBUG_REASON("We have pending travel (the TravelURL is not empty)");
		return true;
	}

	// 正在连接 Server
	if (Context->PendingNetGame != nullptr)
	{
		SET_DEBUG_REASON("We are connecting to another server (PendingNetGame != nullptr)");
		return true;
	}

	if (!World->HasBegunPlay())
	{
		SET_DEBUG_REASON("World hasn't begun play");
		return true;
	}

	// 正在无缝加载
	if (World->IsInSeamlessTravel())
	{
		SET_DEBUG_REASON("We are in seamless travel");
		return true;
	}

	if (const auto GameplayGS = Cast<ADLGameState>(GameState))
	{
		if (!GameplayGS->ClientIsGameplayReady())
		{
			SET_DEBUG_REASON("Client  Gameplay  Not Ready");
			return true;
		}
	}

	SET_DEBUG_REASON("Loading Succeed ");

	return false;
}

void UDLCGeneralLoadingSubsystem::HandlePreLoadMap(const FString& String)
{
	//bCurrentlyInLoadMap = true;
	//LoadingMapName = String;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void UDLCGeneralLoadingSubsystem::HandlePostLoadMap(UWorld* World)
{
	//if (World->GetGameInstance() == GetGameInstance())
	//{
	//	bCurrentlyInLoadMap = false;
	//	LoadingMapName = {};
	//}
}

void UDLCGeneralLoadingSubsystem::InitSubsystem(const FClientSubsystemCollection& Collection)
{
	Super::InitSubsystem(Collection);

	PreLoadMapHandle = FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::HandlePreLoadMap);
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);

	auto UISys = GetGameInstance()->GetSubsystem<UDLUIManagerSubsystem>();
	UISys->RegisterLoadingProcessor(this);
}

void UDLCGeneralLoadingSubsystem::UninitSubsystem()
{
	FCoreUObjectDelegates::PreLoadMap.Remove(PreLoadMapHandle);
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);

	auto UISys = GetGameInstance()->GetSubsystem<UDLUIManagerSubsystem>();
	UISys->UnRegisterLoadingProcessor(this);

	LoadingSWidget = nullptr;

	Super::UninitSubsystem();
}
