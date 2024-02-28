#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "IPropertyTypeCustomization.h"

class IDetailLayoutBuilder;

class FDLCustomizationDetail : public IDetailCustomization
{

public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	//virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

		/** Called when details should be customized */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	//virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
	//                             IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	//virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder,
	//                               IPropertyTypeCustomizationUtils& CustomizationUtils) override;

private:
	TWeakObjectPtr<UFunction> Function;
};

class FDLCustomizationProperty : public IPropertyTypeCustomization
{

public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();


	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
	                             IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder,
	                               IPropertyTypeCustomizationUtils& CustomizationUtils) override;

private:
	TWeakObjectPtr<UFunction> Function;
};
