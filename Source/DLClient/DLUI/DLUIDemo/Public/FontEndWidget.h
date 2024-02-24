#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IFrontEndInterface.h"
#include "FontEndWidget.generated.h"

class UDLCFrontEndSubsystem;
UCLASS()
class DLUIDEMO_API UDLFontEndWidget :
	public UUserWidget,
	public IDLIFrontEndInterface
{
	GENERATED_BODY()
public:
	//IDLIFrontEndInterface
	virtual void InitCurrentSubsystem(UDLCFrontEndSubsystem* Subsystem) override;
	//IDLIFrontEndInterface

	UFUNCTION(BlueprintCallable)
		void Click();
private:
	UDLCFrontEndSubsystem* FrontEndSubsystem = nullptr;
};
