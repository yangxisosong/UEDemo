#include "PathControllerEditor.h"

#include "AdvancedPreviewSceneModule.h"
#include "DrawDebugHelpers.h"
#include "PathControllerActor.h"
#include "GameFramework/Actor.h"
#include "Widgets/Docking/SDockTab.h"
#include "SPathControllerViewport.h"
#include "Widgets/Input/SSpinBox.h"
#include "PathControllerTraceAsset.h"
#include "PathControllerViewportClient.h"
#include "SKismetInspector.h"
#include "SplineComponentVisualizer.h"
#include "SSCSEditor.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Input/SSlider.h"

IMPLEMENT_HIT_PROXY(HSplineVisProxy, HComponentVisProxy);
IMPLEMENT_HIT_PROXY(HSplineKeyProxy, HSplineVisProxy);
IMPLEMENT_HIT_PROXY(HSplineSegmentProxy, HSplineVisProxy);
IMPLEMENT_HIT_PROXY(HSplineTangentHandleProxy, HSplineVisProxy);

const FName FPathControllerEditor::TargetSelectorEditorAppIdentifier(TEXT("PathControllerEditorApp"));

namespace
{
	const FName EditorViewPortTabId(TEXT("EditorViewPortTabId"));
	const FName EditorDetailTabId(TEXT("EditorDetailTabId"));
	const FName EditorPreviewTabId(TEXT("EditorPreviewTabId"));
}

FPathControllerEditor::FPathControllerEditor()
	: IPathControllerEditor(), PCAssetObject(nullptr), MyClient(nullptr)
{
	PreviewScene = MakeShared<FAdvancedPreviewScene>(
		FAdvancedPreviewScene::ConstructionValues()
		.SetEditor(true)
	);
	PreviewScene->SetFloorVisibility(false);
}

void FPathControllerEditor::ResetPreviewCharacter()
{
	if (nullptr == TargetActor)
	{
		const FVector Location(500, 0, 0);
		const FRotator Rotator = FRotator::ZeroRotator;

		if (PCAssetObject->TargetActor)
		{
			TargetActor = PreviewScene->GetWorld()->SpawnActor<AActor>(PCAssetObject->TargetActor);
			TargetActor->SetActorLocation(Location);
			TargetActor->SetActorRotation(Rotator);
		}
		else
		{
			TargetActor = PreviewScene->GetWorld()->SpawnActor<APathControllerTargetActor>(Location, Rotator);
		}
	}

	if (nullptr == PathControllerActor)
	{
		const FVector Location(0, 0, 0);
		const FRotator Rotator = FRotator::ZeroRotator;
		PathControllerActor = PreviewScene->GetWorld()->SpawnActor<APathControllerActor>(Location, Rotator);
		PathControllerActor->Spline->SplineCurves = PCAssetObject->SplineCurves;
		PathControllerActor->Spline->UpdateSpline();
		ResetTargetTransform();
	}
}

void FPathControllerEditor::InitTargetSelectorEditor(const FInitArg& Arg)
{
	//InitBlueprintEditor(EToolkitMode::Standalone, Arg.InitToolkitHost,);
	PCAssetObject = Cast<UPathControllerTraceAsset>(Arg.Object);
	const TArray<UObject*>* EditedObjects = GetObjectsCurrentlyBeingEdited();

	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(PCAssetObject);

	if (EditedObjects == nullptr || EditedObjects->Num() == 0)
	{
		constexpr bool bCreateDefaultToolbar = true;
		constexpr bool bCreateDefaultStandaloneMenu = true;


		const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout
			= FTabManager::NewLayout("Standalone_TargetSelectorV2")
			->AddArea
			(
				FTabManager::NewPrimaryArea()
				->SetOrientation(Orient_Vertical)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.1f)
					->SetHideTabWell(true)
					->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewSplitter()
					->SetOrientation(Orient_Horizontal)
					->SetSizeCoefficient(0.9f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.3f)
						->AddTab(EditorDetailTabId, ETabState::OpenedTab)
						->AddTab(EditorPreviewTabId, ETabState::OpenedTab)
						->SetHideTabWell(true)
						->SetForegroundTab(EditorDetailTabId)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.7f)
						->AddTab(EditorViewPortTabId, ETabState::OpenedTab)
						->SetHideTabWell(true)
					)
				)
			);

		InitAssetEditor(EToolkitMode::Standalone,
		                Arg.InitToolkitHost,
		                TargetSelectorEditorAppIdentifier,
		                StandaloneDefaultLayout,
		                bCreateDefaultStandaloneMenu,
		                bCreateDefaultToolbar,
		                ObjectsToEdit);
	}
	else
	{
		if (!EditedObjects->Contains(PCAssetObject))
		{
			AddEditingObject(PCAssetObject);
		}
	}

	RegenerateMenusAndToolbars();

	this->ResetPreviewCharacter();

	SetSplineLengthText();

	//默认打开 Detail Tab
	const auto Detail = GetTabManager()->TryInvokeTab(EditorDetailTabId);
	Detail.Get()->ActivateInParent(UserClickedOnTab);
}


