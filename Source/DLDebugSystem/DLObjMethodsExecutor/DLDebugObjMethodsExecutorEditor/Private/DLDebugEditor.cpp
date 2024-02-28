#include "DLDebugEditor.h"
#include "DLCustomizationDetail.h"
#include "DLDebugSystem/DLObjMethodsExecutor/DLDebugObjMethodsExecutor/Public/DLDebugObjMethodsExecutor.h"
#include "DLDebugWidget.h"
#include "Blueprint/UserWidget.h"
#include "EdGraph/EdGraphNode_Documentation.h"

const FName DLDebugEditorViewPortTabId(TEXT("DLDebugEditorViewPortTabId"));
const FName DLDebugEditorDetailTabId(TEXT("DLDebugEditorDetailTabId"));
const FName DLDebugEditorTabId(TEXT("DLDebugEditorTabId"));

template <typename... TReturns, typename... TArgs>
void InvokeFunction(UClass* objClass, UObject* obj, UFunction* func, TTuple<TReturns...>& outParams, TArgs&&... args)
{
	objClass = obj != nullptr ? obj->GetClass() : objClass;
	UObject* context = obj != nullptr ? obj : objClass;
	uint8* outPramsBuffer = static_cast<uint8*>(&outParams);

	if (func->HasAnyFunctionFlags(FUNC_Native)) //quick path for c++ functions
	{
		TTuple<TArgs..., TReturns...> params(Forward<TArgs>(args)..., TReturns()...);
		context->ProcessEvent(func, &params);
		//copy back out params
		for (TFieldIterator<FProperty> i(func); i; ++i)
		{
			FProperty* prop = *i;
			if (prop->PropertyFlags & CPF_OutParm)
			{
				void* propBuffer = prop->ContainerPtrToValuePtr<void*>(&params);
				prop->CopyCompleteValue(outPramsBuffer, propBuffer);
				outPramsBuffer += prop->GetSize();
			}
		}
		return;
	}

	TTuple<TArgs...> inParams(Forward<TArgs>(args)...);
	void* funcPramsBuffer = static_cast<uint8*>(FMemory_Alloca(func->ParmsSize));
	uint8* inPramsBuffer = static_cast<uint8*>(&inParams);

	for (TFieldIterator<FProperty> i(func); i; ++i)
	{
		FProperty* prop = *i;
		if (prop->GetFName().ToString().StartsWith("__"))
		{
			//ignore private param like __WolrdContext of function in blueprint funcion library
			continue;
		}
		void* propBuffer = prop->ContainerPtrToValuePtr<void*>(funcPramsBuffer);
		if (prop->PropertyFlags & CPF_OutParm)
		{
			prop->CopyCompleteValue(propBuffer, outPramsBuffer);
			outPramsBuffer += prop->GetSize();
		}
		else if (prop->PropertyFlags & CPF_Parm)
		{
			prop->CopyCompleteValue(propBuffer, inPramsBuffer);
			inPramsBuffer += prop->GetSize();
		}
	}

	context->ProcessEvent(func, funcPramsBuffer); //call function
	outPramsBuffer = static_cast<uint8*>(&outParams); //reset to begin

	//copy back out params
	for (TFieldIterator<FProperty> i(func); i; ++i)
	{
		FProperty* prop = *i;
		if (prop->PropertyFlags & CPF_OutParm)
		{
			void* propBuffer = prop->ContainerPtrToValuePtr<void*>(funcPramsBuffer);
			prop->CopyCompleteValue(outPramsBuffer, propBuffer);
			outPramsBuffer += prop->GetSize();
		}
	}
}

void UDetailInfo::TestFun(int32 Number, FString Name, FVector Pos)
{
	UE_LOG(LogTemp, Warning, TEXT("TestFun name =  %s"), *Name);
}

void UDetailInfo::TestFun2(UFunStruct* Param)
{
	UE_LOG(LogTemp, Warning, TEXT("TestFun2 number = %d name =  %s"), Param->Number2, *Param->Name2);
}

