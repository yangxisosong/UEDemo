#include "AbilityChainNodeEditorStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"

TSharedPtr< FSlateStyleSet > FAbilityChainNodeEditorStyle::StyleInstance = nullptr;

void FAbilityChainNodeEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FAbilityChainNodeEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FAbilityChainNodeEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FAbilityChainNodeEditorStyle::Get()
{
	return *StyleInstance;
}

FName FAbilityChainNodeEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("AbilityChainNodeEditorStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon64x64(64.0f, 64.0f);
const FVector2D Icon128x128(128.0f, 128.0f);

TSharedRef<FSlateStyleSet> FAbilityChainNodeEditorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("AbilityChainNodeEditorStyle"));


	Style->SetContentRoot(FPaths::GameSourceDir() + "Editor/DLAbilityChainEditor/Resources");

	Style->Set("AbilityChainNodeEditorStyle.FileToJson", new IMAGE_BRUSH(TEXT("json"), Icon40x40));
	Style->Set("AbilityChainNodeEditorStyle.NodeIcon", new IMAGE_BRUSH(TEXT("node-red"), Icon40x40));
	Style->Set("AbilityChainNodeEditorStyle.FindNode", new IMAGE_BRUSH(TEXT("find"), Icon40x40));

	Style->Set("ClassIcon.AbilityChainAsset", new IMAGE_BRUSH(TEXT("node-red"), Icon16x16));
	Style->Set("ClassThumbnail.AbilityChainAsset", new IMAGE_BRUSH(TEXT("node-red"), Icon128x128));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT