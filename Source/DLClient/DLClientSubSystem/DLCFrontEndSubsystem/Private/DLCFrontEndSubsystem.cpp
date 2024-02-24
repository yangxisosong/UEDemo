#include "DLCFrontEndSubsystem.h"

#include "AppFrameworkMessageDef.h"
#include "DLUIManagerSubsystem.h"
#include "IDLPrimaryLayout.h"
#include "Blueprint/UserWidget.h"
#include "IFrontEndInterface.h"
#include "GameFramework/GameplayMessageSubsystem.h"

//UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Message_Test);
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Message_Test, "UI.Message.Test");

void UDLCFrontEndSubsystem::InitSubsystem(const FClientSubsystemCollection& Collection)
{
	Super::InitSubsystem(Collection);

	UDLUIManagerSubsystem* UISystem = GetGameInstance()->GetSubsystem<UDLUIManagerSubsystem>();
	if (!UISystem)
	{
		return;
	}


	if (!UISystem->GetPrimaryLayout())
	{
		UISystem->OnPrimaryLayoutReady.AddUObject(this, &ThisClass::OnPrimaryLayoutReady);
	}
	else
	{
		this->OnPrimaryLayoutReady();
	}
}

void UDLCFrontEndSubsystem::UninitSubsystem()
{
	// 移除前端的 UI
	UDLUIManagerSubsystem* UISystem = GetGameInstance()->GetSubsystem<UDLUIManagerSubsystem>();
	if (!UISystem)
	{
		return;
	}

	const auto PrimaryLayout = UISystem->GetPrimaryLayout();
	if (PrimaryLayout)
	{
		const FString TagName = "UI.Layout.GameNormal";
		FGameplayTag Tag;
		Tag.FromExportString(TagName);
		PrimaryLayout->RemoveWidgetToLayout(Tag, Cast<UUserWidget>(Widget));
	}


	Super::UninitSubsystem();
}


void UDLCFrontEndSubsystem::OnPrimaryLayoutReady()
{
	UDLUIManagerSubsystem* UISystem = GetGameInstance()->GetSubsystem<UDLUIManagerSubsystem>();
	if (!UISystem)
	{
		return;
	}

	UClass* MainWidgetClass = WidgetClass.LoadSynchronous();
	if (!ensureAlwaysMsgf(MainWidgetClass, TEXT("一定是忘记配置界面了")))
	{
		return;
	}

	// 创建前端的 UI 到 Layout
	const auto PrimaryLayout = UISystem->GetPrimaryLayout();
	if (PrimaryLayout)
	{
		const FGameplayTag Tag = FGameplayTag::RequestGameplayTag("UI.Layout.GameNormal");
		Widget = PrimaryLayout->AddWidgetToLayout(Tag, MainWidgetClass);
		if(IDLIFrontEndInterface* Interface = Cast<IDLIFrontEndInterface>(Widget))
		{
			Interface->InitCurrentSubsystem(this);
		}
	}

	// 显示鼠标
	GetPlayerControllerChecked()->SetShowMouseCursor(true);
}
