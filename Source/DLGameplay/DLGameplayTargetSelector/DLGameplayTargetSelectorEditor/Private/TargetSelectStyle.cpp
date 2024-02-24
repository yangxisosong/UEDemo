// Copyright Recursoft LLC 2019-2022. All Rights Reserved.

#include "TargetSelectStyle.h"

#include "ISMSystemModule.h"

#include "Styling/SlateStyleRegistry.h"
#include "Styling/CoreStyle.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "SMEditorStyle"

// See SlateEditorStyle.cpp
#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)
#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FTargetSelectStyle::InResources( RelativePath, ".png" ), __VA_ARGS__ )

TSharedPtr<FSlateStyleSet> FTargetSelectStyle::StyleSetInstance = nullptr;
FTextBlockStyle FTargetSelectStyle::NormalText = FTextBlockStyle()
.SetFont(DEFAULT_FONT("Regular", FCoreStyle::RegularTextSize))
.SetColorAndOpacity(FSlateColor::UseForeground())
.SetShadowOffset(FVector2D::ZeroVector)
.SetShadowColorAndOpacity(FLinearColor::Black)
.SetHighlightColor(FLinearColor(0.02f, 0.3f, 0.0f));

static const FVector2D Icon16x16(16.0f, 16.0f);
static const FVector2D Icon20x20(20.0f, 20.0f);
static const FVector2D Icon32x32(32.0f, 32.0f);
static const FVector2D Icon40x40(40.0f, 40.0f);
static const FVector2D Icon64x64(64.0f, 64.0f);
static const FVector2D Icon128x128(128.0f, 128.0f);

void FTargetSelectStyle::Initialize()
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

void FTargetSelectStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSetInstance.Get());
	ensure(StyleSetInstance.IsUnique());
	StyleSetInstance.Reset();
}



void FTargetSelectStyle::SetIcons()
{
	StyleSetInstance->Set("ClassIcon.DLGameplayTargetSelectorCoordinate", new IMAGE_BRUSH(TEXT("Icons/TargetSelectorTransform_16"), Icon16x16));
	StyleSetInstance->Set("ClassThumbnail.DLGameplayTargetSelectorCoordinate", new IMAGE_BRUSH(TEXT("Icons/TargetSelectorTransform_128"), Icon128x128));
	StyleSetInstance->Set("ClassIcon.DLGameplayTargetSelectorShapeTraceBase", new IMAGE_BRUSH(TEXT("Icons/TargetSelectorUnit_16"), Icon16x16));
	StyleSetInstance->Set("ClassThumbnail.DLGameplayTargetSelectorShapeTraceBase", new IMAGE_BRUSH(TEXT("Icons/TargetSelectorUnit_128"), Icon128x128));

	StyleSetInstance->Set("ClassIcon.DLGameplayAbilityBase", new IMAGE_BRUSH(TEXT("Icons/GA"), Icon16x16));
	StyleSetInstance->Set("ClassThumbnail.DLGameplayAbilityBase", new IMAGE_BRUSH(TEXT("Icons/GA"), Icon128x128));
	
	StyleSetInstance->Set("ClassIcon.GameplayCueNotify_Static", new IMAGE_BRUSH(TEXT("Icons/GCS"), Icon16x16));
	StyleSetInstance->Set("ClassThumbnail.GameplayCueNotify_Static", new IMAGE_BRUSH(TEXT("Icons/GCS"), Icon128x128));
	
	StyleSetInstance->Set("ClassIcon.GameplayCueNotify_Actor", new IMAGE_BRUSH(TEXT("Icons/GCD"), Icon16x16));
	StyleSetInstance->Set("ClassThumbnail.GameplayCueNotify_Actor", new IMAGE_BRUSH(TEXT("Icons/GCD"), Icon128x128));
	
	StyleSetInstance->Set("ClassIcon.GameplayEffect", new IMAGE_BRUSH(TEXT("Icons/GE"), Icon16x16));
	StyleSetInstance->Set("ClassThumbnail.GameplayEffect", new IMAGE_BRUSH(TEXT("Icons/GE"), Icon128x128));
}

FString FTargetSelectStyle::InResources(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString ContentDir =  FPaths::ProjectDir() / TEXT("Resources");
	return (ContentDir / RelativePath) + Extension;
}

#undef DEFAULT_FONT
#undef IMAGE_BRUSH

#undef LOCTEXT_NAMESPACE
