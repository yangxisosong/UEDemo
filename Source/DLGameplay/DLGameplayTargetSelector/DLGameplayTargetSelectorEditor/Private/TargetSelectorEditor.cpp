#include "TargetSelectorEditor.h"
#include "GameFramework/Actor.h"
#include "AdvancedPreviewSceneModule.h"
#include "DLAbilityTargetSelector.h"
#include "Widgets/Docking/SDockTab.h"
#include "STargetSelectorViewport.h"
#include "TargetSelectorPreviewObject.h"
#include "Widgets/Input/SSpinBox.h"
#include "AdvancedPreviewScene.h"
#include "AssetViewerSettings.h"
#include "DrawDebugHelpers.h"
#include "TargetSelectorActor.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Runtime/Engine/Classes/Engine/StaticMeshActor.h"
#include "Animation/SkeletalMeshActor.h"
#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectGlobals.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Components/LineBatchComponent.h"
#include "Modules/ModuleManager.h"


const FName FTargetSelectorEditor::TargetSelectorEditorAppIdentifier(TEXT("TargetSelectorEditorApp"));

namespace
{
	const FName EditorViewPortTabId(TEXT("EditorViewPortTabId"));
	const FName EditorDetailTabId(TEXT("EditorDetailTabId"));
	const FName EditorPreviewTabId(TEXT("EditorPreviewTabId"));
}

FTargetSelectorEditor::FTargetSelectorEditor()
	: ITargetSelectorEditor()
{
	PreviewScene = MakeShared<FAdvancedPreviewScene>(
		FAdvancedPreviewScene::ConstructionValues()
		.SetEditor(true)
	);
	PreviewScene->SetFloorVisibility(false);
	PreviewObject = NewObject<UTargetSelectorPreviewObject>(GetTransientPackage());
	PreviewObject->SetFlags(RF_Transient);
}

void FTargetSelectorEditor::ResetPreviewCharacter()
{
	AActor* PreviewActor = GetPreviewActor();
	UStaticMeshComponent* STComponent;
	USkeletalMeshComponent* SMComponent;
	//清除Actor
	if (PreviewActor)
	{
		UActorComponent* ActorComponent = PreviewActor->GetComponentByClass(USkeletalMeshComponent::StaticClass());
		SMComponent = Cast<USkeletalMeshComponent>(ActorComponent);

		UActorComponent* ActorComponent2 = PreviewActor->GetComponentByClass(UStaticMeshComponent::StaticClass());
		STComponent = Cast<UStaticMeshComponent>(ActorComponent2);

		if (SMComponent)
		{
			PreviewScene->RemoveComponent(SMComponent);
		}
		if (STComponent)
		{
			PreviewScene->RemoveComponent(STComponent);
		}

		PreviewScene->GetWorld()->DestroyActor(PreviewActor);
	}
	CurrentACharacter = nullptr;
	//生成AnchorActor
	if (!GetAnchorActor())
	{
		const FVector Location = TargetSelectorObject->PreviewMeshTransform.GetLocation();
		const FRotator Rotator = TargetSelectorObject->PreviewMeshTransform.Rotator();
		const AAnchorActor* SPActor = PreviewScene->GetWorld()->SpawnActor<AAnchorActor>(Location, Rotator);
		SPActor->SphereComponent->ShapeColor = PreviewObject->AnchorLineColor;
	}
	//生成CollisionActor
	if (!GetCollisionActor())
	{
		const FVector Location = TargetSelectorObject->PreviewMeshTransform.GetLocation();
		const FRotator Rotator = TargetSelectorObject->PreviewMeshTransform.Rotator();
		if (HasSphere())
		{
			const ASphereActor* SPActor = PreviewScene->GetWorld()->SpawnActor<ASphereActor>(Location, Rotator);
			const UDLGameplayTargetSelectorSphereTrace* SphereObject = Cast<UDLGameplayTargetSelectorSphereTrace>(
				TargetSelectorObject);
			SPActor->SphereComponent->SetSphereRadius(SphereObject->Radius);
			SPActor->SphereComponent->ShapeColor = PreviewObject->CollisionLineColor;
		}
		else if (HasBox())
		{
			const ABoxActor* SBActor = PreviewScene->GetWorld()->SpawnActor<ABoxActor>(Location, Rotator);
			const UDLGameplayTargetSelectorBoxTrace* BoxObject = Cast<UDLGameplayTargetSelectorBoxTrace>(
				TargetSelectorObject);
			SBActor->BoxComponent->SetBoxExtent(BoxObject->HalfSize);
			SBActor->BoxComponent->ShapeColor = PreviewObject->CollisionLineColor;
		}
		else if (HasCapsule())
		{
			const ACapsuleActor* SCActor = PreviewScene->GetWorld()->SpawnActor<ACapsuleActor>(Location, Rotator);
			const UDLGameplayTargetSelectorCapsuleTrace* CapsuleObject = Cast<UDLGameplayTargetSelectorCapsuleTrace>(
				TargetSelectorObject);
			SCActor->CapsuleComponent->SetCapsuleHalfHeight(CapsuleObject->CapsuleHalfHeight);
			SCActor->CapsuleComponent->SetCapsuleRadius(CapsuleObject->CapsuleRadius);
			SCActor->CapsuleComponent->ShapeColor = PreviewObject->CollisionLineColor;
		}
		else if (HasPoint() || HasPoints())
		{
			//编辑点不创建碰撞点

			//删除锚点按钮
			const int32 LastNum = this->ButtonArray.Get()->GetChildren()->Num();
			if (LastNum == 2 && this->PointArray.Num() == 0)
			{
				const auto RemoveButton = this->ButtonArray.Get()->GetChildren()->GetChildAt(LastNum - 1);
				this->ButtonArray.Get()->RemoveSlot(RemoveButton);
				if (HasRandomPoints())
				{
					this->ButtonArray.Get()->AddSlot()
					    .Padding(FMargin(0, 20, 0, 0))
					[
						SNew(SButton)
						.Text(FText::FromString(UTF8_TO_TCHAR("生成坐标点")))
					.OnClicked_Lambda([this]()
						             {
							             this->CreatCoordinateDot();
							             return FReply::Handled();
						             })
					];
					this->ButtonArray.Get()->AddSlot()
					    .Padding(FMargin(0, 20, 0, 0))
					[
						SNew(SButton)
						.Text(FText::FromString(UTF8_TO_TCHAR("清除坐标点")))
					.OnClicked_Lambda([this]()
						             {
							             this->ClearCreatCoordinateDot();
							             return FReply::Handled();
						             })
					];
				}
				if (HasPoints() && !HasRandomPoints())
				{
					this->ButtonArray.Get()->AddSlot()
					    .Padding(FMargin(0, 20, 0, 0))
					[
						SNew(SButton)
							.Text(FText::FromString(UTF8_TO_TCHAR("修改坐标点")))
						.OnClicked_Lambda([this]()
						             {
							             this->RevisedTransform();
							             return FReply::Handled();
						             })
					];
					this->ButtonArray.Get()->AddSlot()
					    .Padding(FMargin(0, 20, 0, 0))
					[
						SNew(SButton)
							.Text(FText::FromString(UTF8_TO_TCHAR("还原坐标点")))
						.OnClicked_Lambda([this]()
						             {
							             this->RestTransform();
							             return FReply::Handled();
						             })
					];
				}
			}
		}
	}

	//生成StaticMesh
	const FVector MeshLocation = TargetSelectorObject->PreviewMeshTransform.GetLocation() + TargetSelectorObject->
		ResetPreviewTransform.GetLocation();
	const FRotator MeshRotator = TargetSelectorObject->PreviewMeshTransform.Rotator() + TargetSelectorObject->
		ResetPreviewTransform.Rotator();
	const FVector MeshScale = TargetSelectorObject->PreviewMeshTransform.GetScale3D();

	if (TargetSelectorObject->PreviewActor)
	{
		TargetSelectorObject->PreviewActor;
		ACharacter* SMActor = PreviewScene->GetWorld()->SpawnActor<ACharacter>(TargetSelectorObject->PreviewActor);
		if (!SMActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()))
		{
			SMActor->AddComponentByClass(USkeletalMeshComponent::StaticClass(), true, FTransform::Identity, true);
		}
		this->SkeletalMeshActor = nullptr;
		TargetSelectorObject->PreviewMeshTransform = SMActor->GetTransform();
		TargetSelectorObject->ResetPreviewTransform = FTransform::Identity;
		TargetSelectorObject->PreviewMesh = SMActor->GetMesh()->SkeletalMesh;

		SMActor->GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		SMActor->GetArrowComponent()->SetVisibility(false);
		SMActor->GetCapsuleComponent()->SetVisibility(false);

		CurrentACharacter = SMActor;
		CurrentPreviewActor = GetPreviewActor();
		OnSelectionUpdated(GetCurrentPreviewActor());
	}
	else
	{
		const auto Mesh = TargetSelectorObject->PreviewMesh.LoadSynchronous();
		if (Mesh && Mesh->GetRenderAssetType() == EStreamableRenderAssetType::StaticMesh)
		{
			AStaticMeshActor* STActor = PreviewScene->GetWorld()->SpawnActor<AStaticMeshActor>(
				MeshLocation, MeshRotator);
			if (!STActor->GetComponentByClass(UStaticMesh::StaticClass()))
			{
				STActor->AddComponentByClass(UStaticMesh::StaticClass(), true, FTransform::Identity, true);
			}
			UActorComponent* ActorComponent = STActor->GetComponentByClass(UStaticMeshComponent::StaticClass());
			STComponent = Cast<UStaticMeshComponent>(ActorComponent);
			STComponent->SetStaticMesh(Cast<UStaticMesh>(Mesh));
			STActor->SetActorScale3D(MeshScale);
			TargetSelectorObject->AnchorSelectType = EAnchorSelectType::ActorTransform;
			CurrentPreviewActor = GetPreviewActor();
			OnSelectionUpdated(GetCurrentPreviewActor());
		}
		//生成SkeletalMeshActor
		SkeletalMeshActor = nullptr;
		if (Mesh && Mesh->GetRenderAssetType() == EStreamableRenderAssetType::SkeletalMesh)
		{
			TargetSelectorObject->PreviewMeshAnimation.LoadSynchronous();

			ASkeletalMeshActor* SMActor = PreviewScene->GetWorld()->SpawnActor<ASkeletalMeshActor>(
				MeshLocation, MeshRotator);
			this->SkeletalMeshActor = SMActor;
			if (!SMActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()))
			{
				SMActor->AddComponentByClass(USkeletalMeshComponent::StaticClass(), true, FTransform::Identity, true);
			}
			UActorComponent* ActorComponent = SMActor->GetComponentByClass(USkeletalMeshComponent::StaticClass());
			SMComponent = Cast<USkeletalMeshComponent>(ActorComponent);
			SMComponent->SetSkeletalMesh(Cast<USkeletalMesh>(Mesh));
			SMActor->SetActorScale3D(MeshScale);
			CurrentPreviewActor = GetPreviewActor();
			OnSelectionUpdated(GetCurrentPreviewActor());
		}
	}

	//初始化播放设置
	TargetSelectorObject->IsLoop = false;
	TargetSelectorObject->IsPlay = false;
	TargetSelectorObject->Schedule = 0;


	if (GetCollisionActor())
	{
		UpdateCollosionTransform();
	}
	UpdateStartAndEndAnchor();
}