void UDetailInfo::TestFun3(int32 Number, FString Name, FVector Pos)
{
	UE_LOG(LogTemp, Warning, TEXT("TestFun3 name =  %s"), *Name);
}

FDLDebugEditor::FDLDebugEditor()
{
	//DetailInfo = NewObject<UDetailInfo>();

	auto NomadTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(NSLOCTEXT("NiagaraDebugger", "NiagaraDebuggerTabTitle", "Niagara Debugger"));

	EditorTabManager = FGlobalTabmanager::Get()->NewTabManager(NomadTab);
}

void FDLDebugEditor::Init(int32 Num)
{
	auto CreatLeftTab = [&](const FSpawnTabArgs&)-> TSharedRef<SDockTab>
	{
		auto Detail = this->CreatDetailTab();
		TSharedRef<SDockTab> ExampleTab = SNew(SDockTab)
			.TabRole(PanelTab)
			[
				Detail.ToSharedRef()
			];
		return ExampleTab;
	};

	auto CreatRightTab = [&](const FSpawnTabArgs&)-> TSharedRef<SDockTab>
	{
		TSharedRef<SDockTab> ExampleTab = SNew(SDockTab)
			.TabRole(PanelTab)
			[
				SNew(SDLDebugWidget)
				.OnClassClicked_Lambda([this](const UClass* Select)
				                    {
					                    this->OnSelectClass(Select);
				                    })
				.OnSClicked_Lambda([this]()
				                    {
					                    this->OnButtonClick();
					                    return FReply::Handled();
				                    })
			];
		return ExampleTab;
	};

	const FName NewDetailID = FName(DLDebugEditorDetailTabId.ToString() + FString("_") + FString::FromInt(Num));
	EditorTabManager->RegisterTabSpawner(NewDetailID, FOnSpawnTab::CreateLambda(CreatLeftTab))
	                .SetDisplayName(FText::FromString("DataPanel"));

	const FName NewViewPortID = FName(DLDebugEditorViewPortTabId.ToString() + FString("_") + FString::FromInt(Num));
	EditorTabManager->RegisterTabSpawner(NewViewPortID, FOnSpawnTab::CreateLambda(CreatRightTab))
	                .SetDisplayName(FText::FromString("ViewPort"));


	//创建新的布局
	TSharedRef<FTabManager::FLayout> DLDebugEditorDefaultLayout
		= FTabManager::NewLayout("DLDebugEditorLayout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(1.0f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.4f)
					->AddTab(NewDetailID, ETabState::OpenedTab)
					->SetHideTabWell(true)
					->SetForegroundTab(NewDetailID)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.6f)
					->AddTab(NewViewPortID, ETabState::OpenedTab)
					->SetHideTabWell(true)
				)
			)
		);

	auto CreatTab = [DLDebugEditorDefaultLayout,this](const FSpawnTabArgs&)-> TSharedRef<SDockTab>
	{
		TSharedRef<SWidget> TabContents = EditorTabManager->RestoreFrom(
			DLDebugEditorDefaultLayout, TSharedPtr<SWindow>()).ToSharedRef();
		TSharedRef<SDockTab> ExampleTab = SNew(SDockTab)
			.Label(FText::FromString(TEXT("测试窗口")))
			.TabRole(NomadTab)
			.Icon(FAppStyle::GetBrush("LevelEditor.Tabs.Toolbar"))
			.TabColorScale(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f))
		[
			TabContents
		];

		return ExampleTab;
	};

	const FName NewID = FName(DLDebugEditorTabId.ToString() + FString("_") + FString::FromInt(Num));

	EditorTabManager->RegisterTabSpawner(NewID, FOnSpawnTab::CreateLambda(CreatTab))
	                .SetDisplayName(FText::FromString("DLDebugEditorTabId"));
}

