#include "UIMain.h"

#include "AttributeSet/DLCharacterAttributeSet.h"
#include "Blueprint/WidgetTree.h"
#include "Interface/ICharacterStateAccessor.h"
#include "Interface/ICharacterStateListener.h"
#include "Interface/IPlayerAttributeAccessor.h"


void UDLGameplayUIMain::OnInit()
{
	Super::OnInit();

	// 初始化 Bar
	WidgetTree->ForEachWidget([this](UWidget* W)
	{
		if (W->GetClass()->IsChildOf<UDLPlayerAttrBarWidget>())
		{
			this->BindAttrBarWidget(CharacterStateAccessor, CharacterStateListener.operator->(), CastChecked<UDLPlayerAttrBarWidget>(W));
		}
	});
}

void UDLGameplayUIMain::BindAttrBarWidget(TScriptInterface<ICharacterStateAccessor> Accessor, ICharacterStateListener* Listener, UDLPlayerAttrBarWidget* Widget)
{
	auto CallChange = [](TScriptInterface<ICharacterStateAccessor> Accessor, UDLPlayerAttrBarWidget* Widget)
	{
		const float Max = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(Accessor.GetObject(), Widget->MaxValueAttribute);
		const float Current = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(Accessor.GetObject(), Widget->CurrentValueAttribute);
		Widget->OnValueChange(Current, Max);
	};

	auto Call = [this, CallChange, W = TWeakObjectPtr<UDLPlayerAttrBarWidget>(Widget), Accessor](const FOnAttributeChangeData& Data) {
		if (W.IsValid())
		{
			CallChange(Accessor, W.Get());
		}
	};

	Listener->OnGameplayAttributeValueChange(Widget->MaxValueAttribute).AddWeakLambda(this, Call);
	Listener->OnGameplayAttributeValueChange(Widget->CurrentValueAttribute).AddWeakLambda(this, Call);

	CallChange(Accessor, Widget);
}