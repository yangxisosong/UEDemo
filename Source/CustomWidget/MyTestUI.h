#pragma once

#include "CoreMinimal.h"
#include "SSlateMyTest.h"
#include "Components/Widget.h"

#include "MyTestUI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickButton);

UCLASS()
class CUSTOMWIDGET_API UMyTestUI: public UWidget
{
	GENERATED_BODY()
public:
	
public:
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance")
	FSlateBrush Brush;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FSlateBrush Brush2;

public:
	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
		FOnClickButton OnClickTestUI;


	FReply OnClickOk();

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	TSharedPtr<SSlateMyTest> MySlice;
};