void FDLDebugEditor::CreatNewTab()
{
	FDLDebugObjExecutorDelegates::OnDebugNewInstanceWorld.BindLambda([this](int32 ID)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CreatNewTab  %d"),ID);

		//auto ss = GEngine->GetWorldContextFromPIEInstance(ID);
		//;


		//UWorld* TsWorld = ss->World();

		//if(TsWorld)
		//{
		//	auto Actors= TsWorld->GetLevels()[0]->Actors;

		//	UE_LOG(LogTemp, Warning, TEXT("CreatNewTab  %d"), Actors.Num());
		//}

		this->PIEID.Add(ID);
	});
	//const auto Tab = SNew(SDockTab)
	//	.Label(FText::FromString(TEXT("测试窗口")))
	//	.TabRole(ETabRole::NomadTab)
	//	.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Toolbar"))
	//	.TabColorScale(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f))
	//	[
	//		SNew(SDLDebugWidget)
	//	];

	//TSharedPtr<FTabManager::FSearchPreference> SearchPreference = nullptr;

	//FGlobalTabmanager::Get()->InsertNewDocumentTab(TEXT("StandaloneToolkit"), FTabManager::ESearchPreference::PreferLiveTab, Tab);


	FString WidgetClassLoadPath = FString(TEXT("WidgetBlueprint'/Game/ThirdPersonCPP/DebugUMG.DebugUMG_C'"));
	if (!UserWidget.IsValid())
	{
		//TSharedPtr<FTabManager::FSearchPreference> SearchPreference = nullptr;

		//FGlobalTabmanager::Get()->GetActiveTab()
		//FGlobalTabmanager::Get()->InsertNewDocumentTab(DLDebugEditorTabId,
		//                                               FTabManager::ESearchPreference::PreferLiveTab, Tab);

		//根据布局Layout，创建生成布局，会直接创建新窗口
		//auto te= FGlobalTabmanager::Get()->TryInvokeTab(DLDebugEditorDetailTabId);

		//TSharedRef<SWidget> TabContents = FGlobalTabmanager::Get()->RestoreFrom(EditorLayout.ToSharedRef(), TSharedPtr<SWindow>()).ToSharedRef();

		if (SDockTabArray.Num() > 0)
		{
			auto CreatLeftTab = [&](const FSpawnTabArgs&)-> TSharedRef<SDockTab>
			{
				auto Detail = this->CreatDetailTab();
				TSharedRef<SDockTab> ExampleTab = SNew(SDockTab)
					.TabRole(PanelTab)
					[
						Detail.ToSharedRef()
					];
				return ExampleTab;
			};

			auto CreatRightTab = [&](const FSpawnTabArgs&)-> TSharedRef<SDockTab>
			{
				TSharedRef<SDockTab> ExampleTab = SNew(SDockTab)
					.TabRole(PanelTab)
					[
						SNew(SDLDebugWidget)
						.OnClassClicked_Lambda([this](const UClass* Select)
						                    {
							                    this->OnSelectClass(Select);
						                    })
					.OnSClicked_Lambda([this]()
						                    {
							                    this->OnButtonClick();
							                    return FReply::Handled();
						                    })
					];
				return ExampleTab;
			};

			const FName NewDetailID = FName(
				DLDebugEditorDetailTabId.ToString() + FString("_") + FString::FromInt(SDockTabArray.Num()));
			EditorTabManager->RegisterTabSpawner(NewDetailID, FOnSpawnTab::CreateLambda(CreatLeftTab))
			                .SetDisplayName(FText::FromString("DataPanel"));

			const FName NewViewPortID = FName(
				DLDebugEditorViewPortTabId.ToString() + FString("_") + FString::FromInt(SDockTabArray.Num()));
			EditorTabManager->RegisterTabSpawner(NewViewPortID, FOnSpawnTab::CreateLambda(CreatRightTab))
			                .SetDisplayName(FText::FromString("ViewPort"));


			//创建新的布局
			TSharedRef<FTabManager::FLayout> NewLayout
				= FTabManager::NewLayout("DLDebugEditorLayout")
				->AddArea
				(
					FTabManager::NewPrimaryArea()
					->SetOrientation(Orient_Vertical)
					->Split
					(
						FTabManager::NewSplitter()
						->SetOrientation(Orient_Horizontal)
						->SetSizeCoefficient(1.0f)
						->Split
						(
							FTabManager::NewStack()
							->SetSizeCoefficient(0.4f)
							->AddTab(NewDetailID, ETabState::OpenedTab)
							->SetHideTabWell(true)
							->SetForegroundTab(NewDetailID)
						)
						->Split
						(
							FTabManager::NewStack()
							->SetSizeCoefficient(0.6f)
							->AddTab(NewViewPortID, ETabState::OpenedTab)
							->SetHideTabWell(true)
						)
					)
				);

			TSharedRef<SWidget> TabContents = EditorTabManager->RestoreFrom(
				NewLayout, TSharedPtr<SWindow>()).ToSharedRef();
			const TSharedRef<SDockTab> Tab = SNew(SDockTab)
				.Label(FText::FromString(TEXT("测试窗口"+FString::FromInt(SDockTabArray.Num()+1))))
				.TabRole(NomadTab)
				.Icon(FAppStyle::GetBrush("LevelEditor.Tabs.Toolbar"))
				.TabColorScale(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f))
			[
				TabContents
			];

			//Init(SDockTabArray.Num());
			const FName NewID = FName(
				DLDebugEditorTabId.ToString() + FString("_") + FString::FromInt(SDockTabArray.Num()));
			const FName LastID = FName(DLDebugEditorTabId.ToString() + FString("_") + FString::FromInt(0));


			//auto InvokeTab = FGlobalTabmanager::Get()->TryInvokeTab(FName(NewID));
			//SDockTabArray.Add(InvokeTab);
			EditorTabManager->InsertNewDocumentTab(LastID,
			                                       FTabManager::ESearchPreference::PreferLiveTab, Tab);

			//auto tstb = FGlobalTabmanager::Get()->FindExistingLiveTab(LastID);
			//tstb.Get()->GetParent()->AddTab(Tab);


			/*Tab->SetLayoutIdentifier(NewID);*/
			SDockTabArray.Add(Tab);
		}
		else
		{
			Init(SDockTabArray.Num());
			const FName NewID = FName(
				DLDebugEditorTabId.ToString() + FString("_") + FString::FromInt(SDockTabArray.Num()));
			auto InvokeTab = EditorTabManager->TryInvokeTab(FName(NewID));
			SDockTabArray.Add(InvokeTab);
		}
	}
	else
	{
		UClass* Widget = LoadClass<UDLDebugUMG>(nullptr, *WidgetClassLoadPath);
		//UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
		UWorld* World = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->GetWorld();
		UDLDebugUMG* VendingMachineUI = CreateWidget<UDLDebugUMG>(World, Widget);

		UserWidget = MakeWeakObjectPtr(VendingMachineUI);

		if (UserWidget.IsValid())
		{
			CreatNewTab();
		}
	}
}

