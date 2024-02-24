#include "DLUIExtension/DLUIExtensionSysInterface.h"

#include "DLUIExtension/DLUIExtensionWidgetInterface.h"


FDLUIExtensionHandle FDLUIExtensionHandle::InvalidHandle = FDLUIExtensionHandle::GeneratedHandle();

FDLUIExtensionHandle UDLUIExtensionSystemInterface::RegisterExtensionPoint(const FDLUIExtensionPointDesc& PointDesc)
{
	return FDLUIExtensionHandle::InvalidHandle;
}

void UDLUIExtensionSystemInterface::UnregisterExtensionPoint(FDLUIExtensionHandle Handle)
{
}

void UDLUIExtensionSystemInterface::RegisterExtensionWidget(FDLUIExtensionWidgetDesc WidgetDesc,
	const FDLUIExtensionWidgetCondition& Condition)
{
}

void UDLUIExtensionSystemInterface::RegisterOrInstanceExtensionWidget(FDLUIExtensionWidgetDesc WidgetDesc,
	const FDLUIExtensionWidgetCondition& Condition, const FCanInstanceExtensionWidget& CheckCanInstanceWidget)
{
}

FDLUIExtensionHandle UDLUIExtensionSystemInterface::InstanceExtensionWidget(FDLUIExtensionWidgetDesc WidgetDesc)
{
	return FDLUIExtensionHandle::InvalidHandle;
}

void UDLUIExtensionSystemInterface::DestroyExtensionWidget(FDLUIExtensionHandle Handle)
{

}

UUserWidget* UDLUIExtensionSystemInterface::GetWidgetWithTag(FGameplayTag Tag)
{
	return nullptr;
}

void UDLUIExtensionSystemInterface::SetWidgetVisibleWithTag(FGameplayTag Tag, bool IsShow)
{

}


void IDLExtensionUserWidgetInterface::OnExtensionAction(const EDLUIExtensionActionType ActionType)
{
	if (ActionType == EDLUIExtensionActionType::AddWidgetAndHide)
	{
		CastChecked<UUserWidget>(this)->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (ActionType == EDLUIExtensionActionType::AddWidgetAndShow)
	{
		CastChecked<UUserWidget>(this)->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}
