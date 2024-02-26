#include "TargetSelectorViewportClient.h"

#include "DLAbilityTargetSelector.h"
#include "EditorModeManager.h"
#include "STargetSelectorViewport.h"
#include "TargetSelectorActor.h"
#include "TargetSelectorEditor.h"
#include "Editor/UnrealEdEngine.h"
#include "Editor/UnrealEd/Public/Kismet2/ComponentEditorUtils.h"
#include "UnrealEdGlobals.h"
#include "Animation/SkeletalMeshActor.h"

FTargetSelectorViewportClient::FTargetSelectorViewportClient(
	const TWeakPtr<class FTargetSelectorEditor> InTargetSelectorEditorPtr, FPreviewScene* InPreviewScene,
	const TSharedRef<STargetSelectorViewport>& InEditorViewportWidget)
	: FEditorViewportClient(nullptr, InPreviewScene, StaticCastSharedRef<SEditorViewport>(InEditorViewportWidget))
	  , TargetSelectorEditor(InTargetSelectorEditorPtr)
{
	bNeedsInvalidateHitProxy = true;
	//bOwnsModeTools = false;

	//AddRealtimeOverride(false, FText::FromString("Test"));
	FEditorViewportClient::SetShowStats(false);
	TargetSelectorEditor.Pin().Get()->GetOnSelectActor().BindRaw(this, &FTargetSelectorViewportClient::SetSelectActor);
	TargetSelectorEditor.Pin().Get()->GetOnUpDateScene().BindRaw(this, &FTargetSelectorViewportClient::UpdateScene);
}

// 处理视口中的拖拽
bool FTargetSelectorViewportClient::InputWidgetDelta(FViewport* InViewport, EAxisList::Type CurrentAxis, FVector& Drag,
                                                     FRotator& Rot,
                                                     FVector& Scale)
{
	//UE_LOG(LogTemp, Log, TEXT("Drag %s   Rot %s    Scale %s"), *Drag.ToString(), *Rot.ToString(), *Scale.ToString());

	bool bHandled = false;
	if (bIsManipulating && CurrentAxis != EAxisList::None)
	{
		bHandled = true;
		AActor* PreviewActor = TargetSelectorEditor.Pin()->GetCurrentPreviewActor();
		//如果当前选中Actor是锚点，且锚点被锁，不让拖动
		if (!PreviewActor)
		{
			return false;
		}
		if (PreviewActor->GetClass()->IsChildOf<AAnchorActor>() && (TargetSelectorEditor.Pin()->IsAnchorLock))
		{
			return false;
		}
		const FVector ModifiedScale = Scale;

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
		TargetSelectorEditor.Pin()->OnSelectionUpdated(PreviewActor);
	}
	return bHandled;
}

void FTargetSelectorViewportClient::TrackingStarted(const struct FInputEventState& InInputState, bool bIsDraggingWidget,
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

void FTargetSelectorViewportClient::TrackingStopped()
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

void FTargetSelectorViewportClient::AbortTracking()
{
	UE_LOG(LogTemp, Log, TEXT("AbortTracking"));

	FEditorViewportClient::AbortTracking();
}

FVector FTargetSelectorViewportClient::GetWidgetLocation() const
{
	FVector Location = FVector::ZeroVector;
	const AActor* PreviewActor = TargetSelectorEditor.Pin()->GetCurrentPreviewActor();
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

void FTargetSelectorViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event,
                                                 uint32 HitX, uint32 HitY)
{
	GCurrentLevelEditingViewportClient = nullptr;
	//const bool bIsCtrlKeyDown = Viewport->KeyState(EKeys::LeftControl) || Viewport->KeyState(EKeys::RightControl);

	if (HitProxy && HitProxy->IsA(HActor::StaticGetType()))
	{
		const HActor* HitActor = static_cast<HActor*>(HitProxy);
		SelectActor(HitActor->Actor);
		TargetSelectorEditor.Pin()->OnSelectionUpdated(HitActor->Actor);
		return;
	}


	SelectActor(nullptr);

	FEditorViewportClient::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
}

void FTargetSelectorViewportClient::SetWidgetCoordSystemSpace(ECoordSystem NewCoordSystem)
{
	ModeTools->SetCoordSystem(NewCoordSystem);
	RedrawAllViewportsIntoThisScene();
};

void FTargetSelectorViewportClient::SelectActor(AActor* NewActor)
{
	SelectedActor = NewActor;

	GEditor->SelectNone(true, true, false);

	if (SelectedActor.IsValid())
	{
		GEditor->SelectActor(NewActor, true, true);
		SetWidgetMode(UE::Widget::EWidgetMode::WM_Translate);
		EngineShowFlags.SetSelectionOutline(true);
	}
}

void FTargetSelectorViewportClient::SetCurrentWidgetAxis(EAxisList::Type InAxis)
{
	FEditorViewportClient::SetCurrentWidgetAxis(InAxis);
}

FMatrix FTargetSelectorViewportClient::GetWidgetCoordSystem() const
{
	FMatrix ComponentVisWidgetCoordSystem;
	if (GUnrealEd->ComponentVisManager.IsVisualizingArchetype() &&
		GUnrealEd->ComponentVisManager.GetCustomInputCoordinateSystem(this, ComponentVisWidgetCoordSystem))
	{
		return ComponentVisWidgetCoordSystem;
	}

	FMatrix Matrix = FMatrix::Identity;
	if (GetWidgetCoordSystemSpace() == COORD_Local)
	{
		const AActor* PreviewActor = TargetSelectorEditor.Pin()->GetCurrentPreviewActor();
		//FMatrix CustomTransform;
		Matrix = FQuatRotationMatrix(PreviewActor->GetRootComponent()->GetComponentQuat());
	}

	if (!Matrix.Equals(FMatrix::Identity))
	{
		Matrix.RemoveScaling();
	}

	return Matrix;
}

ECoordSystem FTargetSelectorViewportClient::GetWidgetCoordSystemSpace() const
{
	return ModeTools->GetCoordSystem();
}

void FTargetSelectorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);
	const FViewportCameraTransform CameraTrans = GetViewTransform();
	TargetSelectorEditor.Pin().Get()->UpdateCameraTransform(CameraTrans);
	TargetSelectorEditor.Pin().Get()->DrawAuxiliaryLine();

	if(TargetSelectorEditor.Pin().Get()->SkeletalMeshActor)
	{
		TargetSelectorEditor.Pin().Get()->SkeletalMeshActor->GetSkeletalMeshComponent()->
		TickComponent(DeltaSeconds, LEVELTICK_ViewportsOnly, 
			&TargetSelectorEditor.Pin().Get()->SkeletalMeshActor->GetSkeletalMeshComponent()->PrimaryComponentTick);
	}
	else if(TargetSelectorEditor.Pin().Get()->CurrentACharacter)
	{
		TargetSelectorEditor.Pin().Get()->CurrentACharacter->GetMesh()->
			TickComponent(DeltaSeconds, LEVELTICK_ViewportsOnly,
				&TargetSelectorEditor.Pin().Get()->CurrentACharacter->GetMesh()->PrimaryComponentTick);
	}
	
}

void FTargetSelectorViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);

}

void FTargetSelectorViewportClient::SetSelectActor(AActor* Obj)
{
	SelectActor(Obj);
}

void FTargetSelectorViewportClient::UpdateScene() const
{
	Viewport->Draw(false);
	TargetSelectorEditor.Pin().Get()->DrawAuxiliaryLine();
	Viewport->Draw(false);
}