void FDLDebugEditor::GetWorldInfo()
{
	for (TObjectIterator<UWorld> It; It; ++It)
	{
		UWorld* TestWorld = *It;


		TestWorld->GetSubsystem<UDLDebugObjExecutorSubsystem>();

		// Include only PIE and worlds that own the persistent level (i.e. non-streaming levels).
		const bool bIsValidDebugWorld = (TestWorld != nullptr)
			&& TestWorld->WorldType == EWorldType::PIE
			&& TestWorld->PersistentLevel != nullptr
			&& TestWorld->PersistentLevel->OwningWorld == TestWorld;

		if (!bIsValidDebugWorld)
		{
			continue;
		}

		ENetMode NetMode = TestWorld->GetNetMode();

		FString WorldName;

		switch (NetMode)
		{
		case NM_Standalone:
			WorldName = NSLOCTEXT("BlueprintEditor", "DebugWorldStandalone", "Standalone").ToString();
			break;

		case NM_ListenServer:
			WorldName = NSLOCTEXT("BlueprintEditor", "DebugWorldListenServer", "Listen Server").ToString();
			break;

		case NM_DedicatedServer:
			WorldName = NSLOCTEXT("BlueprintEditor", "DebugWorldDedicatedServer", "Dedicated Server").ToString();
			break;

		case NM_Client:
			if (FWorldContext* PieContext = GEngine->GetWorldContextFromWorld(TestWorld))
			{
				WorldName = FString::Printf(
					TEXT("%s %d"), *NSLOCTEXT("BlueprintEditor", "DebugWorldClient", "Client").ToString(),
					PieContext->PIEInstance - 1);
			}
			break;
		}


		UE_LOG(LogTemp, Warning, TEXT("GetWorldInfo name =  %s"), *WorldName);
	}

	if (PIEID.Num() > 0)
	{
		auto ss = GEngine->GetWorldContextFromPIEInstance(PIEID[0]);

		UWorld* TsWorld = ss->World();

		if (TsWorld)
		{
			UE_LOG(LogTemp, Warning, TEXT("GetWorldInfo name =  %s"), *TsWorld->GetName());

			auto Actors = TsWorld->GetLevels()[0]->Actors;

			UE_LOG(LogTemp, Warning, TEXT("CreatNewTab  %d"), Actors.Num());

			for (auto& ac : Actors)
			{
			}
		}
	}
}

void FDLDebugEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
	FEditPropertyChain* PropertyThatChanged)
{
	UE_LOG(LogTemp, Log, TEXT("NotifyPostChange %s"), *PropertyChangedEvent.GetPropertyName().ToString());
	;
	UE_LOG(LogTemp, Log, TEXT("TestStruct->Name2 %s"), *TestStruct->Name2);
	auto CurrentNode = PropertyThatChanged->GetActiveNode();
	while (CurrentNode)
	{
		UE_LOG(LogTemp, Log, TEXT("GetActiveNode %s"), *CurrentNode->GetValue()->GetFName().ToString());

		CurrentNode = CurrentNode->GetNextNode();
	}
}

TSharedPtr<IDetailsView> FDLDebugEditor::CreatDetailTab()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
		"PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, false);
	DetailsViewArgs.NotifyHook = this;
	DetailsViewArgs.HostTabManager = EditorTabManager;

	auto DetailContent = PropertyEditorModule.CreateDetailView(DetailsViewArgs);


	//DetailContent->RegisterInstancedCustomPropertyTypeLayout(FName("DLDebugObjectFilter"),
	//                                                         FOnGetPropertyTypeCustomizationInstance::CreateStatic(
	//	                                                         &FDLCustomizationProperty::MakeInstance));

	//DetailContent->RegisterInstancedCustomPropertyLayout(UDetailInfo::StaticClass(),
	//                                                     FOnGetDetailCustomizationInstance::CreateStatic(
	//	                                                     &FDLCustomizationDetail::MakeInstance));


	DetailContent->ClearSearch();

	DetailInfo = NewObject<UDetailInfo>();
	TestStruct = NewObject<UFunStruct>();

	FProperty* mm = nullptr;
	FProperty* nn = nullptr;
	int i = 0;
	for (TFieldIterator<FProperty> It(DetailInfo->StaticClass()); It; ++It)
	{
		FProperty* param = *It;
		mm = param;
		i++;
		if (i == 2)
		{
			break;
		}
		//UE_LOG(LogTemp, Warning, TEXT("OnSelectClass DetailInfo =  %s"), *param->GetNameCPP());
	}
	i = 0;
	for (TFieldIterator<FProperty> It(TestStruct->StaticClass()); It; ++It)
	{
		FProperty* param = *It;
		nn = param;
		if (i == 1)
		{
			break;
		}
	}

	//mm->PropertyLinkNext = nn;


	//UField* kk = cc->Next;

	//while (kk)
	//{
	//	kk = kk->Next;
	//}

	//TestStruct->AddCppProperty(nn);


	//for (TFieldIterator<FProperty> It(TestStruct.Get()); It; ++It)
	//{
	//	FProperty* param = *It;
	//	UE_LOG(LogTemp, Warning, TEXT(" FProperty =  %s"), *param->GetNameCPP());
	//}

	for (TFieldIterator<FProperty> It(TestStruct->GetClass()); It; ++It)
	{
		FProperty* param = *It;
		UE_LOG(LogTemp, Warning, TEXT("static FProperty =  %s"), *param->GetNameCPP());
	}

	DetailContent->SetObject(TestStruct);


	//DetailContent->SetObject(DetailInfo.Get());


	DetailsView = DetailContent;
	return DetailContent;
}