void FTargetSelectorEditor::InitTargetSelectorEditor(const FInitArg& Arg)
{
	TargetSelectorObject = Cast<UDLGameplayTargetSelectorBase>(Arg.Object);
	const TArray<UObject*>* EditedObjects = GetObjectsCurrentlyBeingEdited();

	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(TargetSelectorObject);

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
						->SetSizeCoefficient(0.4f)
						->AddTab(EditorDetailTabId, ETabState::OpenedTab)
						->AddTab(EditorPreviewTabId, ETabState::OpenedTab)
						->SetHideTabWell(true)
						->SetForegroundTab(EditorDetailTabId)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.6f)
						->AddTab(EditorViewPortTabId, ETabState::OpenedTab)
						->SetHideTabWell(true)
					)
				)
			);

		this->InitAssetEditor(EToolkitMode::Standalone,
		                      Arg.InitToolkitHost,
		                      TargetSelectorEditorAppIdentifier,
		                      StandaloneDefaultLayout,
		                      bCreateDefaultStandaloneMenu,
		                      bCreateDefaultToolbar,
		                      ObjectsToEdit);
	}
	else
	{
		if (!EditedObjects->Contains(TargetSelectorObject))
		{
			AddEditingObject(TargetSelectorObject);
		}
	}

	RegenerateMenusAndToolbars();

	this->ResetPreviewCharacter();

	//默认打开 Detail Tab
	const auto Detail = GetTabManager()->TryInvokeTab(EditorDetailTabId);
	Detail.Get()->ActivateInParent(UserClickedOnTab);
}

UObject* FTargetSelectorEditor::GetTargetObject() const
{
	return TargetSelectorObject;
}

void FTargetSelectorEditor::CreateDetailWidget(const TSharedRef<class FTabManager>& InTabManager)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
		"PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, false);
	DetailsViewArgs.NotifyHook = this;
	DetailsViewArgs.HostTabManager = InTabManager;

	DetailContent = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailContent->SetObject(TargetSelectorObject);
}