void FPathControllerEditor::CreateDetailWidget(const TSharedRef<class FTabManager>& InTabManager)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
		"PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, false);
	DetailsViewArgs.NotifyHook = this;
	DetailsViewArgs.HostTabManager = InTabManager;

	DetailContent = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailContent->ClearSearch();
	DetailContent->SetObject(PCAssetObject);
}

void FPathControllerEditor::StartSimulation() const
{
	IsStartSimulation = true;
	this->OnSelectActor.ExecuteIfBound(nullptr);
	FlushPersistentDebugLines(PreviewScene->GetWorld());
	const FVector Location(0, 0, 0);
	const FRotator Rotator = FRotator::ZeroRotator;
	this->PathControllerActor->SetActorLocation(Location);
	this->PathControllerActor->SetActorRotation(Rotator);

	//开始模拟时将物体移动到spline第一个点的位置
	if (PathControllerActor->Spline->SplineCurves.Position.Points.Num() > 0)
	{
		this->PathControllerActor->
		      SetActorLocation(PathControllerActor->Spline->SplineCurves.Position.Points[0].OutVal);
		this->PathControllerActor->SetActorRotation(FRotator::ZeroRotator);
	}
	switch (PCAssetObject->FunctionType)
	{
	case EPathControllerLaunchType::LaunchToDirection:
		this->PathControllerActor->PathControllerComponent->LaunchToDirection(
			PCAssetObject, PCAssetObject->RelativeDirection);
		break;
	case EPathControllerLaunchType::LaunchToLocation:
		this->PathControllerActor->PathControllerComponent->LaunchToLocation(
			PCAssetObject, PCAssetObject->TargetLocation, PCAssetObject->Duration, PCAssetObject->RelativeDirection,
			PCAssetObject->MaxDistance);
		break;
	case EPathControllerLaunchType::LaunchToTarget:
		this->PathControllerActor->PathControllerComponent->LaunchToTarget(
			PCAssetObject, TargetActor, PCAssetObject->bIsTrack, PCAssetObject->bIsHoming, PCAssetObject->Duration,
			PCAssetObject->RelativeDirection);
		break;
	case EPathControllerLaunchType::LaunchToTargetWithFreeTrack:
		this->PathControllerActor->PathControllerComponent->LaunchToTargetWithFreeTrack(
			PCAssetObject, TargetActor, PCAssetObject->RelativeDirection, PCAssetObject->bIsHoming,
			PCAssetObject->Duration);
		break;
	default:
		break;
	}
}

void FPathControllerEditor::UpDatePreviewMesh() const
{
	if (PCAssetObject->PreviewMesh)
	{
		const auto Mesh = PCAssetObject->PreviewMesh.LoadSynchronous();
		PathControllerActor->MeshComponent->SetStaticMesh(Cast<UStaticMesh>(Mesh));
	}
}

void FPathControllerEditor::ResetTargetTransform()
{
	const auto LastIndex = PathControllerActor->Spline->SplineCurves.Position.Points.Num() - 1;

	const FTransform& LastPoint = PathControllerActor->Spline->GetTransformAtSplinePoint(
		LastIndex, ESplineCoordinateSpace::World, false);
	const FTransform LastPos = LastIndex > 0 ? LastPoint : FTransform::Identity;
	TargetActor->SetActorTransform(LastPos);

	IsMoveTarget = false;
	this->CheckButton.Get()->SetIsChecked(ECheckBoxState::Unchecked);
}