void FDLDebugEditor::OnSelectClass(const UClass* SelectInfo)
{
	UDLDebugObjExecutorSubsystem* ts = NewObject<UDLDebugObjExecutorSubsystem>();

	FDLDebugObjectFilter FDLDebugObjectFilter;

	FDLDebugObjectFilter.ClassFilter = SelectInfo->GetClass();

	TArray<TWeakObjectPtr<UObject>> OutObjectList;

	ts->GetObjectListByFilter(FDLDebugObjectFilter, OutObjectList);

	UE_LOG(LogTemp, Warning, TEXT("OnSelectClass Num =  %d"), OutObjectList.Num());

	for (auto ObjectList : OutObjectList)
	{
		//UE_LOG(LogTemp, Warning, TEXT("GetWorldInfo name =  %s"), *TsWorld->GetName());
	}

	const auto Flags = SelectInfo->GetFlags();
	for (TFieldIterator<UFunction> It(SelectInfo); It; ++It)
	{
		UFunction* Func = *It;
		if (Func && Func->FunctionFlags & (Flags))
		{
			UE_LOG(LogTemp, Warning, TEXT("FunctionFlags Num =  %s"), *Func->GetName());
		}
	}

	TArray<FName> OutArray;
	SelectInfo->GenerateFunctionList(OutArray);

	auto Fun = SelectInfo->FindFunctionByName(OutArray[2]);
	auto ParamName = Fun->GetMetaData("MyTest");



	//FField* ChildProperty = Fun->ChildProperties;
	DetailsView->SetObject(nullptr, true);

	DetailInfo.Get()->FunTest = Fun;
	DetailInfo.Get()->Num = 20;

	//FReferenceCollector* ss=new FReferenceCollector();

	TArray<UObject*> Array;
	auto obj = NewObject<UFunStruct>();
	// 遍历函数的参数
	for (TFieldIterator<FProperty> j(Fun); j; ++j)
	{
		FProperty* param = *j;
		auto a = param->GetNameCPP();
		auto b = param->GetCPPType();

		//UObject::execDynArrayFindStruct()
		//auto c = param->GetNameCPP();

		Array.Add(param->GetUPropertyWrapper());
		param->GetClass()->GetFName();

		//obj->AddCppProperty(param);
		if (param->IsA(FStructProperty::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Function name =  %s, Param  : %s Type: %s"), *Fun->GetName(), *a, *b);

			Cast<FStructProperty>(param)->GetClass();


			//for (TFieldIterator<FProperty> i(param->StaticClass()); i; ++i)
			//{
			//	FProperty* prop = *i;
			//}
		}
		if (param->IsA(FObjectProperty::StaticClass()))
		{
			auto ww = Cast<FObjectProperty>(param)->PropertyClass->ClassDefaultObject;
			UE_LOG(LogTemp, Warning, TEXT("Function name =  %s"),
			       *Cast<FObjectProperty>(param)->PropertyClass->GetName());

			if (ww)
			{
				//DetailsView->SetObject(ww);
			}
		}

		//DetailInfo.Get()->AddReferencedObjects(DetailInfo.Get());

		//DetailsView->SetObject();
		UE_LOG(LogTemp, Warning, TEXT("Function name =  %s, Param  : %s Type: %s"), *Fun->GetName(), *a, *b);

		// 判断是否是返回值
		if (param->PropertyFlags & CPF_ReturnParm)
		{
		}
	}

	UObject* ClassPackage = ANY_PACKAGE;
	UClass* Result = FindObject<UClass>(ClassPackage, *ParamName);
	UE_LOG(LogTemp, Warning, TEXT("FindObject =  %s"), *Result->GetName());

	for (TFieldIterator<FProperty> It(Result); It; ++It)
	{
		FProperty* Func = *It;
		if (Func)
		{
			UE_LOG(LogTemp, Warning, TEXT("FunctionFlags Num =  %s"), *Func->GetName());
		}
	}

	DetailsView->SetObject(Result);
	//obj->GetPreloadDependencies(Array);

	//DetailsView->SetObject(obj);

	//Fun->NumParms

	//SelectInfo->GenerateFunctionList()
}

void FDLDebugEditor::OnButtonClick()
{
	auto cc = DetailsView.Get()->GetSelectedObjects();
	auto obj = cc[0].Get();

	void* funcPramsBuffer = static_cast<uint8*>(FMemory_Alloca(DetailInfo->FunTest->ParmsSize));
	FMemory::Memzero(funcPramsBuffer, DetailInfo->FunTest->ParmsSize);

	uint8* inPramsBuffer = 0;
	uint8* outPramsBuffer = nullptr;
	int32 Offset = 0;

	uint8* Parms = (uint8*)FMemory_Alloca(DetailInfo->FunTest->ParmsSize);
	//FMemory::Memzero(Parms, DetailInfo->FunTest->ParmsSize);

	UClass* TestClass = obj->GetClass();
	for (TFieldIterator<FProperty> It(Cast<UStruct>(obj)); It; ++It)
	{
		FProperty* LocalProp = *It;
		//LocalProp->CopyCompleteValue()
		checkSlow(LocalProp);
		LocalProp->InitializeValue_InContainer(Parms);
	}

	int hh = 0;
	for (TFieldIterator<FProperty> i(Cast<UStruct>(DetailInfo->FunTest)); i; ++i)
	{
		FProperty* prop = *i;
		//if (prop->GetFName().ToString().StartsWith("__"))
		//{
		//	//ignore private param like __WolrdContext of function in blueprint funcion library
		//	continue;
		//}
		//void* propBuffer = prop->ContainerPtrToValuePtr<void*>(funcPramsBuffer);
		//if (prop->PropertyFlags & CPF_OutParm)
		//{
		//	prop->CopyCompleteValue(propBuffer, outPramsBuffer);
		//	outPramsBuffer += prop->GetSize();
		//}
		//else if (prop->PropertyFlags & CPF_Parm)
		//{
		//	prop->CopyCompleteValue(propBuffer, inPramsBuffer);
		//	inPramsBuffer += prop->GetSize();
		//}
		//InstanceValuePtr = PropertyToUpdate.Property->ContainerPtrToValuePtr<uint8>(ObjectPtr);


		//if(!inPramsBuffer)
		//{
		//	//inPramsBuffer =;
		//}
		//uint8* SourcePtr = prop->ContainerPtrToValuePtr<uint8>(prop);
		//prop->CopyCompleteValue(propBuffer, SourcePtr);
		//inPramsBuffer += prop->GetSize();


		//uint8* StartAddr = prop->ContainerPtrToValuePtr<uint8>(Parms);
		//FMemory::Memcpy(StartAddr, prop, Offset);
		//Offset += prop->GetSize();

		//void* propBuffer = prop->ContainerPtrToValuePtr<void*>(funcPramsBuffer);
		if(hh==0)
		{
			//propBuffer = &TestStruct.Get()->Number2;
			//prop->CopyCompleteValue(propBuffer, &TestStruct.Get()->Number2);
		}
		else if(hh==1)
		{
			//propBuffer = &TestStruct.Get()->Name2;
			//prop->CopyCompleteValue(propBuffer, &TestStruct.Get()->Name2);
		}
		else if (hh == 2)
		{
			//propBuffer = &TestStruct.Get()->Pos;
			//prop->CopyCompleteValue(propBuffer, &TestStruct.Get()->Pos);
		}
		
		//auto PramsBuffer = prop->ContainerPtrToValuePtr<void*>(prop);
		

		hh++;
	}
	struct TestS
	{
		int32 num = 10;
		FString str = "00";
		FVector vec;
	};
	TestS ts;

	if(TestStruct)
	{
		auto ptr = TestStruct->GetStruct();

		DetailInfo->ProcessEvent(DetailInfo->FunTest, ptr);
	}
}
