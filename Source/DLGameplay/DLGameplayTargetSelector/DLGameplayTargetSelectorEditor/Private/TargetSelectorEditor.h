#pragma once

#include "CoreMinimal.h"
#include "AdvancedPreviewScene.h"
#include "EditorUndoClient.h"
#include "ITargetSelectorEditor.h"
#include "PreviewScene.h"
#include "Misc/NotifyHook.h"


class UTargetSelectorPreviewObject;
class STargetSelectorDetail;
class UDLGameplayTargetSelectorBase;
class UDLGameplayTargetSelectorSphereTrace;
class UDLGameplayTargetSelectorCoordinate;
class UDLGameplayTargetSelectorCoordinateSimple;
class UDLGameplayTargetSelectorCoordinateCollection;
class ASkeletalMeshActor;


DECLARE_DELEGATE_OneParam(FOnSelectActor, AActor*);

DECLARE_DELEGATE(FOnUpDateScene);

class FTargetSelectorEditor
	: public ITargetSelectorEditor
	  , public FEditorUndoClient
	  , public FNotifyHook
	  , public FGCObject
{
protected:
#pragma region FGCObject

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObject(PreviewObject);
	}

	virtual FString GetReferencerName() const override
	{
		return TEXT("FTargetSelectorEditor");
	}

#pragma endregion

#pragma region ITargetSelectorEditor

	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	virtual FName GetToolkitFName() const override;

	virtual FText GetBaseToolkitName() const override;

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

	ASkeletalMeshActor* SkeletalMeshActor = nullptr;

	ACharacter* CurrentACharacter = nullptr;

public:
	FTargetSelectorEditor();

	void ResetPreviewCharacter();

	struct FInitArg
	{
		UObject* Object = nullptr;
		TSharedPtr<class IToolkitHost> InitToolkitHost;
	};

	void InitTargetSelectorEditor(const FInitArg& Arg);

	UObject* GetTargetObject() const;

	FPreviewScene* GetPreviewScene() const
	{
		return PreviewScene.Get();
	}

	AActor* GetPreviewActor() const;

	AActor* GetCurrentPreviewActor() const { return CurrentPreviewActor; }

	AActor* GetCollisionActor() const;

	AActor* GetAnchorActor() const;

	void OnSelectionUpdated(AActor* Actor);

	bool IsComponentSelected(const UPrimitiveComponent* PrimComponent);

	bool HasBox() const;

	bool HasSphere() const;

	bool HasCapsule() const;

	bool HasPoint() const;

	bool HasPoints() const;

	bool HasRandomPoints() const;

	FVector GetSocketLocation(FName SocketName) const;

	FTransform GetSocketTransform(FName SocketName) const;

	void UpdateStartAndEndAnchor();

	FOnSelectActor& GetOnSelectActor();

	FOnUpDateScene& GetOnUpDateScene();

	//绘制辅助线
	void DrawAuxiliaryLine();

	//动态生成点坐标
	void CreatCoordinateDot();

	//清除动态生成点坐标
	void ClearCreatCoordinateDot();

	//修正坐标
	void RevisedTransform() const;

	//重置坐标
	void RestTransform() const;

	void UpdateCameraTransform(FViewportCameraTransform Trans);
private:
	void CreateDetailWidget(const TSharedRef<class FTabManager>& InTabManager);

	//设置碰撞体线框颜色
	void SetCollisionColor() const;

	//设置动画播放
	void SetPlayAnimation(FName Type) const;

	//更新碰撞盒位置
	void UpdateCollosionTransform() const;

protected:
	virtual void SaveAsset_Execute() override;
private:
	UDLGameplayTargetSelectorBase* TargetSelectorObject = nullptr;

	TSharedPtr<IDetailsView> DetailContent;

	TSharedPtr<FAdvancedPreviewScene> PreviewScene;

	UTargetSelectorPreviewObject* PreviewObject = nullptr;

	FOnSelectActor OnSelectActor;

	FOnUpDateScene OnUpDateScene;

	TArray<AActor*> PointArray;

	TSharedPtr<SVerticalBox> ButtonArray;

	FViewportCameraTransform CameraTransform;

	//TArray<FTransform> DotArray;

	TMap<int32,TArray<FTransform>> DotArray;
};