void FTargetSelectorEditor::DrawAuxiliaryLine()
{
	ULineBatchComponent* const LineBather = GetPreviewScene()->GetWorld()->ForegroundLineBatcher;
	LineBather->Flush();
	TArray<FBatchedLine> Lines;

	if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket && TargetSelectorObject->IsPlay)
	{
		auto Transform = GetSocketTransform(TargetSelectorObject->AnchorSocketName);

		GetAnchorActor()->SetActorTransform(Transform);
		if (GetCollisionActor())
		{
			UpdateCollosionTransform();
		}
		UpdateStartAndEndAnchor();
	}

	FVector StartCenter(GetAnchorActor()->GetActorLocation());
	FVector EndCenter(GetAnchorActor()->GetActorLocation());

	DrawDebugSphere(GetPreviewScene()->GetWorld(),
	                StartCenter, 5, 16, FColor::Yellow, false, 0, 1);


	FColor LineColor = PreviewObject->CollisionAreaLineColor;
	int32 LineWidth = PreviewObject->CollisionAreaLineWidth;
	if (HasCapsule())
	{
		//胶囊体
		UDLGameplayTargetSelectorCapsuleTrace* CapsuleObject = Cast<UDLGameplayTargetSelectorCapsuleTrace>(
			TargetSelectorObject);
		auto Center = GetAnchorActor()->GetActorTransform();
		StartCenter = Center.TransformPositionNoScale(CapsuleObject->StartPosRelativeAnchor);
		EndCenter = Center.TransformPositionNoScale(CapsuleObject->EndPosRelativeAnchor);

		float Width = CapsuleObject->CapsuleRadius;
		float Height = CapsuleObject->CapsuleHalfHeight;

		int32 Num = 32;
		FVector TopDirection(0, 0, 0);

		FVector Forward = FVector(1, 0, 0);
		FVector Right = FVector(0, 1, 0);
		FVector Up = FVector(0, 0, 1);

		if (Height > Width)
		{
			float HrefLength = 0;
			HrefLength = (Height - Width);
			TopDirection = Up * HrefLength;
			//胶囊体用两个球和连线代表
			//LineBather->DrawCircle(StartCenter - TopDirection, FVector(1, 0, 0), FVector(0, 1, 0), LineColor, Width,
			//                       Num, 1);
			//LineBather->DrawCircle(StartCenter - TopDirection, FVector(1, 0, 0), FVector(0, 0, 1), LineColor, Width,
			//                       Num, 1);
			//LineBather->DrawCircle(StartCenter - TopDirection, FVector(0, 1, 0), FVector(0, 0, 1), LineColor, Width,
			//                       Num, 1);
			DrawDebugSphere(GetPreviewScene()->GetWorld(),
			                StartCenter - TopDirection, Width, Num, LineColor, false, 0, 1, LineWidth);


			//LineBather->DrawCircle(EndCenter - TopDirection, FVector(1, 0, 0), FVector(0, 1, 0), LineColor, Width,
			//                       Num, 1);
			//LineBather->DrawCircle(EndCenter - TopDirection, FVector(1, 0, 0), FVector(0, 0, 1), LineColor, Width,
			//                       Num, 1);
			//LineBather->DrawCircle(EndCenter - TopDirection, FVector(0, 1, 0), FVector(0, 0, 1), LineColor, Width,
			//                       Num, 1);

			DrawDebugSphere(GetPreviewScene()->GetWorld(),
			                EndCenter - TopDirection, Width, Num, LineColor, false, 0, 1, LineWidth);


			Lines.Push(FBatchedLine(StartCenter + TopDirection + Forward * Width,
			                        StartCenter + TopDirection + Forward * Width + TopDirection * -2, LineColor, 0,
			                        LineWidth, 1));
			Lines.Push(FBatchedLine(StartCenter + TopDirection - Forward * Width,
			                        StartCenter + TopDirection - Forward * Width + TopDirection * -2, LineColor, 0,
			                        LineWidth, 1));
			Lines.Push(FBatchedLine(StartCenter + TopDirection + Right * Width,
			                        StartCenter + TopDirection + Right * Width + TopDirection * -2, LineColor, 0,
			                        LineWidth,
			                        1));
			Lines.Push(FBatchedLine(StartCenter + TopDirection - Right * Width,
			                        StartCenter + TopDirection - Right * Width + TopDirection * -2, LineColor, 0,
			                        LineWidth,
			                        1));

			Lines.Push(FBatchedLine(EndCenter + TopDirection + Forward * Width,
			                        EndCenter + TopDirection + Forward * Width + TopDirection * -2, LineColor, 0,
			                        LineWidth,
			                        1));
			Lines.Push(FBatchedLine(EndCenter + TopDirection - Forward * Width,
			                        EndCenter + TopDirection - Forward * Width + TopDirection * -2, LineColor, 0,
			                        LineWidth,
			                        1));
			Lines.Push(FBatchedLine(EndCenter + TopDirection + Right * Width,
			                        EndCenter + TopDirection + Right * Width + TopDirection * -2, LineColor, 0,
			                        LineWidth,
			                        1));
			Lines.Push(FBatchedLine(EndCenter + TopDirection - Right * Width,
			                        EndCenter + TopDirection - Right * Width + TopDirection * -2, LineColor, 0,
			                        LineWidth,
			                        1));

			//连接两个胶囊体的辅助线
			Lines.Push(FBatchedLine(StartCenter + TopDirection + Up * Width, EndCenter + TopDirection + Up * Width,
			                        LineColor, 0, LineWidth, 1));
			Lines.Push(FBatchedLine(StartCenter - TopDirection - Up * Width, EndCenter - TopDirection - Up * Width,
			                        LineColor, 0, LineWidth, 1));

			Lines.Push(FBatchedLine(StartCenter + TopDirection + Right * Width,
			                        EndCenter + TopDirection + Right * Width, LineColor, 0, LineWidth, 1));
			Lines.Push(FBatchedLine(StartCenter - TopDirection + Right * Width,
			                        EndCenter - TopDirection + Right * Width, LineColor, 0, LineWidth, 1));
			Lines.Push(FBatchedLine(StartCenter + TopDirection - Right * Width,
			                        EndCenter + TopDirection - Right * Width, LineColor, 0, LineWidth, 1));
			Lines.Push(FBatchedLine(StartCenter - TopDirection - Right * Width,
			                        EndCenter - TopDirection - Right * Width, LineColor, 0, LineWidth, 1));
		}

		//LineBather->DrawCircle(StartCenter + TopDirection, FVector(1, 0, 0), FVector(0, 1, 0), LineColor, Width,
		//                       Num, 1);
		//LineBather->DrawCircle(StartCenter + TopDirection, FVector(1, 0, 0), FVector(0, 0, 1), LineColor, Width,
		//                       Num, 1);
		//LineBather->DrawCircle(StartCenter + TopDirection, FVector(0, 1, 0), FVector(0, 0, 1), LineColor, Width,
		//                       Num, 1);

		DrawDebugSphere(GetPreviewScene()->GetWorld(),
		                StartCenter + TopDirection, Width, Num, LineColor, false, 0, 1, LineWidth);

		/*LineBather->DrawCircle(EndCenter + TopDirection, FVector(1, 0, 0), FVector(0, 1, 0), LineColor, Width, Num,
		                       1);
		LineBather->DrawCircle(EndCenter + TopDirection, FVector(1, 0, 0), FVector(0, 0, 1), LineColor, Width, Num,
		                       1);
		LineBather->DrawCircle(EndCenter + TopDirection, FVector(0, 1, 0), FVector(0, 0, 1), LineColor, Width, Num,
		                       1);*/

		DrawDebugSphere(GetPreviewScene()->GetWorld(),
		                EndCenter + TopDirection, Width, Num, LineColor, false, 0, 1, LineWidth);
	}
	else if (HasBox())
	{
		//盒子
		UDLGameplayTargetSelectorBoxTrace* BoxObject = Cast<UDLGameplayTargetSelectorBoxTrace>(TargetSelectorObject);
		auto Center = GetAnchorActor()->GetActorTransform();
		StartCenter = (Center.TransformPositionNoScale(BoxObject->StartPosRelativeAnchor));
		EndCenter = (Center.TransformPositionNoScale(BoxObject->EndPosRelativeAnchor));

		FVector MinPos = BoxObject->HalfSize;
		FVector MaxPos = -1 * MinPos;
		FBox Box(MinPos, MaxPos);

		FTransform StartTransforms = GetCollisionActor()->GetTransform();
		FTransform EndTransformed = GetCollisionActor()->GetTransform();

		StartTransforms.SetTranslation(StartCenter);
		EndTransformed.SetTranslation(EndCenter);

		LineBather->DrawBox(Box, StartTransforms.ToMatrixWithScale(), LineColor, 1);
		LineBather->DrawBox(Box, EndTransformed.ToMatrixWithScale(), LineColor, 1);

		FVector Dir = EndCenter - StartCenter;
		float Maxlength = Dir.Size();
		Dir.Normalize();

		float Length = Maxlength + BoxObject->HalfSize.X * 2;
		FQuat Squat = GetCollisionActor()->GetTransform().GetRotation();

		FVector OnePos = FVector(-1, 1, 1) * BoxObject->HalfSize;

		OnePos = UKismetMathLibrary::Quat_RotateVector(Squat, OnePos);
		Lines.Push(FBatchedLine(StartCenter - OnePos,
		                        StartCenter - OnePos + Dir * Length, LineColor,
		                        0, LineWidth, 1));

		OnePos = FVector(-1, -1, 1) * BoxObject->HalfSize;
		OnePos = UKismetMathLibrary::Quat_RotateVector(Squat, OnePos);
		Lines.Push(FBatchedLine(StartCenter - OnePos,
		                        StartCenter - OnePos + Dir * Length, LineColor,
		                        0, LineWidth, 1));

		OnePos = FVector(-1, -1, -1) * BoxObject->HalfSize;
		OnePos = UKismetMathLibrary::Quat_RotateVector(Squat, OnePos);
		Lines.Push(FBatchedLine(StartCenter - OnePos,
		                        StartCenter - OnePos + Dir * Length,
		                        LineColor, 0, LineWidth, 1));

		OnePos = FVector(-1, 1, -1) * BoxObject->HalfSize;
		OnePos = UKismetMathLibrary::Quat_RotateVector(Squat, OnePos);
		Lines.Push(FBatchedLine(StartCenter - OnePos,
		                        StartCenter - OnePos + Dir * Length, LineColor,
		                        0, LineWidth, 1));
	}
	else if (HasSphere())
	{
		//球体
		UDLGameplayTargetSelectorShapeTraceBase* Object = Cast<UDLGameplayTargetSelectorShapeTraceBase>(
			TargetSelectorObject);

		auto Center = GetAnchorActor()->GetActorTransform();
		StartCenter = (Center.TransformPositionNoScale(Object->StartPosRelativeAnchor));
		EndCenter = (Center.TransformPositionNoScale(Object->EndPosRelativeAnchor));
		FVector dir = EndCenter - StartCenter;

		auto testdir = GetCollisionActor()->GetActorForwardVector();
		testdir.Normalize();
		DrawDebugDirectionalArrow(GetPreviewScene()->GetWorld(), GetCollisionActor()->GetActorLocation(),
		                          GetCollisionActor()->GetActorLocation() + testdir * 10, 60, FColor::Yellow);

		float Radius = Cast<UDLGameplayTargetSelectorSphereTrace>(Object)->Radius;
		int32 Num = 32;
		/*LineBather->DrawCircle(StartCenter, FVector(1, 0, 0), FVector(0, 1, 0), LineColor, Radius, Num, 1);

		LineBather->DrawCircle(StartCenter, FVector(1, 0, 0), FVector(0, 0, 1), LineColor, Radius, Num, 1);

		LineBather->DrawCircle(StartCenter, FVector(0, 1, 0), FVector(0, 0, 1), LineColor, Radius, Num, 1);*/

		DrawDebugSphere(GetPreviewScene()->GetWorld(),
		                StartCenter, Radius, Num, LineColor, false, 0, 1, LineWidth);

		/*LineBather->DrawCircle(EndCenter, FVector(1, 0, 0), FVector(0, 1, 0), LineColor, Radius, Num, 1);

		LineBather->DrawCircle(EndCenter, FVector(1, 0, 0), FVector(0, 0, 1), LineColor, Radius, Num, 1);

		LineBather->DrawCircle(EndCenter, FVector(0, 1, 0), FVector(0, 0, 1), LineColor, Radius, Num, 1);*/

		DrawDebugSphere(GetPreviewScene()->GetWorld(),
		                EndCenter, Radius, Num, LineColor, false, 0, 1, LineWidth);

		float maxlength = dir.Size();
		dir.Normalize();
		FVector right = FVector::CrossProduct(FVector::UpVector, dir);
		right.Normalize();

		Lines.Push(FBatchedLine(StartCenter + (right * Radius), (StartCenter + (right * Radius)) + dir * maxlength,
		                        LineColor, 0, LineWidth, 1));
		Lines.Push(FBatchedLine(StartCenter - (right * Radius), (StartCenter - (right * Radius)) + dir * maxlength,
		                        LineColor, 0, LineWidth, 1));
		Lines.Push(FBatchedLine(StartCenter + FVector::UpVector * Radius,
		                        (StartCenter + FVector::UpVector * Radius) + dir * maxlength, LineColor, 0, LineWidth,
		                        1));
		Lines.Push(FBatchedLine(StartCenter - FVector::UpVector * Radius,
		                        (StartCenter - FVector::UpVector * Radius) + dir * maxlength, LineColor, 0, LineWidth,
		                        1));
	}
	else if (HasPoint() || HasPoints())
	{
		FlushPersistentDebugLines(GetPreviewScene()->GetWorld());
		for (int32 i = 0; i < PointArray.Num(); i++)
		{
			FVector Position = PointArray[i]->GetActorLocation();
			FTransform PointFTransform = PointArray[i]->GetActorTransform();

			DrawDebugSphere(GetPreviewScene()->GetWorld(), Position, 5, 16, LineColor);

			FVector Dir = Position - GetAnchorActor()->GetActorLocation();
			Dir.Normalize();
			FVector ArrowPosition = Position - Dir * 6;
			DrawDebugDirectionalArrow(GetPreviewScene()->GetWorld(), GetAnchorActor()->GetActorLocation(),
			                          ArrowPosition, 60, PreviewObject->ToCollisionLineColor);

			Cast<APointActor>(PointArray[i])->ArrowComponent->SetWorldTransform(PointFTransform);

			Cast<APointActor>(PointArray[i])->ArrowComponent->EditorScale = PreviewObject->ArrowScale;

			Cast<APointActor>(PointArray[i])->ArrowComponent->ArrowColor = PreviewObject->ArrowColor;

			const auto CoordinatesObject = Cast<UDLGameplayTargetSelectorCoordinateCollection>(TargetSelectorObject);
			FTransform AddTrans = FTransform::Identity;
			if (CoordinatesObject)
			{
				AddTrans = CoordinatesObject->GetRevisedOffsetByIndex(i);
			}

			if (AddTrans.GetLocation().Size() != 0 && Cast<UDLTargetSelectorRevisedByDistance>(
				CoordinatesObject->GlobalRevised))
			{
				float Min = Cast<UDLTargetSelectorRevisedByDistance>(CoordinatesObject->GlobalRevised)->MinDistance;
				float Max = Cast<UDLTargetSelectorRevisedByDistance>(CoordinatesObject->GlobalRevised)->MaxDistance;
				FVector CamDir = Position - CameraTransform.GetLocation();
				CamDir.Normalize();
				FVector Right = FVector(0, 1, 0);
				Right.Normalize();
				FVector Up = FVector(0, 0, 1);
				Up.Normalize();

				auto Rotator = FRotationMatrix::MakeFromX(CamDir).Rotator();
				Right = UKismetMathLibrary::Quat_RotateVector(Rotator.Quaternion(), Right);
				Up = UKismetMathLibrary::Quat_RotateVector(Rotator.Quaternion(), Up);

				//DrawDebugSphere(GetPreviewScene()->GetWorld(), Position, Min, 32, FColor::Green);
				//DrawDebugSphere(GetPreviewScene()->GetWorld(), Position, Max, 32, FColor::Green);

				DrawDebugCircle(GetPreviewScene()->GetWorld(), Position, Min, 64, FColor::Green,
				                false, 0, 1, 0, Up, Right, false);

				DrawDebugCircle(GetPreviewScene()->GetWorld(), Position, Max, 64, FColor::Green,
				                false, 0, 1, 0, Up, Right, false);


				DrawDebugSphere(GetPreviewScene()->GetWorld(),
				                CoordinatesObject->GetFinalTransformByIndex(i).GetLocation(), 5, 16, LineColor);
			}
		}
	}

	if (HasRandomPoints())
	{
		const auto CoordinatesObject = Cast<UDLGameplayTargetSelectorDynamicGeneration>(TargetSelectorObject);
		FlushPersistentDebugLines(GetPreviewScene()->GetWorld());
		FQuat MeshRotation = TargetSelectorObject->PreviewMeshTransform.GetRotation();
		FTransform SocketTransform = FTransform::Identity;
		if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket && TargetSelectorObject->PreviewMesh)
		{
			USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(TargetSelectorObject->PreviewMesh.Get());
			if (SkeletalMesh)
			{
				USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(
					GetPreviewActor()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
				SocketTransform = SkeletalMeshComponent->GetSocketTransform(TargetSelectorObject->AnchorSocketName);
				MeshRotation = SocketTransform.GetRotation();
			}
		}


		for (int32 k = 0; k < PointArray.Num(); k++)
		{
			FVector LocalPos = PointArray[k]->GetActorLocation();
			switch (CoordinatesObject->ShapeType)
			{
			case ECoordinateConstraintShape::Cylinder:
				DrawDebugCylinder(GetPreviewScene()->GetWorld(), LocalPos,
				                  FVector(LocalPos.X, LocalPos.Y, LocalPos.Z + CoordinatesObject->MinHeight),
				                  CoordinatesObject->MinWidth, 16,
				                  FColor::Green);

				if (CoordinatesObject->MaxWidth > CoordinatesObject->MinWidth)
				{
					DrawDebugCylinder(GetPreviewScene()->GetWorld(), PointArray[k]->GetActorLocation(),
					                  FVector(LocalPos.X, LocalPos.Y, LocalPos.Z + CoordinatesObject->MinHeight),
					                  CoordinatesObject->MaxWidth, 16,
					                  FColor::Green);
				}
				break;
			case ECoordinateConstraintShape::Box:

				//const FVector RotateVector = UKismetMathLibrary::Quat_RotateVector(Rotation, PointArray[k]->GetActorLocation());
				DrawDebugBox(GetPreviewScene()->GetWorld(), PointArray[k]->GetActorLocation(),
				             FVector(CoordinatesObject->MinLength, CoordinatesObject->MinWidth,
				                     CoordinatesObject->MinHeight), MeshRotation, FColor::Green);
				DrawDebugBox(GetPreviewScene()->GetWorld(), PointArray[k]->GetActorLocation(),
				             FVector(CoordinatesObject->MaxLength, CoordinatesObject->MaxWidth,
				                     CoordinatesObject->MaxHeight), MeshRotation, FColor::Green);
				break;
			case ECoordinateConstraintShape::Sphere:

				DrawDebugSphere(GetPreviewScene()->GetWorld(),
				                PointArray[k]->GetActorLocation(), CoordinatesObject->MinWidth, 16, FColor::Green);

				DrawDebugSphere(GetPreviewScene()->GetWorld(),
				                PointArray[k]->GetActorLocation(), CoordinatesObject->MaxWidth, 16, FColor::Green);
				break;
			}

			for (int32 i = 0; i < DotArray[k].Num(); i++)
			{
				const FVector RotateVector = UKismetMathLibrary::Quat_RotateVector(
					MeshRotation * CoordinatesObject->RelativeTransformArray[k].Coordinate.GetRotation(),
					DotArray[k][i].GetLocation());
				if (CoordinatesObject->IsShowMinDistance)
				{
					DrawDebugSphere(GetPreviewScene()->GetWorld(),
					                PointArray[k]->GetActorLocation() +
					                RotateVector,
					                CoordinatesObject->MinDistanceToDot, 8,
					                FColor::Red);
				}
				else
				{
					DrawDebugPoint(GetPreviewScene()->GetWorld(),
					               PointArray[k]->GetActorLocation() + RotateVector, 5, FColor::Red);
				}
			}
		}
	}

	if (GetCollisionActor())
	{
		Lines.Push(FBatchedLine(GetAnchorActor()->GetActorLocation(), GetCollisionActor()->GetActorLocation(),
		                        PreviewObject->ToAnchorLineColor,
		                        0,
		                        PreviewObject->ToAnchorLineWidth, 1));
	}
	Lines.Push(FBatchedLine(GetAnchorActor()->GetActorLocation(), StartCenter, PreviewObject->ToCollisionLineColor, 0,
	                        PreviewObject->ToCollisionLineWidth, 1));
	Lines.Push(FBatchedLine(GetAnchorActor()->GetActorLocation(), EndCenter, PreviewObject->ToCollisionLineColor, 0,
	                        PreviewObject->ToCollisionLineWidth, 1));

	LineBather->DrawLines(Lines);
}

