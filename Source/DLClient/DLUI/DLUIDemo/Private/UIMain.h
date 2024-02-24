#pragma once

#include "CoreMinimal.h"
#include "ADLHUD.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "UIMain.generated.h"

UCLASS(Blueprintable)
class UDLPlayerAttrBarWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintImplementableEvent)
		void OnValueChange(float Current, float Max);

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayAttribute MaxValueAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayAttribute CurrentValueAttribute;
};



UCLASS()
class UDLGameplayUIMain
	: public UDLGameplayUIMainBase
{

	GENERATED_BODY()

protected:

	virtual void OnInit() override;

private:

	void BindAttrBarWidget(TScriptInterface<ICharacterStateAccessor> Accessor, ICharacterStateListener* Listener, UDLPlayerAttrBarWidget* Widget);

public:

	UFUNCTION(BlueprintImplementableEvent)
		void OnHeathChange(float CurrentHeath, float MaxHeath);

	UFUNCTION(BlueprintImplementableEvent)
		void OnManaChange(float CurrentHeath, float MaxHeath);

	UFUNCTION(BlueprintImplementableEvent)
		void OnEnterGame();

	UFUNCTION(BlueprintImplementableEvent)
		void OnKeyBoardDown(FKey Key);
};


