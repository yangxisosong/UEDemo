#include "DLCustomizationDetail.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DLDebugEditor.h"
#include "IDetailChildrenBuilder.h"
#include "SKismetInspector.h"

#define LOCTEXT_NAMESPACE "CustCategory"

TSharedRef<IDetailCustomization> FDLCustomizationDetail::MakeInstance()
{
	return MakeShareable(new FDLCustomizationDetail);
}

void FDLCustomizationDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	UE_LOG(LogTemp, Warning, TEXT("CustomizeDetails "));

	IDetailCategoryBuilder& CustCategory = DetailBuilder.EditCategory(FName("CustCategory"),
	                                                                  LOCTEXT("CustCategory", "Custom Category"),
	                                                                  ECategoryPriority::Important);
	UDetailInfo* DetailsObject = nullptr;
	TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);
	for (TWeakObjectPtr<UObject> Object : CustomizedObjects)
	{
		if (Object.IsValid())
		{
			DetailsObject = Cast<UDetailInfo>(Object);
			if (DetailsObject)
			{
				break;
			}
		}
	}

	TSharedPtr<SKismetInspector> Inspector = nullptr;

	check(DetailsObject);
	CustCategory.AddCustomRow(LOCTEXT("RowSearchName", "Button")) //搜索条件
	            .NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("DetailName", "New Button"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		[
			/*SNew(SButton)
			.Text(LOCTEXT("ButtonText", "Button"))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("ButtonToolTip", "this is a button"))*/

			SAssignNew(Inspector, SKismetInspector)
			.ShowTitleArea(false)
			.ShowPublicViewControl(false)
			.HideNameArea(true)
		];

	SKismetInspector::FShowDetailsOptions Options;
	Options.bHideFilterArea = false;
	Options.bShowComponents = true;

	if (Cast<UDetailInfo>(CustomizedObjects[0]))
	{
		Function = Cast<UDetailInfo>(CustomizedObjects[0])->FunTest;
	}

	if (Function.Get())
	{
		// 遍历函数的参数
		TArray<UObject*> Array;
		for (TFieldIterator<FProperty> j(Function.Get()); j; ++j)
		{
			FProperty* param = *j;
			auto a = param->GetNameCPP();
			auto b = param->GetCPPType();
			//auto c = param->GetNameCPP();
			Array.Add(param->GetUPropertyWrapper());
			//Inspector->ShowDetailsForSingleObject(param->GetUPropertyWrapper(), Options);
		}
		//Inspector->ShowDetailsForObjects(Array, Options);

		//Inspector->ShowDetailsForSingleObject(UFunStruct::StaticClass()->GetDefaultObject(), Options);
	}

	IDetailCategoryBuilder& DefaultValueCategory = DetailBuilder.EditCategory(
		TEXT("DefaultValueCategory"), LOCTEXT("DefaultValueCategoryHeading", "Default Value"));
	TArray<UObject*> ObjectList;
	IDetailPropertyRow* Row = DefaultValueCategory.AddExternalObjectProperty(ObjectList, TEXT("666"));
	if (Row != nullptr)
	{
		Row->IsEnabled(true);
	}

	DetailBuilder.RegisterInstancedCustomPropertyTypeLayout(FName("NetRole"),
	                                                        FOnGetPropertyTypeCustomizationInstance::CreateStatic(
		                                                        &FDLCustomizationProperty::MakeInstance));
}

TSharedRef<IPropertyTypeCustomization> FDLCustomizationProperty::MakeInstance()
{
	return MakeShareable(new FDLCustomizationProperty);
}

void FDLCustomizationProperty::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
                                               IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	UE_LOG(LogTemp, Warning, TEXT("CustomizeHeader "));
	TArray<UObject*> CustomizedObjects;
	PropertyHandle->GetOuterObjects(CustomizedObjects);
}

void FDLCustomizationProperty::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
                                                 IDetailChildrenBuilder& ChildBuilder,
                                                 IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	UE_LOG(LogTemp, Warning, TEXT("CustomizeChildren "));

	TArray<UObject*> CustomizedObjects;
	PropertyHandle->GetOuterObjects(CustomizedObjects);

	ChildBuilder.GetParentGroup();

	if (Cast<UDetailInfo>(CustomizedObjects[0]))
	{
		Function = Cast<UDetailInfo>(CustomizedObjects[0])->FunTest;
	}

	if (Function.Get())
	{
		// 遍历函数的参数
		for (TFieldIterator<FProperty> j(Function.Get()); j; ++j)
		{
			FProperty* param = *j;
			auto a = param->GetNameCPP();
			auto b = param->GetCPPType();
			//auto c = param->GetNameCPP();


			UE_LOG(LogTemp, Warning, TEXT("Function name =  %s, Param  : %s Type: %s"), *Function.Get()->GetName(), *a,
			       *b);

			ChildBuilder.AddCustomRow(FText::FromString(a))
			            .NameContent()
			[
				PropertyHandle->CreatePropertyNameWidget(FText::FromString(a),
				                                         FText::FromString(a))
			];

			// 判断是否是返回值
			if (param->PropertyFlags & CPF_ReturnParm)
			{
			}
		}
	}
	else
	{
		ChildBuilder.AddCustomRow(FText::FromString(FString(TEXT("111"))))
		            .NameContent()
			[
				PropertyHandle->CreatePropertyNameWidget(FText::FromString(FString(TEXT("222"))),
				                                         FText::FromString(FString(TEXT("333"))))
			]
			.ValueContent()
			[
				SNew(SButton)
				.Text(LOCTEXT("ButtonText", "Button"))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("ButtonToolTip", "this is a button"))
			];
	}
}

#undef LOCTEXT_NAMESPACE
