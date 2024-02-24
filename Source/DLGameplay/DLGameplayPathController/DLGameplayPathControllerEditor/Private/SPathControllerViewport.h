#pragma once
#include "SEditorViewport.h"

#include "CoreMinimal.h"


class FPathControllerViewportClient;

class SPathControllerViewport
	: public SEditorViewport
{
public:
	SLATE_BEGIN_ARGS(SPathControllerViewport)
		{
		}

		SLATE_ARGUMENT(TWeakPtr<class FPathControllerEditor>, PathControllerEditor)
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
	TWeakPtr<class FPathControllerEditor> PathControllerEditor;
	TSharedPtr<FPathControllerViewportClient> ViewportClient;
};
