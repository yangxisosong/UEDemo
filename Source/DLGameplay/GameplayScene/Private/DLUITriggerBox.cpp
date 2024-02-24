#include"DLUITriggerBox.h"
#include "AppFrameworkMessageDef.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameplayMessageSubsystem.h"

namespace EDLGameplayTriggerMsg
{
	UE_DEFINE_GAMEPLAY_TAG(GameplayTrigger_ShowButtonTips, "DLMsgType.GameplayTrigger.ShowButtonTips");
	UE_DEFINE_GAMEPLAY_TAG(GameplayTrigger_HideButtonTips, "DLMsgType.GameplayTrigger.HideButtonTips");
	UE_DEFINE_GAMEPLAY_TAG(GameplayTrigger_ButtonClick, "DLMsgType.GameplayTrigger.ButtonClick");
}


void ADLUITriggerBox::BeginPlay()
{
	Super::BeginPlay();
}

ADLUITriggerBox::ADLUITriggerBox()
{
	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &ADLUITriggerBox::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ADLUITriggerBox::OnOverlapEnd);
}

void ADLUITriggerBox::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (Cast<ACharacter>(OtherActor) && OtherActor->GetGameInstance()->GetFirstGamePlayer())
	{
		const auto InputLocalPlayerSubsystem = OtherActor->GetGameInstance()->GetFirstGamePlayer()->
		                                                   GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

		if (InputLocalPlayerSubsystem)
		{
			InputLocalPlayerSubsystem->AddMappingContext(InputContext.LoadSynchronous(),
			                                             InputContextPriority);
		}
		IsInTriggerBox = true;

		UGameplayMessageSubsystem* MsgSystem = GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>();
		if (MsgSystem)
		{
			MsgSystem->BroadcastMessage(TriggerShowUITag, ButtonTipsMsgBody);
		}
	}
}

void ADLUITriggerBox::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	if (Cast<ACharacter>(OtherActor) && OtherActor->GetGameInstance()->GetFirstGamePlayer())
	{
		UGameplayMessageSubsystem* MsgSystem = GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>();

		const auto InputLocalPlayerSubsystem = OtherActor->GetGameInstance()->GetFirstGamePlayer()->
		                                                   GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (InputLocalPlayerSubsystem)
		{
			InputLocalPlayerSubsystem->RemoveMappingContext(InputContext.LoadSynchronous());
		}
		IsInTriggerBox = false;

		if (MsgSystem)
		{
			MsgSystem->BroadcastMessage(TriggerHideUITag, ButtonTipsMsgBody);
		}
	}
}

void ADLUITriggerBox::OnTriggerButton(FKey Key)
{
	FDLUITriggerButtonClickMsgBody Msg;
	Msg.Key = Key;
	UGameplayMessageSubsystem* MsgSystem = GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>();

	if (MsgSystem)
	{
		MsgSystem->BroadcastMessage(ButtonTrigger, Msg);
	}
}
