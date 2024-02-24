#include "STargetSelectorViewport.h"

#include "EditorViewportCommands.h"
#include "SViewportToolBar.h"
#include "TargetSelectorEditor.h"
#include "TargetSelectorViewportClient.h"
#include "Editor/UnrealEd/Public/SEditorViewportToolBarMenu.h"
#include "Editor/UnrealEd/Public/STransformViewportToolbar.h"

class SEditorViewportToolBar : public SViewportToolBar
{
public:
	SLATE_BEGIN_ARGS(SEditorViewportToolBar) {}
	SLATE_ARGUMENT(TWeakPtr<STargetSelectorViewport>, EditorViewport)
		SLATE_END_ARGS()

		/** Constructs this widget with the given parameters */
		void Construct(const FArguments& InArgs)
	{
		EditorViewport = InArgs._EditorViewport;

		static const FName DefaultForegroundName("DefaultForeground");

		this->ChildSlot
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("NoBorder"))
			.ColorAndOpacity(this, &SViewportToolBar::OnGetColorAndOpacity)
			.ForegroundColor(FEditorStyle::GetSlateColor(DefaultForegroundName))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f, 2.0f)
			[
				SNew(SEditorViewportToolbarMenu)
				.ParentToolBar(SharedThis(this))
			.Cursor(EMouseCursor::Default)
			.Image("EditorViewportToolBar.MenuDropdown")
			.OnGetMenuContent(this, &SEditorViewportToolBar::GeneratePreviewMenu)
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f, 2.0f)
			[
				SNew(SEditorViewportToolbarMenu)
				.ParentToolBar(SharedThis(this))
			.Cursor(EMouseCursor::Default)
			.Label(this, &SEditorViewportToolBar::GetCameraMenuLabel)
			.LabelIcon(this, &SEditorViewportToolBar::GetCameraMenuLabelIcon)
			.OnGetMenuContent(this, &SEditorViewportToolBar::GenerateCameraMenu)
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f, 2.0f)
			[
				SNew(SEditorViewportToolbarMenu)
				.ParentToolBar(SharedThis(this))
			.Cursor(EMouseCursor::Default)
			.Label(this, &SEditorViewportToolBar::GetViewMenuLabel)
			.LabelIcon(this, &SEditorViewportToolBar::GetViewMenuLabelIcon)
			.OnGetMenuContent(this, &SEditorViewportToolBar::GenerateViewMenu)
			]
		+ SHorizontalBox::Slot()
			.Padding(3.0f, 1.0f)
			.HAlign(HAlign_Right)
			[
				SNew(STransformViewportToolBar)
				.Viewport(EditorViewport.Pin().ToSharedRef())
			.CommandList(EditorViewport.Pin()->GetCommandList())
			]
			]
			];

		SViewportToolBar::Construct(SViewportToolBar::FArguments());
	}

	/** Creates the preview menu */
	TSharedRef<SWidget> GeneratePreviewMenu() const
	{
		const TSharedPtr<const FUICommandList> CommandList = EditorViewport.IsValid() ? EditorViewport.Pin()->GetCommandList() : nullptr;

		constexpr bool bInShouldCloseWindowAfterMenuSelection = true;

		FMenuBuilder PreviewOptionsMenuBuilder(bInShouldCloseWindowAfterMenuSelection, CommandList);
		{
			PreviewOptionsMenuBuilder.BeginSection("BlueprintEditorPreviewOptions", NSLOCTEXT("BlueprintEditor", "PreviewOptionsMenuHeader", "Preview Viewport Options"));
			{
				PreviewOptionsMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().ToggleRealTime);
			}
			PreviewOptionsMenuBuilder.EndSection();
		}

		return PreviewOptionsMenuBuilder.MakeWidget();
	}

	FText GetCameraMenuLabel() const
	{
		if (EditorViewport.IsValid())
		{
			return GetCameraMenuLabelFromViewportType(EditorViewport.Pin()->GetViewportClient()->GetViewportType());
		}

		return NSLOCTEXT("BlueprintEditor", "CameraMenuTitle_Default", "Camera");
	}

	const FSlateBrush* GetCameraMenuLabelIcon() const
	{
		if (EditorViewport.IsValid())
		{
			return GetCameraMenuLabelIconFromViewportType(EditorViewport.Pin()->GetViewportClient()->GetViewportType());
		}

		return FEditorStyle::GetBrush(NAME_None);
	}

	TSharedRef<SWidget> GenerateCameraMenu() const
	{
		const TSharedPtr<const FUICommandList> CommandList = EditorViewport.IsValid() ? EditorViewport.Pin()->GetCommandList() : nullptr;

		constexpr bool bInShouldCloseWindowAfterMenuSelection = true;
		FMenuBuilder CameraMenuBuilder(bInShouldCloseWindowAfterMenuSelection, CommandList);

		CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Perspective);

		CameraMenuBuilder.BeginSection("LevelViewportCameraType_Ortho", NSLOCTEXT("BlueprintEditor", "CameraTypeHeader_Ortho", "Orthographic"));
		CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Top);
		CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Bottom);
		CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Left);
		CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Right);
		CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Front);
		CameraMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().Back);
		CameraMenuBuilder.EndSection();

		return CameraMenuBuilder.MakeWidget();
	}

	FText GetViewMenuLabel() const
	{
		FText Label = NSLOCTEXT("BlueprintEditor", "ViewMenuTitle_Default", "View");

		if (EditorViewport.IsValid())
		{
			switch (EditorViewport.Pin()->GetViewportClient()->GetViewMode())
			{
			case VMI_Lit:
				Label = NSLOCTEXT("BlueprintEditor", "ViewMenuTitle_Lit", "Lit");
				break;

			case VMI_Unlit:
				Label = NSLOCTEXT("BlueprintEditor", "ViewMenuTitle_Unlit", "Unlit");
				break;

			case VMI_BrushWireframe:
				Label = NSLOCTEXT("BlueprintEditor", "ViewMenuTitle_Wireframe", "Wireframe");
				break;
			default: ;
			}
		}

		return Label;
	}

	const FSlateBrush* GetViewMenuLabelIcon() const
	{
		static FName LitModeIconName("EditorViewport.LitMode");
		static FName UnlitModeIconName("EditorViewport.UnlitMode");
		static FName WireframeModeIconName("EditorViewport.WireframeMode");

		FName Icon = NAME_None;

		if (EditorViewport.IsValid())
		{
			switch (EditorViewport.Pin()->GetViewportClient()->GetViewMode())
			{
			case VMI_Lit:
				Icon = LitModeIconName;
				break;

			case VMI_Unlit:
				Icon = UnlitModeIconName;
				break;

			case VMI_BrushWireframe:
				Icon = WireframeModeIconName;
				break;
			default: ;
			}
		}

		return FEditorStyle::GetBrush(Icon);
	}

	TSharedRef<SWidget> GenerateViewMenu() const
	{
		const TSharedPtr<const FUICommandList> CommandList = EditorViewport.IsValid() ? EditorViewport.Pin()->GetCommandList() : nullptr;

		constexpr bool bInShouldCloseWindowAfterMenuSelection = true;
		FMenuBuilder ViewMenuBuilder(bInShouldCloseWindowAfterMenuSelection, CommandList);

		ViewMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().LitMode, NAME_None, NSLOCTEXT("BlueprintEditor", "LitModeMenuOption", "Lit"));
		ViewMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().UnlitMode, NAME_None, NSLOCTEXT("BlueprintEditor", "UnlitModeMenuOption", "Unlit"));
		ViewMenuBuilder.AddMenuEntry(FEditorViewportCommands::Get().WireframeMode, NAME_None, NSLOCTEXT("BlueprintEditor", "WireframeModeMenuOption", "Wireframe"));

		return ViewMenuBuilder.MakeWidget();
	}

