#include "PathControllerViewportClient.h"

#include "DrawDebugHelpers.h"
#include "EditorModeManager.h"
#include "PathControllerEditor.h"
#include "Editor/UnrealEdEngine.h"
#include "Editor/UnrealEd/Public/Kismet2/ComponentEditorUtils.h"
#include "UnrealEdGlobals.h"
#include "SPathControllerViewport.h"
#include "SplineComponentVisualizer.h"
#include "SSCSEditor.h"
#include "Components/SplineComponent.h"
#include "UnrealWidget.h"

FPathControllerViewportClient::FPathControllerViewportClient(
	const TWeakPtr<class FPathControllerEditor> InTargetSelectorEditorPtr, FPreviewScene* InPreviewScene,
	const TSharedRef<SPathControllerViewport>& InEditorViewportWidget)
	: FEditorViewportClient(nullptr, InPreviewScene, StaticCastSharedRef<SEditorViewport>(InEditorViewportWidget))
	  , PathControllerEditor(InTargetSelectorEditorPtr)
{
	bNeedsInvalidateHitProxy = true;
	bOwnsModeTools = false;

	WidgetCoordSystem = COORD_Local;

	PathControllerEditor.Pin()->SetClient(this);

	//AddRealtimeOverride(false, FText::FromString("Test"));
	FEditorViewportClient::SetShowStats(false);
	SetRealtime(false);

	PathControllerEditor.Pin().Get()->GetOnSelectActor().BindRaw(this, &FPathControllerViewportClient::SetSelectActor);
	PathControllerEditor.Pin().Get()->GetOnUpDateScene().BindRaw(this, &FPathControllerViewportClient::UpdateScene);
	PathControllerEditor.Pin().Get()->GetOnSelectSpline().BindRaw(this, &FPathControllerViewportClient::UpdateSelectSpline);
	//注册组件可视化：

	if (GUnrealEd != nullptr)
	{
		auto Visualizer = GUnrealEd->FindComponentVisualizer(USplineComponent::StaticClass()->GetFName());
		if (!Visualizer.IsValid())
		{
			const TSharedPtr<FSplineComponentVisualizer> SplineComponentVisualizer = MakeShareable(
				new FSplineComponentVisualizer);
			GUnrealEd->RegisterComponentVisualizer(FName("PathControllerVisualizer"), SplineComponentVisualizer);
			SplineComponentVisualizer->OnRegister();
		}
	}
}

FPathControllerViewportClient::~FPathControllerViewportClient()
{
	//注销组件可视化：
	const auto Visualizer= GUnrealEd->FindComponentVisualizer(FName("PathControllerVisualizer"));
	if(Visualizer)
	{
		GUnrealEd->UnregisterComponentVisualizer(FName("PathControllerVisualizer"));
	}
}

// 处理视口中的拖拽
bool FPathControllerViewportClient::InputWidgetDelta(FViewport* InViewport, EAxisList::Type CurrentAxis, FVector& Drag,
                                                     FRotator& Rot,
                                                     FVector& Scale)
{
	//UE_LOG(LogTemp, Log, TEXT("Drag %s   Rot %s    Scale %s"), *Drag.ToString(), *Rot.ToString(), *Scale.ToString());

	bool bHandled = false;
	if (bIsManipulating && CurrentAxis != EAxisList::None)
	{
		bHandled = true;
		AActor* PreviewActor = PathControllerEditor.Pin()->GetCurrentPreviewActor();
		//如果当前选中Actor是锚点，且锚点被锁，不让拖动
		if (!PreviewActor)
		{
			return false;
		}
		if (!PathControllerEditor.Pin()->CurrentPreviewActor)
		{
			return false;
		}
		const FVector ModifiedScale = Scale;
		if (PathControllerEditor.Pin()->IsSelectSpline)
		{
			if (GUnrealEd->ComponentVisManager.HandleInputDelta(this, InViewport, Drag, Rot, Scale))
			{
				GUnrealEd->RedrawLevelEditingViewports();
				Invalidate();
				PathControllerEditor.Pin()->OnSplineUpdated();
				return true;
			}
		}
		USceneComponent* SceneComp = PreviewActor->GetRootComponent();
		if (SceneComp)
		{
			FComponentEditorUtils::AdjustComponentDelta(SceneComp, Drag, Rot);

			GEditor->ApplyDeltaToComponent(
				SceneComp,
				true,
				&Drag,
				&Rot,
				&ModifiedScale,
				SceneComp->GetRelativeLocation());

			SceneComp->PostEditComponentMove(true);
		}
		GUnrealEd->RedrawLevelEditingViewports();
		PathControllerEditor.Pin()->OnPathControllerSelectionUpdated(PreviewActor);
	}
	return bHandled;
}

