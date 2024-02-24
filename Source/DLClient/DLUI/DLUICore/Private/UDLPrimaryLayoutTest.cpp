#include "UDLPrimaryLayoutTest.h"

#include "Components/OverlaySlot.h"


UUserWidget* UDLPrimaryLayoutTest::AddWidgetToLayout(FGameplayTag LayoutTag, TSubclassOf<UUserWidget> WidgetClass)
{
	if (LayoutMap.Contains(LayoutTag))
	{
		if (WidgetClass)
		{
			// TODO 后续可能需要用对象池来做 

			const auto W = CreateWidget<UUserWidget>(this, WidgetClass);
			const auto MySlot = LayoutMap[LayoutTag]->AddChildToOverlay(W);
			MySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
			MySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);

			return W;
		}
	}
	return nullptr;
}

void UDLPrimaryLayoutTest::RemoveWidgetToLayout(FGameplayTag LayoutTag, UUserWidget* WidgetIns)
{
	// TODO 
}

void UDLPrimaryLayoutTest::RegisterLayout(FGameplayTag LayoutTag, UOverlay* Widget)
{
	LayoutMap.Add(LayoutTag, Widget);
}