void FTargetSelectorEditor::CreatCoordinateDot()
{
	for (auto& i : DotArray)
	{
		const TArray<FTransform> CreatDots = Cast<UDLGameplayTargetSelectorDynamicGeneration>(TargetSelectorObject)->
			CreatRandomCoordinate();
		this->OnUpDateScene.ExecuteIfBound();

		i.Value = CreatDots;
	}
}

void FTargetSelectorEditor::ClearCreatCoordinateDot()
{
	for (auto& i : DotArray)
	{
		i.Value.Empty();
	}
}

void FTargetSelectorEditor::RevisedTransform() const
{
	if (HasPoints())
	{
		const auto CoordinatesObject = Cast<UDLGameplayTargetSelectorCoordinateCollection>(TargetSelectorObject);
		CoordinatesObject->RevisedTransform();

		const int32 Num = CoordinatesObject->RelativeTransformArray.Num();
		FTransform SocketTransform = FTransform::Identity;
		if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket && TargetSelectorObject->PreviewMesh)
		{
			USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(TargetSelectorObject->PreviewMesh.Get());
			if (SkeletalMesh)
			{
				USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(
					GetPreviewActor()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
				SocketTransform = SkeletalMeshComponent->GetSocketTransform(TargetSelectorObject->AnchorSocketName);
			}
		}
		for (int32 i = 0; i < Num; i++)
		{
			const auto Position = CoordinatesObject->RelativeTransformArray[i];
			FQuat Squat = TargetSelectorObject->PreviewMeshTransform.GetRotation();
			FVector Location = TargetSelectorObject->PreviewMeshTransform.GetTranslation();
			if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket && TargetSelectorObject->
				PreviewMesh)
			{
				Squat = SocketTransform.GetRotation();
				Location = SocketTransform.GetLocation();
			}
			const FVector Offset = Position.Coordinate.GetLocation();
			const FVector RotateVector = UKismetMathLibrary::Quat_RotateVector(Squat, Offset);

			FTransform Point;
			Point.SetLocation(Location + RotateVector);
			Point.SetRotation(
				Squat * CoordinatesObject->RelativeTransformArray[i].Coordinate.GetRotation());
		}
		this->OnUpDateScene.ExecuteIfBound();
	}
}

void FTargetSelectorEditor::RestTransform() const
{
	if (HasPoints())
	{
		const auto CoordinatesObject = Cast<UDLGameplayTargetSelectorCoordinateCollection>(TargetSelectorObject);
		const int32 Num = CoordinatesObject->RelativeTransformArray.Num();
		FTransform SocketTransform = FTransform::Identity;
		if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket && TargetSelectorObject->PreviewMesh)
		{
			USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(TargetSelectorObject->PreviewMesh.Get());
			if (SkeletalMesh)
			{
				USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(
					GetPreviewActor()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
				SocketTransform = SkeletalMeshComponent->GetSocketTransform(TargetSelectorObject->AnchorSocketName);
			}
		}
		for (int32 i = 0; i < Num; i++)
		{
			const auto Position = CoordinatesObject->RelativeTransformArray[i];
			FQuat Squat = TargetSelectorObject->PreviewMeshTransform.GetRotation();
			FVector Location = TargetSelectorObject->PreviewMeshTransform.GetTranslation();
			if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket && TargetSelectorObject->
				PreviewMesh)
			{
				Squat = SocketTransform.GetRotation();
				Location = SocketTransform.GetLocation();
			}
			const FVector Offset = Position.Coordinate.GetLocation();
			const FVector RotateVector = UKismetMathLibrary::Quat_RotateVector(Squat, Offset);

			FTransform Point;
			Point.SetLocation(Location + RotateVector);
			Point.SetRotation(
				Squat * CoordinatesObject->RelativeTransformArray[i].Coordinate.GetRotation());
		}
		CoordinatesObject->ClearRevisedOffset();
		this->OnUpDateScene.ExecuteIfBound();
	}
}