private:
	/** Reference to the parent viewport */
	TWeakPtr<STargetSelectorViewport> EditorViewport;
};


void STargetSelectorViewport::Construct(const FArguments& InArgs)
{
	TargetSelectorEditor = InArgs._TargetSelectorEditor;

	SEditorViewport::Construct(SEditorViewport::FArguments());
}

TSharedRef<FEditorViewportClient> STargetSelectorViewport::MakeEditorViewportClient()
{
	FPreviewScene* PreviewScene = TargetSelectorEditor.Pin()->GetPreviewScene();
	
	ViewportClient = MakeShareable(new FTargetSelectorViewportClient(TargetSelectorEditor, PreviewScene, SharedThis(this)));
	ViewportClient->SetRealtime(true);
	ViewportClient->bSetListenerPosition = false;
	ViewportClient->VisibilityDelegate.BindSP(this, &STargetSelectorViewport::IsVisible);
	ViewportClient->SetViewLocation(FVector(-100,0,100));
	return ViewportClient.ToSharedRef();
}

TSharedPtr<SWidget> STargetSelectorViewport::MakeViewportToolbar()
{
	return
		SNew(SEditorViewportToolBar)
		.EditorViewport(SharedThis(this))
		.IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute());
}

void STargetSelectorViewport::PopulateViewportOverlays(TSharedRef<SOverlay> Overlay)
{
	SEditorViewport::PopulateViewportOverlays(Overlay);
}

void STargetSelectorViewport::BindCommands()
{
	SEditorViewport::BindCommands();
}

void STargetSelectorViewport::OnCycleWidgetMode()
{
	SEditorViewport::OnCycleWidgetMode();
}

void STargetSelectorViewport::OnCycleCoordinateSystem()
{
	SEditorViewport::OnCycleCoordinateSystem();
}