void FPathControllerViewportClient::TrackingStarted(const struct FInputEventState& InInputState, bool bIsDraggingWidget,
                                                    bool bNudge)
{
	UE_LOG(LogTemp, Log, TEXT("TrackingStarted"));
	if (!bIsManipulating && bIsDraggingWidget)
	{
		// Suspend component modification during each delta step to avoid recording unnecessary overhead into the transaction buffer
		GEditor->DisableDeltaModification(true);

		bIsManipulating = true;
	}
	FEditorViewportClient::TrackingStarted(InInputState, bIsDraggingWidget, bNudge);
}

void FPathControllerViewportClient::TrackingStopped()
{
	UE_LOG(LogTemp, Log, TEXT("TrackingStopped"));
	if (bIsManipulating)
	{
		// End transaction
		bIsManipulating = false;

		// Restore component delta modification
		GEditor->DisableDeltaModification(false);
	}
	FEditorViewportClient::TrackingStopped();
}

void FPathControllerViewportClient::AbortTracking()
{
	UE_LOG(LogTemp, Log, TEXT("AbortTracking"));

	FEditorViewportClient::AbortTracking();
}

FVector FPathControllerViewportClient::GetWidgetLocation() const
{
	FVector ComponentVisWidgetLocation;
	if (GUnrealEd->ComponentVisManager.IsVisualizingArchetype() &&
		GUnrealEd->ComponentVisManager.GetWidgetLocation(this, ComponentVisWidgetLocation))
	{
		return ComponentVisWidgetLocation;
	}

	FVector Location = FVector::ZeroVector;

	const AActor* PreviewActor = PathControllerEditor.Pin()->GetCurrentPreviewActor();
	if (PreviewActor)
	{
		const USceneComponent* SceneComp = PreviewActor->GetRootComponent();
		if (SceneComp)
		{
			Location = SceneComp->GetComponentLocation();
		}
	}

	return Location;
};

void FPathControllerViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event,
                                                 uint32 HitX, uint32 HitY)
{
	const FViewportClick Click(&View, this, Key, Event, HitX, HitY);
	GCurrentLevelEditingViewportClient = nullptr;

	if (HitProxy)
	{
		if (HitProxy && HitProxy->IsA(HActor::StaticGetType()))
		{
			const HActor* HitActor = static_cast<HActor*>(HitProxy);
			SelectActor(HitActor->Actor);
			PathControllerEditor.Pin()->OnPathControllerSelectionUpdated(HitActor->Actor);
			return;
		}
		if (HitProxy->IsA(HWidgetAxis::StaticGetType()))
		{
			const bool bOldModeWidgets1 = EngineShowFlags.ModeWidgets;
			const bool bOldModeWidgets2 = View.Family->EngineShowFlags.ModeWidgets;

			EngineShowFlags.SetModeWidgets(false);
			FSceneViewFamily* SceneViewFamily = const_cast<FSceneViewFamily*>(View.Family);
			SceneViewFamily->EngineShowFlags.SetModeWidgets(false);
			const bool bWasWidgetDragging = Widget->IsDragging();
			Widget->SetDragging(false);

			// Invalidate the hit proxy map so it will be rendered out again when GetHitProxy
			// is called
			Viewport->InvalidateHitProxy();

			// Undo the evil
			EngineShowFlags.SetModeWidgets(bOldModeWidgets1);
			SceneViewFamily->EngineShowFlags.SetModeWidgets(bOldModeWidgets2);

			Widget->SetDragging(bWasWidgetDragging);

			// Invalidate the hit proxy map again so that it'll be refreshed with the original
			// scene contents if we need it again later.
			Viewport->InvalidateHitProxy();
			return;
		}
		SelectActor(nullptr);

		//FEditorViewportClient::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
		GUnrealEd->ComponentVisManager.HandleClick(this, HitProxy, Click);

		PathControllerEditor.Pin()->OnSelectSpline();
		SetWidgetMode(FWidget::EWidgetMode::WM_Translate);
		EngineShowFlags.SetSelectionOutline(true);
	}
	else
	{
		if (PathControllerEditor.Pin()->IsSelectSpline)
		{
			SelectActor(PathControllerEditor.Pin()->CurrentPreviewActor);
			PathControllerEditor.Pin()->IsSelectSpline = false;
		}
		SelectActor(nullptr);
		PathControllerEditor.Pin()->OnPathControllerSelectionUpdated(nullptr);
	}
}

void FPathControllerViewportClient::SetWidgetCoordSystemSpace(ECoordSystem NewCoordSystem)
{
	WidgetCoordSystem = NewCoordSystem;
};

