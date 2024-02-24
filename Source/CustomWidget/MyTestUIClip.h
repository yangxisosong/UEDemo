#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "SSlateMyTestClip.h"

#include "MyTestUIClip.generated.h"


UCLASS()
class CUSTOMWIDGET_API UMyTestUIClip: public UWidget
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickButton);
public:
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance")
	FSlateBrush Brush;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FSlateBrush Brush2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FSlateFontInfo m_Font;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FText m_Text;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float ClipNum=0;
public:
	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
	FOnClickButton OnClickTestUI;


	FReply OnClickOk();

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	TSharedPtr<SSlateMyTestClip> MySlice;
};