void FTargetSelectorEditor::UpdateCameraTransform(FViewportCameraTransform Trans)
{
	CameraTransform = Trans;
}

void FTargetSelectorEditor::SetCollisionColor() const
{
	AActor* TmpActor = GetCollisionActor();

	//设置锚点颜色
	if (Cast<AAnchorActor>(GetAnchorActor())->SphereComponent->ShapeColor != PreviewObject->AnchorLineColor)
	{
		Cast<AAnchorActor>(GetAnchorActor())->SphereComponent->ShapeColor = PreviewObject->AnchorLineColor;
	}

	if (HasCapsule())
	{
		if (Cast<ACapsuleActor>(TmpActor)->CapsuleComponent->ShapeColor != PreviewObject->CollisionLineColor)
		{
			Cast<ACapsuleActor>(TmpActor)->CapsuleComponent->ShapeColor = PreviewObject->CollisionLineColor;
		}
	}
	else if (HasBox())
	{
		if (Cast<ABoxActor>(TmpActor)->BoxComponent->ShapeColor != PreviewObject->CollisionLineColor)
		{
			Cast<ABoxActor>(TmpActor)->BoxComponent->ShapeColor = PreviewObject->CollisionLineColor;
		}
	}
	else if (HasSphere())
	{
		if (Cast<ASphereActor>(TmpActor)->SphereComponent->ShapeColor != PreviewObject->CollisionLineColor)
		{
			Cast<ASphereActor>(TmpActor)->SphereComponent->ShapeColor = PreviewObject->CollisionLineColor;
		}
	}
}

void FTargetSelectorEditor::SetPlayAnimation(FName Type) const
{
	USkeletalMeshComponent* Com = nullptr;
	//bool cc = CurrentACharacter != nullptr;
	//auto xx = TargetSelectorObject->PreviewMeshAnimation.Get();
	if (SkeletalMeshActor && TargetSelectorObject->PreviewMeshAnimation)
	{
		if (Cast<ASkeletalMeshActor>(SkeletalMeshActor))
		{
			Com = SkeletalMeshActor->GetSkeletalMeshComponent();
		}
	}
	else if (CurrentACharacter != nullptr && TargetSelectorObject->PreviewMeshAnimation.Get())
	{
		Com = CurrentACharacter->GetMesh();
	}

	if (Com == nullptr)
	{
		return;
	}


	if (Type == TEXT("Schedule"))
	{
		Com->PlayAnimation(
			TargetSelectorObject->PreviewMeshAnimation.Get(), TargetSelectorObject->IsLoop);

		TargetSelectorObject->IsLoop = false;
		TargetSelectorObject->IsPlay = false;

		Com->Play(false);
		Com->Stop();

		const auto MaxLength = Cast<UAnimSequenceBase>(TargetSelectorObject->PreviewMeshAnimation.Get())->
			SequenceLength;
		Com->SetPosition(TargetSelectorObject->Schedule * MaxLength, false);
		if (TargetSelectorObject->Schedule == 0)
		{
			Com->PlayAnimation(nullptr, false);
		}
	}
	else
	{
		if (TargetSelectorObject->IsPlay)
		{
			Com->PlayAnimation(
				TargetSelectorObject->PreviewMeshAnimation.Get(), TargetSelectorObject->IsLoop);
		}
		else
		{
			Com->Play(false);
			Com->Stop();
			Com->SetPosition(TargetSelectorObject->Schedule, false);
		}
	}
}

void FTargetSelectorEditor::UpdateCollosionTransform() const
{
	const FQuat Squat = GetAnchorActor()->GetTransform().GetRotation();
	const FVector Location = GetAnchorActor()->GetTransform().GetLocation();

	const FVector Offset = Cast<UDLGameplayTargetSelectorShapeTraceBase>(TargetSelectorObject)->
	                       CollisionTransform.GetLocation();

	const FVector RotateVector = UKismetMathLibrary::Quat_RotateVector(Squat, Offset);
	GetCollisionActor()->SetActorLocation(Location + RotateVector);

	//带动碰撞盒子旋转
	const FQuat parRot = GetAnchorActor()->GetTransform().GetRotation() *
		Cast<UDLGameplayTargetSelectorShapeTraceBase>(TargetSelectorObject)->CollisionTransform.
		                                                                     GetRotation();
	GetCollisionActor()->SetActorRotation(parRot);
}

void FTargetSelectorEditor::SaveAsset_Execute()
{
	ITargetSelectorEditor::SaveAsset_Execute();
}

FLinearColor FTargetSelectorEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::Blue;
}

FName FTargetSelectorEditor::GetToolkitFName() const
{
	return TEXT("TargetSelectorEditor");
}

FText FTargetSelectorEditor::GetBaseToolkitName() const
{
	return FText::FromString(TEXT("TargetSelector"));
}

FString FTargetSelectorEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("TargetSelector ");
}

void FTargetSelectorEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
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
		const TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			[
				SNew(SButton)
				.Text(FText::FromString(UTF8_TO_TCHAR("选择模型")))
			.OnClicked_Lambda([this]()
				             {
					             const auto Actor = this->GetPreviewActor();
					             if (Actor)
					             {
						             this->OnSelectionUpdated(Actor);
						             this->OnSelectActor.ExecuteIfBound(Actor);
					             }
					             UE_LOG(LogTemp, Warning, TEXT("PreviewMesh"));
					             return FReply::Handled();
				             })
			]
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			  .Padding(FMargin(0, 20, 0, 0))
			[
				SNew(SButton)
				.Text(FText::FromString(UTF8_TO_TCHAR("选择碰撞点")))
			.OnClicked_Lambda([this]()
				             {
					             const auto Actor = this->GetCollisionActor();
					             if (Actor)
					             {
						             this->OnSelectionUpdated(Actor);
						             this->OnSelectActor.ExecuteIfBound(Actor);
					             }
					             UE_LOG(LogTemp, Warning, TEXT("Collision"));
					             return FReply::Handled();
				             })
			];

		TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(STargetSelectorViewport)
				.TargetSelectorEditor(SharedThis(this))
			]
			+ SOverlay::Slot()
			[
				SNew(SBox)
				.WidthOverride(100)
			.HeightOverride(100)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			.Padding(FMargin(0, 100, 0, 0))
				[
					VerticalBox
				]
			]
		];
		this->ButtonArray = VerticalBox;
		return DockTab;
	};

	auto CreatePreviewDetails = [this](const FSpawnTabArgs& Args)-> TSharedRef<SDockTab>
	{
		FAdvancedPreviewSceneModule& AdvancedPreviewSceneModule = FModuleManager::LoadModuleChecked<
			FAdvancedPreviewSceneModule>("AdvancedPreviewScene");

		TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		[
			AdvancedPreviewSceneModule.CreateAdvancedPreviewSceneSettingsWidget(
				PreviewScene.ToSharedRef(), PreviewObject)
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

void FTargetSelectorEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	InTabManager->UnregisterTabSpawner(EditorDetailTabId);
	InTabManager->UnregisterTabSpawner(EditorViewPortTabId);
	InTabManager->UnregisterTabSpawner(EditorPreviewTabId);

	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

void FTargetSelectorEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
                                             FEditPropertyChain* PropertyThatChanged)
{
	UE_LOG(LogTemp, Log, TEXT("NotifyPostChange %s"), *PropertyChangedEvent.GetPropertyName().ToString());

	auto CurrentNode = PropertyThatChanged->GetActiveNode();
	while (CurrentNode)
	{
		UE_LOG(LogTemp, Log, TEXT("GetActiveNode %s"), *CurrentNode->GetValue()->GetFName().ToString());

		CurrentNode = CurrentNode->GetNextNode();
	}

	//static FName RelativeOffset(TEXT("RelativeOffset"));
	static FName PreviewMesh(TEXT("PreviewMesh"));
	static FName PreviewActor(TEXT("PreviewActor"));
	static FName PreviewMeshTransform(TEXT("PreviewMeshTransform"));
	static FName AnchorSocketName(TEXT("AnchorSocketName"));
	static FName Radius(TEXT("Radius"));
	static FName HalfSize(TEXT("HalfSize"));
	static FName AnchorSize(TEXT("AnchorSize"));
	static FName CapsuleRadius(TEXT("CapsuleRadius"));
	static FName CapsuleHalfHeight(TEXT("CapsuleHalfHeight"));
	static FName CollisionTransform(TEXT("CollisionTransform"));
	static FName AnchorSelectType(TEXT("AnchorSelectType"));
	//static FName TraceLength(TEXT("TraceLength"));
	//static FName RelativeTransform(TEXT("RelativeTransform"));
	static FName PreviewMeshAnimation(TEXT("PreviewMeshAnimation"));
	static FName ResetPreviewTransform(TEXT("ResetPreviewTransform"));

	static FName AnimationInfoLoop(TEXT("IsLoop"));
	static FName AnimationInfoPlay(TEXT("IsPlay"));
	static FName AnimationInfoSchedule(TEXT("Schedule"));

	CurrentNode = PropertyThatChanged->GetActiveMemberNode();
	if (CurrentNode)
	{
		FProperty* Property = CurrentNode->GetValue();
		if (Property)
		{
			if (Property->GetFName() == PreviewMesh || Property->GetFName() == PreviewActor)
			{
				this->ResetPreviewCharacter();
			}
			else if (Property->GetFName() == PreviewMeshTransform)
			{
				if (IsAnchorLock && GetPreviewActor())
				{
					FTransform* FMesh = Property->ContainerPtrToValuePtr<FTransform>(TargetSelectorObject);

					//更新Mesh位置
					FTransform AddTrans = *FMesh;
					AddTrans.SetLocation(
						AddTrans.GetLocation() + TargetSelectorObject->ResetPreviewTransform.GetLocation());
					AddTrans.SetRotation(
						AddTrans.GetRotation() * TargetSelectorObject->ResetPreviewTransform.GetRotation());
					GetPreviewActor()->SetActorTransform(AddTrans);
					TargetSelectorObject->ResetPreviewTransform.SetScale3D(
						GetPreviewActor()->GetActorTransform().GetScale3D());
					//带动锚点
					//需要判断是否有锚点

					if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket)
					{
						FTransform SocketTransform = GetSocketTransform(TargetSelectorObject->AnchorSocketName);
						TargetSelectorObject->AnchorTransform = SocketTransform;
						GetAnchorActor()->SetActorTransform(SocketTransform);
					}
					else if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::ActorTransform)
					{
						TargetSelectorObject->AnchorTransform = TargetSelectorObject->PreviewMeshTransform;
						GetAnchorActor()->SetActorTransform(TargetSelectorObject->PreviewMeshTransform);
					}

					//带动碰撞盒子位置
					if (GetCollisionActor())
					{
						UpdateCollosionTransform();
					}

					UpdateStartAndEndAnchor();
				}
			}
			else if (Property->GetFName() == AnchorSocketName)
			{
				//else if 输入SocketName时

				if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::ActorTransform)
				{
				}
				else
				{
					//带动锚点
					TargetSelectorObject->AnchorTransform = GetSocketTransform(TargetSelectorObject->AnchorSocketName);

					GetAnchorActor()->SetActorTransform(TargetSelectorObject->AnchorTransform);
					//带动碰撞盒子
					if (GetCollisionActor())
					{
						UpdateCollosionTransform();
					}
					UpdateStartAndEndAnchor();
				}
			}
			else if (Property->GetFName() == Radius)
			{
				float* Value = Property->ContainerPtrToValuePtr<float>(TargetSelectorObject);
				ASphereActor* SphereActor = Cast<ASphereActor>(GetCollisionActor());
				SphereActor->SphereComponent->SetSphereRadius(*Value);
				UpdateStartAndEndAnchor();
			}
			else if (Property->GetFName() == HalfSize)
			{
				FVector* Value = Property->ContainerPtrToValuePtr<FVector>(TargetSelectorObject);
				ABoxActor* BoxActor = Cast<ABoxActor>(GetCollisionActor());
				BoxActor->BoxComponent->SetBoxExtent(*Value);
				UpdateStartAndEndAnchor();
			}
			else if (Property->GetFName() == CapsuleRadius)
			{
				float* Value = Property->ContainerPtrToValuePtr<float>(TargetSelectorObject);
				ACapsuleActor* CapsuleActor = Cast<ACapsuleActor>(GetCollisionActor());
				CapsuleActor->CapsuleComponent->SetCapsuleRadius(*Value);
				UpdateStartAndEndAnchor();
			}
			else if (Property->GetFName() == CapsuleHalfHeight)
			{
				float* Value = Property->ContainerPtrToValuePtr<float>(TargetSelectorObject);
				ACapsuleActor* CapsuleActor = Cast<ACapsuleActor>(GetCollisionActor());
				CapsuleActor->CapsuleComponent->SetCapsuleHalfHeight(*Value);
				UpdateStartAndEndAnchor();
			}
			else if (Property->GetFName() == AnchorSize)
			//else if 改变锚点size
			{
				float* Value = Property->ContainerPtrToValuePtr<float>(TargetSelectorObject);
				AAnchorActor* AnchorActor = Cast<AAnchorActor>(GetAnchorActor());
				AnchorActor->SphereComponent->SetSphereRadius(*Value);
			}
			//更新碰撞盒子Transform
			else if (Property->GetFName() == CollisionTransform)
			{
				//带动碰撞盒子位置
				if (GetCollisionActor())
				{
					UpdateCollosionTransform();
				}

				UpdateStartAndEndAnchor();
			}
			else if (Property->GetFName() == AnchorSelectType)
			{
				EAnchorSelectType* SelectType = Property->ContainerPtrToValuePtr<EAnchorSelectType>(
					TargetSelectorObject);
				if (*SelectType == EAnchorSelectType::ActorTransform)
				{
					//带动锚点
					TargetSelectorObject->AnchorTransform.SetLocation(
						TargetSelectorObject->PreviewMeshTransform.GetLocation());
					GetAnchorActor()->SetActorTransform(TargetSelectorObject->PreviewMeshTransform);
				}
				else if (*SelectType == EAnchorSelectType::Socket)
				{
					//带动锚点
					TargetSelectorObject->AnchorTransform.SetLocation(
						GetSocketLocation(TargetSelectorObject->AnchorSocketName));
					GetAnchorActor()->SetActorTransform(TargetSelectorObject->AnchorTransform);
				}
				//带动碰撞盒子
				if (Cast<UDLGameplayTargetSelectorShapeTraceBase>(TargetSelectorObject))
				{
					UpdateCollosionTransform();
				}
				UpdateStartAndEndAnchor();
			}
			else if (Property->GetFName() == ResetPreviewTransform)
			{
				if (IsAnchorLock && GetPreviewActor())
				{
					//更新Mesh位置
					FTransform AddTrans = TargetSelectorObject->PreviewMeshTransform;
					AddTrans.SetLocation(
						AddTrans.GetLocation() + TargetSelectorObject->ResetPreviewTransform.GetLocation());
					AddTrans.SetRotation(
						AddTrans.GetRotation() * TargetSelectorObject->ResetPreviewTransform.GetRotation());
					AddTrans.SetScale3D(TargetSelectorObject->ResetPreviewTransform.GetScale3D());
					TargetSelectorObject->PreviewMeshTransform.SetScale3D(
						TargetSelectorObject->ResetPreviewTransform.GetScale3D());
					GetPreviewActor()->SetActorTransform(AddTrans);
				}
			}
			else if (Property->GetFName() == PreviewMeshAnimation)
			{
				this->SetPlayAnimation(PreviewMeshAnimation);
			}
			else if (Property->GetFName() == AnimationInfoLoop || Property->GetFName() == AnimationInfoPlay ||
				Property->GetFName() == AnimationInfoSchedule)
			{
				this->SetPlayAnimation(Property->GetFName());

				if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket)
				{
					auto Transform = GetSocketTransform(TargetSelectorObject->AnchorSocketName);

					GetAnchorActor()->SetActorTransform(Transform);
					if (GetCollisionActor())
					{
						UpdateCollosionTransform();
					}
					UpdateStartAndEndAnchor();
				}
			}
			else
			{
				UpdateStartAndEndAnchor();
			}
		}
	}
}

void FTargetSelectorEditor::OnClose()
{
}


