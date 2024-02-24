#pragma once

#include "CoreMinimal.h"
#include "AdvancedPreviewScene.h"
#include "BlueprintEditor.h"
#include "IPathControllerEditor.h"
#include "PreviewScene.h"

class UPathControllerTraceAsset;
class FPathControllerViewportClient;
DECLARE_DELEGATE_OneParam(FOnSelectActor, AActor*);

DECLARE_DELEGATE(FOnUpDateScene);

DECLARE_DELEGATE_OneParam(FOnSelectSpline,FVector)

class APathControllerTargetActor;
class APathControllerActor;
class USplineComponent;
struct HComponentVisProxy;

class FPathControllerEditor
	: public IPathControllerEditor
	, public FBlueprintEditor
{
protected:
#pragma region FGCObject

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		//Collector.AddReferencedObject(PreviewObject);
	}

	virtual FString GetReferencerName() const override
	{
		return TEXT("FPathControllerEditor");
	}

#pragma endregion

#pragma region IPathControllerEditor

	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	virtual FName GetToolkitFName() const override;

	virtual FText GetToolkitName()const override;

	virtual FText GetBaseToolkitName() const override;

	virtual FText GetToolkitToolTipText() const override;

	virtual FString GetWorldCentricTabPrefix() const override;

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
	                              FProperty* PropertyThatChanged) override
	{
	}

	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
	                              FEditPropertyChain* PropertyThatChanged) override;

	virtual void NotifyPreChange(FProperty* PropertyAboutToChange) override
	{
	}

	virtual void NotifyPreChange(FEditPropertyChain* PropertyAboutToChange) override
	{
	}

	virtual void OnClose() override;

#pragma endregion

public:
	static const FName TargetSelectorEditorAppIdentifier;

	AActor* CurrentPreviewActor = nullptr;

	bool IsAnchorLock = true;

	USplineComponent* Spline = nullptr;

	bool IsSelectSpline = false;
public:
	FPathControllerEditor();

	void ResetPreviewCharacter();

	struct FInitArg
	{
		UObject* Object = nullptr;
		TSharedPtr<class IToolkitHost> InitToolkitHost;
	};

	void InitTargetSelectorEditor(const FInitArg& Arg);

	virtual AActor* GetPreviewActor();

	FPreviewScene* GetPreviewScene() const
	{
		return PreviewScene.Get();
	}

	AActor* GetCurrentPreviewActor() const { return CurrentPreviewActor; }

	void OnPathControllerSelectionUpdated(AActor* Actor);

	void OnSelectSpline();

	void OnSplineUpdated() const;

	FOnSelectActor& GetOnSelectActor();

	FOnUpDateScene& GetOnUpDateScene();

	FOnSelectSpline& GetOnSelectSpline();

	virtual void Tick(float DeltaSeconds) override;

	//绘制辅助线
	void DrawAuxiliaryLine() const;

	void SetClient(FPathControllerViewportClient* Client);

private:
	void CreateDetailWidget(const TSharedRef<class FTabManager>& InTabManager);

	void StartSimulation() const;

	void UpDatePreviewMesh() const;

	void ResetTargetTransform();

	void UpDateTargetActor();

	void SetTargetActorMove(bool IsMove);

	void SetMoveSpeed(float Speed);

	void SetMoveRadius(float Radius);

	void SetAxisLock(int32 Type, bool IsLoack);

	void SelectSplinePoint(int32 Index);

	void SetSceneRealTime() const;

	void SetSplineLengthText();
protected:
	virtual void SaveAsset_Execute() override;
private:
	UPathControllerTraceAsset* PCAssetObject;

	TSharedPtr<IDetailsView> DetailContent;

	TSharedPtr<FAdvancedPreviewScene> PreviewScene;

	FOnSelectActor OnSelectActor;

	FOnUpDateScene OnUpDateScene;

	FOnSelectSpline OnSelectSplineDelegate;

	AActor* TargetActor = nullptr;

	APathControllerActor* PathControllerActor = nullptr;

	TSharedPtr<SCheckBox> CheckButton;

	TSharedPtr <SVerticalBox> VerticalBoxPtr;

	TSharedPtr <STextBlock> LengthText;

	float TargetMoveRadius = 500;

	FVector TargetCenter = FVector(500, 0, 0);

	bool IsMoveTarget = false;

	FVector TargetFollow ;

	int TargetMoveSpeed = 10;

	bool XAxisLock = false;
	bool YAxisLock = false;
	bool ZAxisLock = false;

	FPathControllerViewportClient* MyClient;

	mutable bool IsStartSimulation = false;

	float LastSplineLength = 0;
};