void FPathControllerViewportClient::SelectActor(AActor* NewActor)
{
	SelectedActor = NewActor;

	GEditor->SelectNone(true, true, false);

	if (SelectedActor.IsValid())
	{
		GEditor->SelectActor(NewActor, true, false);
		SetWidgetMode(FWidget::EWidgetMode::WM_Translate);
		EngineShowFlags.SetSelectionOutline(true);
	}
	else
	{
		GEditor->SelectActor(NewActor, true, false);
		SetWidgetMode(FWidget::EWidgetMode::WM_None);
		EngineShowFlags.SetSelectionOutline(true);
	}
}

void FPathControllerViewportClient::SetCurrentWidgetAxis(EAxisList::Type InAxis)
{
	FEditorViewportClient::SetCurrentWidgetAxis(InAxis);
}

FMatrix FPathControllerViewportClient::GetWidgetCoordSystem() const
{
	FMatrix ComponentVisWidgetCoordSystem;
	if (GUnrealEd->ComponentVisManager.IsVisualizingArchetype() && GUnrealEd->ComponentVisManager.
	                                                                          GetCustomInputCoordinateSystem(
		                                                                          this, ComponentVisWidgetCoordSystem))
	{
		return ComponentVisWidgetCoordSystem;
	}

	FMatrix Matrix = FMatrix::Identity;
	if (GetWidgetCoordSystemSpace() == COORD_Local)
	{
		const AActor* PreviewActor = PathControllerEditor.Pin()->GetCurrentPreviewActor();
		//FMatrix CustomTransform;
		if (PreviewActor)
		{
			Matrix = FQuatRotationMatrix(PreviewActor->GetRootComponent()->GetComponentQuat());
		}
	}

	if (!Matrix.Equals(FMatrix::Identity))
	{
		Matrix.RemoveScaling();
	}

	return Matrix;
}

ECoordSystem FPathControllerViewportClient::GetWidgetCoordSystemSpace() const
{
	return WidgetCoordSystem;
}

void FPathControllerViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);

	const FViewportCameraTransform CameraTrans = GetViewTransform();

	PathControllerEditor.Pin()->DrawAuxiliaryLine();
	
}

void FPathControllerViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);

	if (GUnrealEd != nullptr)
	{
		if (PathControllerEditor.Pin()->CurrentPreviewActor)
		{
			const UActorComponent* Comp = PathControllerEditor.Pin()->CurrentPreviewActor->GetComponentByClass(
				USplineComponent::StaticClass());
			if (Comp != nullptr && Comp->IsRegistered())
			{
				// Try and find a visualizer
				const TSharedPtr<FComponentVisualizer> Visualizer = GUnrealEd->FindComponentVisualizer(
					USplineComponent::StaticClass()->GetFName());
				if (Visualizer.IsValid())
				{
					Visualizer->DrawVisualization(Comp, View, PDI);
				}
			}
		}
	}
}

bool FPathControllerViewportClient::InputKey(FViewport* InViewport, int32 ControllerId, FKey Key, EInputEvent Event,
                                             float AmountDepressed, bool bGamepad)
{
	bool bHandled = GUnrealEd->ComponentVisManager.HandleInputKey(this, InViewport, Key, Event);

	if (!bHandled)
	{
		bHandled = FEditorViewportClient::InputKey(InViewport, ControllerId, Key, Event, AmountDepressed, bGamepad);
	}

	return bHandled;
}

void FPathControllerViewportClient::SetSelectActor(AActor* Obj)
{
	if (!Obj && PathControllerEditor.Pin()->IsSelectSpline)
	{
		SelectActor(PathControllerEditor.Pin()->CurrentPreviewActor);
		PathControllerEditor.Pin()->IsSelectSpline = false;
	}
	SelectActor(Obj);
}

void FPathControllerViewportClient::UpdateScene() const
{
	Viewport->Draw(false);
	//TargetSelectorEditor.Pin().Get()->DrawAuxiliaryLine();
	//Viewport->Draw(false);
}

void FPathControllerViewportClient::UpdateSelectSpline(FVector Pos)
{
	TSharedPtr<FComponentVisualizer> Visualizer = GUnrealEd->FindComponentVisualizer(
		USplineComponent::StaticClass()->GetFName());

	if(Visualizer.Get()!=nullptr)
	{
		GUnrealEd->ComponentVisManager.SetActiveComponentVis(this, Visualizer);
	}

	SetLookAtLocation(Pos,true);

	FVector CameraPos = GetViewLocation();
	FVector Dir = Pos - CameraPos;
	float Length = Dir.Size();
	Dir.Normalize();

	if(Length>1000)
	{
		SetViewLocation(Pos - Dir * 1000);
	}
	 
}
