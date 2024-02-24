#include "AbilityChainNodeAssetFactory.h"
#include "AbilityChainAsset.h"
#include "ClassViewerModule.h"
#include "AudioEditor/Private/Factories/SoundFactoryUtility.h"
#include "Kismet2/SClassPickerDialog.h"


UAbilityChainNodeAssetFactory::UAbilityChainNodeAssetFactory(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UAbilityChainAsset::StaticClass();
}

UObject* UAbilityChainNodeAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName,
                                                       EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UAbilityChainAsset* AssetObj;


	if (AbilityChainNodeAssetClass !=nullptr)
	{
		AssetObj = NewObject<UAbilityChainAsset>(InParent, AbilityChainNodeAssetClass, InName, Flags);
	}
	else
	{
		AssetObj = NewObject<UAbilityChainAsset>(InParent, InClass, InName, Flags);
	}

	return AssetObj;
}

bool UAbilityChainNodeAssetFactory::ConfigureProperties()
{
	AbilityChainNodeAssetClass = nullptr;

	//FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::DisplayName;
	Options.bShowNoneOption = false;

	const TSharedPtr<FAssetClassParentFilter> Filter = MakeShareable(new FAssetClassParentFilter);
	Options.ClassFilter = Filter;

	Filter->DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_HideDropDown;
	Filter->AllowedChildrenOfClasses.Add(UAbilityChainAsset::StaticClass());

	const FText TitleText = FText::FromString(TEXT("Pick UTreeNodeAsset Class"));
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass,
														  UAbilityChainAsset::StaticClass());

	if (bPressedOk)
	{
		AbilityChainNodeAssetClass = ChosenClass;
	}

	return bPressedOk;
}
