#pragma once
#include "SEditorViewport.h"

#include "CoreMinimal.h"


class FTargetSelectorViewportClient;

class STargetSelectorViewport
	: public SEditorViewport
{
public:
	SLATE_BEGIN_ARGS(STargetSelectorViewport)
		{
		}

		SLATE_ARGUMENT(TWeakPtr<class FTargetSelectorEditor>, TargetSelectorEditor)
	SLATE_END_ARGS()


public:
	void Construct(const FArguments& InArgs);

protected:
	virtual TSharedRef<class FEditorViewportClient> MakeEditorViewportClient() override;

	virtual TSharedPtr<class SWidget> MakeViewportToolbar() override;
	virtual void PopulateViewportOverlays(TSharedRef<class SOverlay> Overlay) override;
	virtual void BindCommands() override;
	virtual void OnCycleWidgetMode() override;
	virtual void OnCycleCoordinateSystem() override;

private:
	TWeakPtr<class FTargetSelectorEditor> TargetSelectorEditor;
	TSharedPtr<FTargetSelectorViewportClient> ViewportClient;
};