void FTargetSelectorEditor::OnSelectionUpdated(AActor* Actor)
{
	if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket)
	{
		const auto Transform = GetSocketTransform(TargetSelectorObject->AnchorSocketName);
		GetAnchorActor()->SetActorTransform(Transform);
	}

	CurrentPreviewActor = Actor;
	if (Actor != nullptr)
	{
		//else if拖动的是碰撞盒子
		if (GetCollisionActor() && CurrentPreviewActor->GetClass()->IsChildOf<AShapeActor>()
			&& (!Cast<UDLGameplayTargetSelectorShapeTraceBase>(TargetSelectorObject)->CollisionTransform.Equals(
				GetCollisionActor()->GetActorTransform())))
		{
			FVector Offset = GetCollisionActor()->GetActorTransform().GetLocation() -
				GetAnchorActor()->GetActorTransform().GetLocation();

			Offset = UKismetMathLibrary::Quat_RotateVector(
				GetAnchorActor()->GetActorTransform().GetRotation().Inverse(), Offset);

			Cast<UDLGameplayTargetSelectorShapeTraceBase>(TargetSelectorObject)->CollisionTransform.SetLocation(Offset);
			Cast<UDLGameplayTargetSelectorShapeTraceBase>(TargetSelectorObject)->CollisionTransform.SetRotation(
				GetCollisionActor()->GetTransform().GetRotation() * GetAnchorActor()->GetTransform().GetRotation().
				Inverse());

			UpdateStartAndEndAnchor();
		}
		//else if拖动的是MeshActor
		else if (
			GetPreviewActor() && (CurrentPreviewActor->GetClass()->IsChildOf<AStaticMeshActor>() ||
				CurrentPreviewActor->GetClass()->IsChildOf<ASkeletalMeshActor>() ||
				CurrentPreviewActor->GetClass()->IsChildOf<ACharacter>()))
		{
			if (IsAnchorLock &&
				(CurrentPreviewActor->GetClass()->IsChildOf<ASkeletalMeshActor>() ||
					CurrentPreviewActor->GetClass()->IsChildOf<AStaticMeshActor>() ||
					CurrentPreviewActor->GetClass()->IsChildOf<ACharacter>()))
			{
				//带动锚点
				if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket &&
					CurrentPreviewActor->GetClass()->IsChildOf<ASkeletalMeshActor>())
				{
					const ASkeletalMeshActor* SMActor = Cast<ASkeletalMeshActor>(GetPreviewActor());
					const USkeletalMeshComponent* Component = SMActor->GetSkeletalMeshComponent();
					const FTransform SocketTransform = Component->GetSocketTransform(
						TargetSelectorObject->AnchorSocketName);
					TargetSelectorObject->AnchorTransform = SocketTransform;
					GetAnchorActor()->SetActorTransform(SocketTransform);
				}
				else if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::ActorTransform)
				{
					TargetSelectorObject->AnchorTransform = TargetSelectorObject->PreviewMeshTransform;
					GetAnchorActor()->SetActorTransform(TargetSelectorObject->PreviewMeshTransform);
				}

				//更新Mesh位置
				FTransform TmpTrans = GetPreviewActor()->GetActorTransform();

				TargetSelectorObject->PreviewMeshTransform.SetLocation(
					TmpTrans.GetTranslation() - TargetSelectorObject->ResetPreviewTransform.GetTranslation());
				TargetSelectorObject->PreviewMeshTransform.SetRotation(
					GetPreviewActor()->GetActorTransform().GetRotation() * TargetSelectorObject->ResetPreviewTransform.
					GetRotation().Inverse());
				TargetSelectorObject->PreviewMeshTransform.SetScale3D(
					GetPreviewActor()->GetActorTransform().GetScale3D());
				TargetSelectorObject->ResetPreviewTransform.SetScale3D(
					GetPreviewActor()->GetActorTransform().GetScale3D());
				//带动碰撞盒子位置
				if (GetCollisionActor())
				{
					UpdateCollosionTransform();
				}

				UpdateStartAndEndAnchor();
			}

			//PrimitiveComponent代理相关
			for (UActorComponent* Component : Actor->GetComponents())
			{
				if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Component))
				{
					if (!PrimitiveComponent->SelectionOverrideDelegate.IsBound())
					{
						PrimitiveComponent->SelectionOverrideDelegate =
							UPrimitiveComponent::FSelectionOverride::CreateSP(
								this, &FTargetSelectorEditor::IsComponentSelected);
					}
					PrimitiveComponent->PushSelectionToProxy();
				}
			}
		}
		// 拖的是点
		else if (HasPoint() && CurrentPreviewActor->GetClass()->IsChildOf<APointActor>())
		{
			UDLGameplayTargetSelectorCoordinateSimple* Object = Cast<UDLGameplayTargetSelectorCoordinateSimple>(
				TargetSelectorObject);

			FTransform ParentTransform;
			if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket)
			{
				const ASkeletalMeshActor* SMActor = Cast<ASkeletalMeshActor>(GetPreviewActor());
				const USkeletalMeshComponent* Component = SMActor->GetSkeletalMeshComponent();
				FTransform SocketTransform = Component->GetSocketTransform(
					TargetSelectorObject->AnchorSocketName);
				ParentTransform = SocketTransform;
			}
			else
			{
				ParentTransform =TargetSelectorObject->PreviewMeshTransform;
			}
			const auto PointRelativeTransform = CurrentPreviewActor->GetTransform().
				GetRelativeTransform(ParentTransform);
			Object->RelativeTransform = PointRelativeTransform;

		}
		else if (HasPoints() && CurrentPreviewActor->GetClass()->IsChildOf<APointActor>())
		{
			UDLGameplayTargetSelectorCoordinateCollection* Object = Cast<UDLGameplayTargetSelectorCoordinateCollection>(
			TargetSelectorObject);

			FTransform ParentTransform;

			if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket)
			{
				FTransform SocketTransform = GetSocketTransform(TargetSelectorObject->AnchorSocketName);
				ParentTransform = SocketTransform;
			}
			else
			{
				ParentTransform = TargetSelectorObject->PreviewMeshTransform;
			}

			for (int32 i = 0; i < PointArray.Num(); i++)
			{
				if (PointArray[i] == CurrentPreviewActor)
				{
					const auto PointRelativeTransform =  PointArray[i]->GetActorTransform().
						GetRelativeTransform(ParentTransform);
					Object->RelativeTransformArray[i].Coordinate = PointRelativeTransform;
				}
			}
		}
	}
}

AActor* FTargetSelectorEditor::GetCollisionActor() const
{
	UWorld* PreviewWorld = PreviewScene->GetWorld();
	AActor* Actor = nullptr;
	for (TActorIterator<ABoxActor> It(PreviewWorld); It; ++It)
	{
		Actor = *It;
		if (!Actor->IsPendingKillPending()
		)
		{
			break;
		}
	}
	for (TActorIterator<ASphereActor> It(PreviewWorld); It; ++It)
	{
		Actor = *It;
		if (!Actor->IsPendingKillPending()
		)
		{
			break;
		}
	}
	for (TActorIterator<ACapsuleActor> It(PreviewWorld); It; ++It)
	{
		Actor = *It;
		if (!Actor->IsPendingKillPending()
		)
		{
			break;
		}
	}
	return Actor;
}

AActor* FTargetSelectorEditor::GetAnchorActor() const
{
	UWorld* PreviewWorld = PreviewScene->GetWorld();
	AActor* Actor = nullptr;
	for (TActorIterator<AAnchorActor> It(PreviewWorld); It; ++It)
	{
		Actor = *It;
		if (!Actor->IsPendingKillPending()
		)
		{
			break;
		}
	}

	return Actor;
}

AActor* FTargetSelectorEditor::GetPreviewActor() const
{
	UWorld* PreviewWorld = PreviewScene->GetWorld();
	AActor* Actor = nullptr;
	for (TActorIterator<ASkeletalMeshActor> It(PreviewWorld); It; ++It)
	{
		Actor = *It;
		if (!Actor->IsPendingKillPending() &&
			(Actor->GetComponentByClass(ASkeletalMeshActor::StaticClass()))
		)
		{
			break;
		}
	}
	for (TActorIterator<AStaticMeshActor> It(PreviewWorld); It; ++It)
	{
		Actor = *It;
		if (!Actor->IsPendingKillPending() &&
			(Actor->GetComponentByClass(AStaticMeshActor::StaticClass()))
		)
		{
			break;
		}
	}

	for (TActorIterator<ACharacter> It(PreviewWorld); It; ++It)
	{
		Actor = *It;
		if (!Actor->IsPendingKillPending() &&
			(Actor->GetComponentByClass(ACharacter::StaticClass()))
		)
		{
			break;
		}
	}
	return Actor;
}


bool FTargetSelectorEditor::HasBox() const
{
	if (Cast<UDLGameplayTargetSelectorBoxTrace>(TargetSelectorObject))
	{
		return true;
	}
	return false;
}

bool FTargetSelectorEditor::HasSphere() const
{
	if (Cast<UDLGameplayTargetSelectorSphereTrace>(TargetSelectorObject))
	{
		return true;
	}
	return false;
}

bool FTargetSelectorEditor::HasCapsule() const
{
	if (Cast<UDLGameplayTargetSelectorCapsuleTrace>(TargetSelectorObject))
	{
		return true;
	}
	return false;
}

bool FTargetSelectorEditor::HasPoint() const
{
	if (Cast<UDLGameplayTargetSelectorCoordinateSimple>(TargetSelectorObject))
	{
		return true;
	}
	return false;
}

bool FTargetSelectorEditor::HasPoints() const
{
	if (Cast<UDLGameplayTargetSelectorCoordinateCollection>(TargetSelectorObject))
	{
		return true;
	}
	return false;
}

bool FTargetSelectorEditor::HasRandomPoints() const
{
	if (Cast<UDLGameplayTargetSelectorDynamicGeneration>(TargetSelectorObject))
	{
		return true;
	}
	return false;
}

FVector FTargetSelectorEditor::GetSocketLocation(FName SocketName) const
{
	if (GetPreviewActor())
	{
		UActorComponent* ActorComponent = GetPreviewActor()->GetComponentByClass(USkeletalMeshComponent::StaticClass());
		const USkeletalMeshComponent* SMComponent = Cast<USkeletalMeshComponent>(ActorComponent);
		if (SMComponent)
		{
			return SMComponent->GetSocketLocation(SocketName);
		}
	}
	return FVector(0, 0, 0);
}

FTransform FTargetSelectorEditor::GetSocketTransform(FName SocketName) const
{
	if (GetPreviewActor())
	{
		UActorComponent* ActorComponent = GetPreviewActor()->GetComponentByClass(USkeletalMeshComponent::StaticClass());
		const USkeletalMeshComponent* SMComponent = Cast<USkeletalMeshComponent>(ActorComponent);
		if (SMComponent)
		{
			return SMComponent->GetSocketTransform(SocketName);
		}
	}
	return FTransform::Identity;
}

bool FTargetSelectorEditor::IsComponentSelected(const UPrimitiveComponent* PrimComponent)
{
	return true;
}