void FPathControllerEditor::UpDateTargetActor()
{
	if (TargetActor)
	{
		const FVector Location = TargetActor->GetActorLocation();
		const FRotator Rotator = TargetActor->GetActorRotation();

		PreviewScene->GetWorld()->DestroyActor(TargetActor);

		if (PCAssetObject->TargetActor)
		{
			TargetActor = PreviewScene->GetWorld()->SpawnActor<AActor>(PCAssetObject->TargetActor);
			TargetActor->SetActorLocation(Location);
			TargetActor->SetActorRotation(Rotator);
		}
		else
		{
			TargetActor = PreviewScene->GetWorld()->SpawnActor<APathControllerTargetActor>(Location, Rotator);
		}
	}
}

void FPathControllerEditor::SetTargetActorMove(const bool IsMove)
{
	IsMoveTarget = IsMove;
	TargetCenter = TargetActor->GetActorLocation();

	auto Dir = FVector(FMath::RandRange(0, 1), FMath::RandRange(0, 1), FMath::RandRange(0, 1));
	Dir.Normalize();
	TargetFollow = TargetCenter + Dir * FMath::RandRange(1.0f, TargetMoveRadius);
}

void FPathControllerEditor::SetMoveSpeed(float Speed)
{
	this->TargetMoveSpeed = Speed;
	const auto TextBlock = StaticCastSharedRef<STextBlock>(VerticalBoxPtr.Get()->GetChildren()->GetChildAt(5));

	const FString SpeedText = FString(UTF8_TO_TCHAR("速度：")) + FString::FromInt(TargetMoveSpeed);
	TextBlock.Get().SetText(FText::FromString(SpeedText));
}

void FPathControllerEditor::SetMoveRadius(float Radius)
{
	this->TargetMoveRadius = Radius;

	const auto RadiusTextBlock =
		StaticCastSharedRef<STextBlock>(VerticalBoxPtr.Get()->GetChildren()->GetChildAt(7));
	const FString SpeedText = FString(UTF8_TO_TCHAR("移动半径：")) + FString::FromInt(TargetMoveRadius);

	RadiusTextBlock.Get().SetText(FText::FromString(SpeedText));
}

void FPathControllerEditor::SetAxisLock(int32 Type, bool IsLoack)
{
	if (Type == 1)
	{
		XAxisLock = IsLoack;
	}
	else if (Type == 2)
	{
		YAxisLock = IsLoack;
	}
	else if (Type == 3)
	{
		ZAxisLock = IsLoack;
	}
}

void FPathControllerEditor::SelectSplinePoint(int32 Index)
{
	this->OnSelectActor.ExecuteIfBound(PathControllerActor);
	CurrentPreviewActor = PathControllerActor;
	const TSharedPtr<FComponentVisualizer> Visualizer = GUnrealEd->FindComponentVisualizer(
		USplineComponent::StaticClass()->GetFName());
	if (Visualizer.Get() == nullptr)
	{
		return;
	}

	IsSelectSpline = true;
	if (Index == 0)
	{
		//起点
		//FSplineComponentVisualizer
		const FVector Pos = PathControllerActor->Spline->SplineCurves.Position.Points[0].OutVal;
		this->OnSelectSplineDelegate.ExecuteIfBound(Pos);
		StaticCastSharedPtr<FSplineComponentVisualizer>(Visualizer)->HandleSelectFirstLastSplinePoint(
			PathControllerActor->Spline, true);
	}
	else
	{
		//终点
		const int32 index = PathControllerActor->Spline->SplineCurves.Position.Points.Num() - 1;
		const FVector Pos = PathControllerActor->Spline->SplineCurves.Position.Points[index].OutVal;
		this->OnSelectSplineDelegate.ExecuteIfBound(Pos);
		StaticCastSharedPtr<FSplineComponentVisualizer>(Visualizer)->HandleSelectFirstLastSplinePoint(
			PathControllerActor->Spline, false);
	}
	OnSelectSpline();
}

void FPathControllerEditor::SetSceneRealTime() const
{
	if (nullptr == PathControllerActor || nullptr == MyClient)
	{
		return;
	}
	//MyClient->IsFocused() PreviewScene-
	if (IsStartSimulation == true || IsMoveTarget == true)
	{
		if (!MyClient->IsRealtime())
		{
			MyClient->PopRealtimeOverride();
			MyClient->SetShowStats(true);
			MyClient->SetRealtime(true);
		}
	}

	if (MyClient->IsRealtime() && PathControllerActor->PathControllerComponent->GetIsStopped() &&
		IsMoveTarget == false)
	{
		IsStartSimulation = false;
		MyClient->SetRealtime(false);
	}
}

