// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDLPrimaryLayout.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "UObject/Object.h"
#include "UDLPrimaryLayoutBase.generated.h"


class UCanvas;
/**
 * 这里具体实现 PrimaryLayout
 */
UCLASS()
class DLUICORE_API UDLPrimaryLayoutBase
	: public UUserWidget
	, public IDLPrimaryLayout
{
	GENERATED_BODY()
public:

	virtual UUserWidget* AddWidgetToLayout(FGameplayTag LayoutTag, TSubclassOf<UUserWidget> WidgetClass) override;

	virtual void RemoveWidgetToLayout(FGameplayTag LayoutTag, UUserWidget* WidgetIns) override;

protected:

	UFUNCTION(BlueprintCallable)
		void RegisterLayout(FGameplayTag LayoutTag, UOverlay* Widget);

private:

	UPROPERTY()
		TMap<FGameplayTag, UOverlay*> LayoutMap;
};
