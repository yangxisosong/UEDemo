#include "TargetSelectorAssetFactory.h"

#include "ClassViewerModule.h"
#include "DLAbilityTargetSelector.h"
#include "AudioEditor/Private/Factories/SoundFactoryUtility.h"
#include "Kismet2/SClassPickerDialog.h"


UTargetSelectorAssetFactory::UTargetSelectorAssetFactory(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UDLGameplayTargetSelectorBase::StaticClass();
}

UObject* UTargetSelectorAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName,
                                                       EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UDLGameplayTargetSelectorBase* AssetObj;


	if (TargetSelectorAssetClass != nullptr)
	{
		AssetObj = NewObject<UDLGameplayTargetSelectorBase>(InParent, TargetSelectorAssetClass, InName, Flags);
	}
	else
	{
		AssetObj = NewObject<UDLGameplayTargetSelectorBase>(InParent, InClass, InName, Flags);
	}

	return AssetObj;
}

bool UTargetSelectorAssetFactory::ConfigureProperties()
{
	TargetSelectorAssetClass = nullptr;

	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::DisplayName;
	Options.bShowNoneOption = false;

	const TSharedPtr<FAssetClassParentFilter> Filter = MakeShareable(new FAssetClassParentFilter);
	Options.ClassFilter = Filter;

	Filter->DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_HideDropDown;
	Filter->AllowedChildrenOfClasses.Add(UDLGameplayTargetSelectorShapeTraceBase::StaticClass());
	Filter->AllowedChildrenOfClasses.Add(UDLGameplayTargetSelectorCoordinate::StaticClass());

	const FText TitleText = FText::FromString(TEXT("Pick Target Selector Asset Class"));
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass,
	                                                      UDLGameplayTargetSelectorBase::StaticClass());

	if (bPressedOk)
	{
		TargetSelectorAssetClass = ChosenClass;
	}

	return bPressedOk;
}
