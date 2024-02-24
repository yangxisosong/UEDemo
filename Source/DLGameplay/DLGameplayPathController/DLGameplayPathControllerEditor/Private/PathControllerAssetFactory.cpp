#include "PathControllerAssetFactory.h"

#include "ClassViewerModule.h"
#include "PathControllerTraceAsset.h"
#include "AudioEditor/Private/Factories/SoundFactoryUtility.h"
#include "Kismet2/SClassPickerDialog.h"


UPathControllerAssetFactory::UPathControllerAssetFactory(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UPathControllerTraceAsset::StaticClass();
}

UObject* UPathControllerAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName,
                                                       EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UPathControllerTraceAsset* AssetObj;


	if (PCAssetClass!=nullptr)
	{
		AssetObj = NewObject<UPathControllerTraceAsset>(InParent, PCAssetClass, InName, Flags);
	}
	else
	{
		AssetObj = NewObject<UPathControllerTraceAsset>(InParent, InClass, InName, Flags);
	}

	return AssetObj;
}

bool UPathControllerAssetFactory::ConfigureProperties()
{
	PCAssetClass = nullptr;

	//FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;

	const TSharedPtr<FAssetClassParentFilter> Filter = MakeShareable(new FAssetClassParentFilter);
	Options.ClassFilter = Filter;

	Filter->DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_HideDropDown;
	Filter->AllowedChildrenOfClasses.Add(UPathControllerTraceAsset::StaticClass());

	const FText TitleText = FText::FromString(TEXT("Pick UPathControllerTraceAsset Class"));
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass,
														  UPathControllerTraceAsset::StaticClass());

	if (bPressedOk)
	{
		PCAssetClass = ChosenClass;
	}

	return bPressedOk;
}