void FTargetSelectorEditor::UpdateStartAndEndAnchor()
{
	if (HasCapsule())
	{
		//更新Start和End Vector （是相对于碰撞点的位置）
		UDLGameplayTargetSelectorCapsuleTrace* CapsuleObject = Cast<UDLGameplayTargetSelectorCapsuleTrace>(
			TargetSelectorObject);

		FVector UpVector = GetCollisionActor()->GetActorForwardVector() * CapsuleObject->TraceLength / 2;

		UpVector = UKismetMathLibrary::Quat_RotateVector(
			GetAnchorActor()->GetActorTransform().GetRotation().Inverse(), UpVector);
		const FVector DownVector = -1 * UpVector;
		const auto CenterPos = GetCollisionActor()->GetActorTransform().Inverse() * GetAnchorActor()->
			GetActorTransform();

		CapsuleObject->StartPosRelativeAnchor = CapsuleObject->CollisionTransform.GetLocation() + UpVector;

		CapsuleObject->EndPosRelativeAnchor = CapsuleObject->CollisionTransform.GetLocation() + DownVector;
	}
	else if (HasBox())
	{
		//更新Start和End Vector（是相对于碰撞点的位置）
		UDLGameplayTargetSelectorBoxTrace* BoxObject = Cast<UDLGameplayTargetSelectorBoxTrace>(TargetSelectorObject);

		FVector LeftVector = (GetCollisionActor()->GetActorForwardVector() * BoxObject->TraceLength / 2);

		LeftVector = UKismetMathLibrary::Quat_RotateVector(
			GetAnchorActor()->GetActorTransform().GetRotation().Inverse(), LeftVector);
		const FVector RightVector = -1 * LeftVector;

		BoxObject->StartPosRelativeAnchor = BoxObject->CollisionTransform.GetLocation() + LeftVector;
		BoxObject->EndPosRelativeAnchor = BoxObject->CollisionTransform.GetLocation() + RightVector;
	}
	else if (HasSphere())
	{
		//更新Start和End Vector （是相对于碰撞点的位置）
		UDLGameplayTargetSelectorSphereTrace* SphereObject = Cast<UDLGameplayTargetSelectorSphereTrace>(
			TargetSelectorObject);

		FVector LeftVector = (GetCollisionActor()->GetActorForwardVector() * SphereObject->TraceLength / 2);

		LeftVector = UKismetMathLibrary::Quat_RotateVector(
			GetAnchorActor()->GetActorTransform().GetRotation().Inverse(), LeftVector);

		const FVector RightVector = -1 * LeftVector;

		SphereObject->StartPosRelativeAnchor = SphereObject->CollisionTransform.GetLocation() + LeftVector;

		SphereObject->EndPosRelativeAnchor = SphereObject->CollisionTransform.GetLocation() + RightVector;
	}
	else if (HasPoint())
	{
		const UDLGameplayTargetSelectorCoordinateSimple* CoordinateObject = Cast<
			UDLGameplayTargetSelectorCoordinateSimple>(TargetSelectorObject);

		if (PointArray.Num() == 1)
		{
			const auto Position = CoordinateObject->RelativeTransform;
			FQuat Squat = TargetSelectorObject->PreviewMeshTransform.GetRotation();
			const FVector Offset = Position.GetLocation();
			FVector Location = TargetSelectorObject->PreviewMeshTransform.GetLocation();
			if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket)
			{
				FTransform Transform = GetSocketTransform(TargetSelectorObject->AnchorSocketName);
				Location = Transform.GetLocation();
				Squat = Transform.GetRotation();
			}
			const FVector RotateVector = UKismetMathLibrary::Quat_RotateVector(Squat, Offset);

			//UE_LOG(LogTemp, Warning, TEXT("Squat x=%d,y=%d,z=%d,w=%d"), Squat.X, Squat.Y, Squat.Z, Squat.W);
			PointArray[0]->SetActorLocation(Location + RotateVector);
			PointArray[0]->SetActorRotation(Squat * CoordinateObject->RelativeTransform.GetRotation());
		}
		else
		{
			FVector Location = TargetSelectorObject->PreviewMeshTransform.GetLocation();
			FQuat Rotator = TargetSelectorObject->PreviewMeshTransform.GetRotation();
			if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket)
			{
				FTransform Transform = GetSocketTransform(TargetSelectorObject->AnchorSocketName);
				Location = Transform.GetLocation();
				Rotator = Transform.GetRotation();
			}
			const FVector Offset = CoordinateObject->RelativeTransform.GetLocation();
			const FVector RotateVector = UKismetMathLibrary::Quat_RotateVector(Rotator, Offset);
			Location += RotateVector;

			UE_LOG(LogTemp, Warning, TEXT("Point Size Incorrect"));
			APointActor* PointActor = PreviewScene->GetWorld()->SpawnActor<APointActor>(Location, Rotator.Rotator());
			PointActor->ArrowComponent->ArrowColor = PreviewObject->ArrowColor;
			PointActor->ArrowComponent->SetEditorScale(PreviewObject->ArrowScale);

			this->PointArray.Push(PointActor);

			this->ButtonArray.Get()->AddSlot()
			    .Padding(FMargin(0, 20, 0, 0))
			[
				SNew(SButton)
					.Text(FText::FromString("Point" + FString::FromInt(0)))
				.OnClicked_Lambda([this]()
				             {
					             const auto Actor = this->PointArray[0];
					             if (Actor)
					             {
						             this->OnSelectionUpdated(Actor);
						             this->OnSelectActor.ExecuteIfBound(Actor);
					             }
					             UE_LOG(LogTemp, Warning, TEXT("PreviewMesh"));
					             return FReply::Handled();
				             })
			];
		}
	}
	else if (HasPoints())
	{
		const UDLGameplayTargetSelectorCoordinateCollection* CoordinatesObject = Cast<
			UDLGameplayTargetSelectorCoordinateCollection>(TargetSelectorObject);

		const int32 Num = CoordinatesObject->RelativeTransformArray.Num();
		if (PointArray.Num() <= Num)
		{
			UE_LOG(LogTemp, Warning, TEXT("Points Size Incorrect"));

			FVector Location = TargetSelectorObject->PreviewMeshTransform.GetLocation();
			FRotator Rotator = TargetSelectorObject->PreviewMeshTransform.Rotator();
			if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket)
			{
				FTransform Transform = GetSocketTransform(TargetSelectorObject->AnchorSocketName);
				Location = Transform.GetLocation();
				Rotator = Transform.Rotator();
			}
			for (int32 i = PointArray.Num(); i < Num; i++)
			{
				APointActor* PointActor = PreviewScene->GetWorld()->SpawnActor<APointActor>(Location, Rotator);
				PointActor->ArrowComponent->ArrowColor = PreviewObject->ArrowColor;
				PointActor->ArrowComponent->SetEditorScale(PreviewObject->ArrowScale);
				this->PointArray.Push(PointActor);

				this->ButtonArray.Get()->AddSlot()
				    .Padding(FMargin(0, 20, 0, 0))
				[
					SNew(SButton)
						.Text(FText::FromString("Point" + FString::FromInt(i)))
					.OnClicked_Lambda([this, i]()
					             {
						             const auto Actor = this->PointArray[i];
						             if (Actor)
						             {
							             this->OnSelectionUpdated(Actor);
							             this->OnSelectActor.ExecuteIfBound(Actor);
						             }
						             UE_LOG(LogTemp, Warning, TEXT("PreviewMesh"));
						             return FReply::Handled();
					             })
				];
			}

			DotArray.Empty();
			for (int32 i = 0; i < this->PointArray.Num(); i++)
			{
				DotArray.Add(i, {});
			}
		}
		else
		{
			//删除多余actor
			int32 DelNum = PointArray.Num() - Num;
			int32 LastNum = this->ButtonArray.Get()->GetChildren()->Num();
			for (int32 k = 0; k < DelNum; k++)
			{
				auto RemoveButton = this->ButtonArray.Get()->GetChildren()->GetChildAt(LastNum - (1 + k));
				this->ButtonArray.Get()->RemoveSlot(RemoveButton);

				PreviewScene->GetWorld()->DestroyActor(PointArray[PointArray.Num() - 1]);
				PointArray.RemoveAt(PointArray.Num() - 1);
			}

			this->OnSelectionUpdated(GetPreviewActor());
			this->OnSelectActor.ExecuteIfBound(GetPreviewActor());

			this->OnUpDateScene.ExecuteIfBound();
		}

		for (int32 i = 0; i < Num; i++)
		{
			const auto Position = CoordinatesObject->RelativeTransformArray[i];
			FQuat Squat = TargetSelectorObject->PreviewMeshTransform.GetRotation();
			FVector Location = TargetSelectorObject->PreviewMeshTransform.GetTranslation();
			if (TargetSelectorObject->AnchorSelectType == EAnchorSelectType::Socket)
			{
				FTransform Transform = GetSocketTransform(TargetSelectorObject->AnchorSocketName);
				Location = Transform.GetLocation();
				Squat = Transform.GetRotation();
			}
			const FVector Offset = Position.Coordinate.GetLocation();
			const FVector RotateVector = UKismetMathLibrary::Quat_RotateVector(Squat, Offset);

			PointArray[i]->SetActorLocation(Location + RotateVector);
			PointArray[i]->SetActorRotation(
				Squat * CoordinatesObject->RelativeTransformArray[i].Coordinate.GetRotation());
		}

		DotArray.Empty();
		for (int32 i = 0; i < this->PointArray.Num(); i++)
		{
			DotArray.Add(i, {});
		}
	}
}

FOnSelectActor& FTargetSelectorEditor::GetOnSelectActor()
{
	return OnSelectActor;
}

FOnUpDateScene& FTargetSelectorEditor::GetOnUpDateScene()
{
	return OnUpDateScene;
}
