#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateTypes.h"

class FTargetSelectStyle
{
public:
	static void Initialize();
	
	static void Shutdown();
	
	static TSharedPtr<ISlateStyle> Get() { return StyleSetInstance; }

	static FName GetStyleSetName() { return TEXT("TargetSelectStyle"); }

protected:

	static void SetIcons();

	static FString InResources(const FString& RelativePath, const ANSICHAR* Extension);

	static FTextBlockStyle NormalText;

private:
	static TSharedPtr<FSlateStyleSet> StyleSetInstance;

};
