#include "TargetSelectorEditorStyle.h"
#include "ISMSystemModule.h"

#include "Styling/SlateStyleRegistry.h"
#include "Styling/CoreStyle.h"
#include "Interfaces/IPluginManager.h"



#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)
#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FTargetSelectorEditorStyle::InResources( RelativePath, ".png" ), __VA_ARGS__ )

TSharedPtr<FSlateStyleSet> FTargetSelectorEditorStyle::StyleSetInstance = nullptr;
FTextBlockStyle FTargetSelectorEditorStyle::NormalText = FTextBlockStyle()
.SetFont(DEFAULT_FONT("Regular", FCoreStyle::RegularTextSize))
.SetColorAndOpacity(FSlateColor::UseForeground())
.SetShadowOffset(FVector2D::ZeroVector)
.SetShadowColorAndOpacity(FLinearColor::Black)
.SetHighlightColor(FLinearColor(0.02f, 0.3f, 0.0f));

static const FVector2D Icon16x16(16.0f, 16.0f);
static const FVector2D Icon20x20(20.0f, 20.0f);
static const FVector2D Icon32x32(32.0f, 32.0f);
static const FVector2D Icon40x40(40.0f, 40.0f);
static const FVector2D Icon128x128(128.0f, 128.0f);

void FTargetSelectorEditorStyle::Initialize()
{
	// Only init once.
	if (StyleSetInstance.IsValid())
	{
		return;
	}

	StyleSetInstance = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	StyleSetInstance->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
	StyleSetInstance->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	SetIcons();

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSetInstance.Get());
}

void FTargetSelectorEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSetInstance.Get());
	ensure(StyleSetInstance.IsUnique());
	StyleSetInstance.Reset();
}

void FTargetSelectorEditorStyle::SetGraphStyles()
{
}

void FTargetSelectorEditorStyle::SetIcons()
{
	StyleSetInstance->Set("ClassIcon.TargetSelector", new IMAGE_BRUSH(TEXT("Icons/TargetSelector_16"), Icon16x16));
	StyleSetInstance->Set("ClassThumbnail.TargetSelector", new IMAGE_BRUSH(TEXT("Icons/TargetSelector_128"), Icon128x128));
}

FString FTargetSelectorEditorStyle::InResources(const FString& RelativePath, const ANSICHAR* Extension)
{
	return (FPaths::ProjectDir() / TEXT("Resources") / RelativePath) + Extension;
}

#undef DEFAULT_FONT
#undef IMAGE_BRUSH