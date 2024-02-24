// Fill out your copyright notice in the Description page of Project Settings.


#include "UDLUIPolicyForDemo.h"
#include "DLUIManagerSubsystem.h"
#include "DLLocalPlayer.h"
#include "UDLPrimaryLayoutBase.h"
#include "Blueprint/UserWidget.h"

TScriptInterface<IDLPrimaryLayout> UDLUIPolicyForDemo::GetPrimaryLayout()
{
	return PrimaryLayout;
}


void UDLUIPolicyForDemo::OnPrimaryLayoutReady(UDLLocalPlayer* LocalPlayer, APlayerController* PlayerController)
{
	if ((!PrimaryLayout) && PrimaryLayoutClass)
	{
		PrimaryLayout = CreateWidget<UDLPrimaryLayoutBase>(PlayerController, PrimaryLayoutClass);

		// 不使用 AddToViewPort  因为这个会被 World 管控Widget，我们更希望 更加自定义一些
		const auto GameViewport = LocalPlayer->GetWorld()->GetGameViewport();
		if (GameViewport && PrimaryLayout)
		{
			GameViewport->AddViewportWidgetContent(PrimaryLayout->TakeWidget(), 10);
			UIManager->NotifyPrimaryLayoutReady();
		}
	}
}

void UDLUIPolicyForDemo::OnLocalPlayerReady(UDLLocalPlayer* LocalPlayer)
{
	Super::OnLocalPlayerReady(LocalPlayer);

	LocalPlayer->CallAndRegister_OnPlayerControllerSet(
		UDLLocalPlayer::FPlayerControllerSetDelegate::FDelegate::CreateUObject(this,
		&UDLUIPolicyForDemo::OnPrimaryLayoutReady
		)
	);
}

void UDLUIPolicyForDemo::OnLocalPlayerRemove(UDLLocalPlayer* LocalPlayer)
{
	if (PrimaryLayout)
	{
		PrimaryLayout->RemoveFromViewport();
		PrimaryLayout = nullptr;
	}

	Super::OnLocalPlayerRemove(LocalPlayer);
}
