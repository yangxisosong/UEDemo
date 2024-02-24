#pragma once
#include "EditorViewportClient.h"
#include "CoreMinimal.h"
#include "AdvancedPreviewScene.h"
#include "EngineUtils.h"
#include "TargetSelectorPreviewObject.h"
#include "Engine/Classes/GameFramework/PlayerController.h"

class STargetSelectorViewport;

class FTargetSelectorViewportClient
	: public FEditorViewportClient
	  , public TSharedFromThis<FTargetSelectorViewportClient>
{
public:
	FTargetSelectorViewportClient(TWeakPtr<class FTargetSelectorEditor> InTargetSelectorEditorPtr,
	                              FPreviewScene* InPreviewScene,
	                              const TSharedRef<STargetSelectorViewport>& InEditorViewportWidget);

	// 处理视口中的拖拽
	virtual bool InputWidgetDelta(FViewport* InViewport, EAxisList::Type CurrentAxis, FVector& Drag, FRotator& Rot,
	                              FVector& Scale) override;

	virtual void TrackingStarted(const struct FInputEventState& InInputState, bool bIsDraggingWidget,
	                             bool bNudge) override;

	virtual void TrackingStopped() override;

	virtual void AbortTracking() override;

	virtual FVector GetWidgetLocation() const override;

	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event,
	                          uint32 HitX, uint32 HitY) override;

	virtual void SetWidgetCoordSystemSpace(ECoordSystem NewCoordSystem) override;

	void SelectActor(AActor* NewActor);
	virtual void SetCurrentWidgetAxis(EAxisList::Type InAxis) override;
	virtual FMatrix GetWidgetCoordSystem() const override;
	virtual ECoordSystem GetWidgetCoordSystemSpace() const override;

	// FEditorViewportClient interface
	virtual void Tick(float DeltaSeconds) override;

	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;

protected:
	void SetSelectActor(AActor* Obj);

	void UpdateScene() const;
protected:
	TWeakPtr<class FTargetSelectorEditor> TargetSelectorEditor;
	TWeakObjectPtr<UTargetSelectorPreviewObject> PreviewObject;
	TWeakObjectPtr<AActor> SelectedActor;
	/** If true then we are manipulating a specific property or component */
	bool bIsManipulating;
};