void FPathControllerEditor::SetSplineLengthText()
{
	if(LengthText.IsValid())
	{
		const auto Len = PathControllerActor->Spline->GetSplineLength();
		const FString LenStr = UTF8_TO_TCHAR("线段长度：") + FString::SanitizeFloat(Len);
		LengthText->SetText(FText::FromString(LenStr));
		LastSplineLength = Len;
	}
}

void FPathControllerEditor::SaveAsset_Execute()
{
	OnSplineUpdated();
	FBlueprintEditor::SaveAsset_Execute();
}

FLinearColor FPathControllerEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::Blue;
}

FName FPathControllerEditor::GetToolkitFName() const
{
	return TEXT("PathControllerEditor");
}

FText FPathControllerEditor::GetToolkitName() const
{
	return FText::FromString(PCAssetObject->GetName());
}

FText FPathControllerEditor::GetBaseToolkitName() const
{
	return FText::FromString(PCAssetObject->GetName());
}

FText FPathControllerEditor::GetToolkitToolTipText() const
{
	return GetToolTipTextForObject(PCAssetObject);
}

FString FPathControllerEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("PathController");
}

void FPathControllerEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FBlueprintEditor::RegisterTabSpawners(InTabManager);
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(FText::FromString("WorkspaceMenuCategory"));


	auto CreateDetail = [this, InTabManager](const FSpawnTabArgs& Args)-> TSharedRef<SDockTab>
	{
		this->CreateDetailWidget(InTabManager);

		TSharedRef<SDockTab> DockTab = SNew(SDockTab)
			.Label(FText::FromString("Detail"))
			[
				DetailContent.ToSharedRef()
			];

		return DockTab;
	};

	auto CreateViewport = [this](const FSpawnTabArgs& Args)-> TSharedRef<SDockTab>
	{
		const TSharedRef<SCheckBox> Button = SNew(SCheckBox)
			.IsChecked(ECheckBoxState::Unchecked)
			.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
		                                                    {
			                                                    if (State == ECheckBoxState::Checked)
			                                                    {
				                                                    this->SetTargetActorMove(true);
			                                                    }
			                                                    else
			                                                    {
				                                                    this->SetTargetActorMove(false);
			                                                    }
			                                                    UE_LOG(LogTemp, Warning, TEXT("SetTargetActorMove"));
		                                                    });
		const auto NormalFont = FStyleDefaults::GetFontInfo(15);
		const auto LengthTextBox = SNew(STextBlock)
															.Font(NormalFont)
															.ColorAndOpacity(FLinearColor::Green);
		LengthText = LengthTextBox;
		const auto HorizontalBox = SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Left)
			  .VAlign(VAlign_Fill)
			  .Padding(FMargin(0, 0, 0, 0))
			  .AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(UTF8_TO_TCHAR("移动目标")))
			]
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Right)
			  .VAlign(VAlign_Fill)
			  .Padding(FMargin(0, 0, 0, 0))
			[
				Button
			];

		const TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			  .AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString(UTF8_TO_TCHAR("目标点重置")))
			.OnClicked_Lambda([this]()
				             {
					             this->ResetTargetTransform();
					             UE_LOG(LogTemp, Warning, TEXT("ResetTargetTransform"));
					             return FReply::Handled();
				             })
			]
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			  .Padding(FMargin(0, 20, 0, 0))
			  .AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString(UTF8_TO_TCHAR("模拟路径")))
			.OnClicked_Lambda([this]()
				             {
					             this->StartSimulation();
					             UE_LOG(LogTemp, Warning, TEXT("StartSimulation"));
					             return FReply::Handled();
				             })
			]
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			  .Padding(FMargin(0, 20, 0, 0))
			  .AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString(UTF8_TO_TCHAR("路径起点")))
			.OnClicked_Lambda([this]()
				             {
					             this->SelectSplinePoint(0);
					             return FReply::Handled();
				             })
			]
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			  .Padding(FMargin(0, 20, 0, 0))
			  .AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString(UTF8_TO_TCHAR("路径终点")))
			.OnClicked_Lambda([this]()
				             {
					             this->SelectSplinePoint(1);
					             return FReply::Handled();
				             })
			]
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			  .Padding(FMargin(0, 20, 0, 0))
			  .AutoHeight()
			[
				HorizontalBox
			]
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			  .Padding(FMargin(0, 20, 0, 0))
			  .AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(UTF8_TO_TCHAR("速度")))
			]
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			[
				SNew(SSlider)
				.MinValue(1)
			.MaxValue(100)
			.Value(0)
			.OnValueChanged_Lambda([this](float value)
				             {
					             this->SetMoveSpeed(value);
					             UE_LOG(LogTemp, Warning, TEXT("OnSpeedValueChanged %f"), value);
				             })
			]
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			  .Padding(FMargin(0, 20, 0, 0))
			  .AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(UTF8_TO_TCHAR("移动半径")))
			]
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			[
				SNew(SSlider)
				.MinValue(100)
			.MaxValue(1000)
			.Value(TargetMoveRadius)
			.OnValueChanged_Lambda([this](float value)
				             {
					             this->SetMoveRadius(value);
					             UE_LOG(LogTemp, Warning, TEXT("OnRadiusValueChanged %f"), value);
				             })
			]
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			  .Padding(FMargin(0, 20, 0, 0))
			  .AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .HAlign(HAlign_Left)
				  .VAlign(VAlign_Fill)
				  .Padding(FMargin(0, 0, 0, 0))
				  .AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString(UTF8_TO_TCHAR("锁定X轴")))
				]
				+ SHorizontalBox::Slot()
				  .HAlign(HAlign_Right)
				  .VAlign(VAlign_Fill)
				  .Padding(FMargin(0, 0, 0, 0))
				[
					SNew(SCheckBox)
				.IsChecked(ECheckBoxState::Unchecked)
			.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
					               {
						               if (State == ECheckBoxState::Checked)
						               {
							               this->SetAxisLock(1, true);
						               }
						               else
						               {
							               this->SetAxisLock(1, false);
						               }
						               UE_LOG(LogTemp, Warning, TEXT("SetXAxisLock"));
					               })
				]
			]
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			  .Padding(FMargin(0, 20, 0, 0))
			  .AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .HAlign(HAlign_Left)
				  .VAlign(VAlign_Fill)
				  .Padding(FMargin(0, 0, 0, 0))
				  .AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString(UTF8_TO_TCHAR("锁定Y轴")))
				]
				+ SHorizontalBox::Slot()
				  .HAlign(HAlign_Right)
				  .VAlign(VAlign_Fill)
				  .Padding(FMargin(0, 0, 0, 0))
				[
					SNew(SCheckBox)
				.IsChecked(ECheckBoxState::Unchecked)
			.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
					               {
						               if (State == ECheckBoxState::Checked)
						               {
							               this->SetAxisLock(2, true);
						               }
						               else
						               {
							               this->SetAxisLock(2, false);
						               }
						               UE_LOG(LogTemp, Warning, TEXT("SetYAxisLock"));
					               })
				]
			]
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			  .Padding(FMargin(0, 20, 0, 0))
			  .AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .HAlign(HAlign_Left)
				  .VAlign(VAlign_Fill)
				  .Padding(FMargin(0, 0, 0, 0))
				  .AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString(UTF8_TO_TCHAR("锁定Z轴")))
				]
				+ SHorizontalBox::Slot()
				  .HAlign(HAlign_Right)
				  .VAlign(VAlign_Fill)
				  .Padding(FMargin(0, 0, 0, 0))
				[
					SNew(SCheckBox)
				.IsChecked(ECheckBoxState::Unchecked)
			.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
					               {
						               if (State == ECheckBoxState::Checked)
						               {
							               this->SetAxisLock(3, true);
						               }
						               else
						               {
							               this->SetAxisLock(3, false);
						               }
						               UE_LOG(LogTemp, Warning, TEXT("SetZAxisLock"));
					               })
				]
			];

		

		TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SPathControllerViewport)
				.PathControllerEditor(SharedThis(this))
			]
			+ SOverlay::Slot()
			[
				SNew(SBox)
				.WidthOverride(200)
				.HeightOverride(500)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				.Padding(FMargin(0, 130, 0, 0))
				[
					VerticalBox
				]
			]
			+ SOverlay::Slot()
				[
					SNew(SBox)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					.Padding(FMargin(0, 100, 0, 0))
					[
						LengthTextBox
					]
				]
		];

		this->CheckButton = Button;
		this->VerticalBoxPtr = VerticalBox;
		const auto TextBlock = StaticCastSharedRef<STextBlock>(VerticalBox.Get().GetChildren()->GetChildAt(5));

		FString SpeedText = FString(UTF8_TO_TCHAR("速度：")) + FString::FromInt(TargetMoveSpeed);
		TextBlock.Get().SetText(FText::FromString(SpeedText));

		const auto RadiusTextBlock = StaticCastSharedRef<STextBlock>(VerticalBox.Get().GetChildren()->GetChildAt(7));

		SpeedText = FString(UTF8_TO_TCHAR("移动半径：")) + FString::FromInt(TargetMoveRadius);
		RadiusTextBlock.Get().SetText(FText::FromString(SpeedText));
		return DockTab;
	};

	auto CreatePreviewDetails = [this](const FSpawnTabArgs& Args)-> TSharedRef<SDockTab>
	{
		FAdvancedPreviewSceneModule& AdvancedPreviewSceneModule = FModuleManager::LoadModuleChecked<
			FAdvancedPreviewSceneModule>("AdvancedPreviewScene");

		TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		[
			AdvancedPreviewSceneModule.CreateAdvancedPreviewSceneSettingsWidget(
				PreviewScene.ToSharedRef(), nullptr)
		];

		return DockTab;
	};

	InTabManager->RegisterTabSpawner(EditorDetailTabId, FOnSpawnTab::CreateLambda(CreateDetail))
	            .SetDisplayName(FText::FromString("DataPanel"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef());

	InTabManager->RegisterTabSpawner(EditorViewPortTabId, FOnSpawnTab::CreateLambda(CreateViewport))
	            .SetDisplayName(FText::FromString("EditorViewport"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef());


	InTabManager->RegisterTabSpawner(EditorPreviewTabId, FOnSpawnTab::CreateLambda(CreatePreviewDetails))
	            .SetDisplayName(FText::FromString("EditorPreview"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void FPathControllerEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	InTabManager->UnregisterTabSpawner(EditorDetailTabId);
	InTabManager->UnregisterTabSpawner(EditorViewPortTabId);
	InTabManager->UnregisterTabSpawner(EditorPreviewTabId);

	FBlueprintEditor::UnregisterTabSpawners(InTabManager);
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

void FPathControllerEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
                                             FEditPropertyChain* PropertyThatChanged)
{
	UE_LOG(LogTemp, Log, TEXT("NotifyPostChange %s"), *PropertyChangedEvent.GetPropertyName().ToString());

	static FName PreviewMesh(TEXT("PreviewMesh"));
	static FName TargetActorName(TEXT("TargetActor"));

	const auto CurrentNode = PropertyThatChanged->GetActiveMemberNode();

	if (CurrentNode)
	{
		const FProperty* Property = CurrentNode->GetValue();
		if (Property)
		{
			if (Property->GetFName() == PreviewMesh)
			{
				this->UpDatePreviewMesh();
			}
			else if (Property->GetFName() == TargetActorName)
			{
				this->UpDateTargetActor();
			}
		}
	}

	SetSplineLengthText();
}

void FPathControllerEditor::OnClose()
{
	this->MyClient = nullptr;
}

AActor* FPathControllerEditor::GetPreviewActor()
{
	UWorld* PreviewWorld = PreviewScene.Get()->GetWorld();
	for (TActorIterator<AActor> It(PreviewWorld); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor->IsPendingKillPending() && (Actor->GetComponentByClass(APathControllerActor::StaticClass())))
		{
			PreviewActorPtr = Actor;
			break;
		}
	}

	return PreviewActorPtr.Get();
}

void FPathControllerEditor::OnPathControllerSelectionUpdated(AActor* Actor)
{
	CurrentPreviewActor = Actor;
	IsSelectSpline = false;
	if (Actor != nullptr)
	{
		if (Actor == TargetActor)
		{
			DetailContent->RemoveInvalidObjects();
			DetailContent->SetObject(TargetActor->GetRootComponent());
		}
	}
	else
	{
		DetailContent->RemoveInvalidObjects();
		DetailContent->SetObject(PCAssetObject);
	}
}

void FPathControllerEditor::OnSelectSpline()
{
	IsSelectSpline = true;
	DetailContent->RemoveInvalidObjects();
	DetailContent->SetObject(PathControllerActor->Spline);
}

void FPathControllerEditor::OnSplineUpdated() const
{
	PCAssetObject->SplineCurves = PathControllerActor->Spline->SplineCurves;
}

FOnSelectActor& FPathControllerEditor::GetOnSelectActor()
{
	return OnSelectActor;
}

FOnUpDateScene& FPathControllerEditor::GetOnUpDateScene()
{
	return OnUpDateScene;
}

FOnSelectSpline& FPathControllerEditor::GetOnSelectSpline()
{
	return OnSelectSplineDelegate;
}

void FPathControllerEditor::Tick(float DeltaSeconds)
{
	FBlueprintEditor::Tick(DeltaSeconds);
	PathControllerActor->PathControllerComponent->ShowDebugLine = true;
	PathControllerActor->PathControllerComponent->TickComponent(DeltaSeconds, LEVELTICK_ViewportsOnly, nullptr);

	SetSceneRealTime();

	if(LastSplineLength!=PathControllerActor->Spline->GetSplineLength())
	{
		SetSplineLengthText();
	}

	if (IsMoveTarget)
	{
		const FVector MaxLength = TargetActor->GetActorLocation() - TargetFollow;
		if (MaxLength.Size() < 5 || MaxLength.Size() > TargetMoveRadius)
		{
			auto Dir = FVector(FMath::RandRange(0, 1), FMath::RandRange(0, 1), FMath::RandRange(0, 1));
			Dir.Normalize();
			if (XAxisLock)
			{
				Dir.X = 0;
			}
			if (YAxisLock)
			{
				Dir.Y = 0;
			}
			if (ZAxisLock)
			{
				Dir.Z = 0;
			}
			TargetFollow = TargetCenter + Dir * FMath::RandRange(0.0f, TargetMoveRadius);
		}
		else
		{
			auto MoveDir = TargetFollow - TargetActor->GetActorLocation();
			MoveDir.Normalize();

			auto OldPos = TargetActor->GetActorLocation();
			TargetActor->SetActorLocation(OldPos + MoveDir * DeltaSeconds * TargetMoveSpeed * 5);
		}
	}
}

void FPathControllerEditor::DrawAuxiliaryLine() const
{
	//FlushPersistentDebugLines(GetPreviewScene()->GetWorld());
	if (PCAssetObject->FunctionType == EPathControllerLaunchType::LaunchToDirection)
	{
		const FVector DirectionStartpos = PCAssetObject->SplineCurves.Position.Points[0].OutVal;
		FVector Dir = PCAssetObject->RelativeDirection;
		Dir.Normalize();
		const FVector Endpos = Dir * 10000;
		DrawDebugLine(GetPreviewScene()->GetWorld(), DirectionStartpos, Endpos, FColor::Red,
		              true, 0, 0, 1);
	}
	else if (PCAssetObject->FunctionType == EPathControllerLaunchType::LaunchToLocation)
	{
		const auto Position = PCAssetObject->TargetLocation;
		DrawDebugSphere(GetPreviewScene()->GetWorld(), Position, 10, 32, FColor::Red);

		FVector LocationStartpos = PCAssetObject->SplineCurves.Position.Points[0].OutVal;
		DrawDebugLine(GetPreviewScene()->GetWorld(), LocationStartpos, Position, FColor::Red,
		              false, 0, 1, 1);
	}

	if (PCAssetObject->EnableTargetRadiusCheck)
	{
		DrawDebugSphere(GetPreviewScene()->GetWorld(), TargetActor->GetActorLocation(), PCAssetObject->TargetRadius,
		                32, FColor::Green);
	}

	const FVector StartPos = PathControllerActor->GetActorTransform().GetLocation();
	FVector Dir = PCAssetObject->SubObjectForword;
	Dir.Normalize();
	const FVector EndPos = StartPos + Dir * 100;
	DrawDebugDirectionalArrow(GetPreviewScene()->GetWorld(), StartPos, EndPos,
	                          50, FColor::Red, false, -1, 0, 5);
}

void FPathControllerEditor::SetClient(FPathControllerViewportClient* Client)
{
	MyClient = Client;
}
