#include "DLUIExtension/DLUIExtensionPoint.h"

#include "DLUIManagerSubsystem.h"
#include "DLUIExtension/DLUIExtensionWidget.h"
#include "Editor/WidgetCompilerLog.h"

#define LOCTEXT_NAMESPACE "UDLUIExtensionWidgetBase"

UDLUIExtensionPoint::UDLUIExtensionPoint(const FObjectInitializer& ObjectInitializer)
	: UDynamicEntryBoxBase(ObjectInitializer)
{
}

void UDLUIExtensionPoint::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
}

TSharedRef<SWidget> UDLUIExtensionPoint::RebuildWidget()
{
	auto Widget = UDynamicEntryBoxBase::RebuildWidget();
	RegistLisenter();
	return Widget;
}

void UDLUIExtensionPoint::SynchronizeProperties()
{
	Super::SynchronizeProperties();

#if WITH_EDITORONLY_DATA
	if (IsDesignTime() && MyPanelWidget.IsValid())
	{
		if (!EntryWidgetClass || !IsEntryClassValid(EntryWidgetClass.Get()))
		{
			ResetInternal(true);
		}
		else
		{
			ResetInternal();
			CreateEntryInternal(EntryWidgetClass.Get());
		}
	}
#endif
}

void UDLUIExtensionPoint::AddOrRemoveCallback(FGameplayTag ActualTag, bool IsAdd)
{
	SetEntryWidgetClass();

	if (IsAdd)
	{
		if (EntryWidgetClass)
		{
			CreateEntryInternal(EntryWidgetClass.Get());
		}
	}
	else
	{
		ResetInternal(true);
	}
}

UUserWidget* UDLUIExtensionPoint::GetChildrenWidget()
{
	return ChildrenWidget.Get();
}

void UDLUIExtensionPoint::RegistLisenter()
{
	if (UDLUIManagerSubsystem::Get(this))
	{
		ExtensionPointInfo.OnExecExtensionPointAction.BindLambda(
			[this](EDLUIExtensionActionType ActionType, const FDLUIExtensionActionPayloadData& Data)
			{
				this->AddCurrentWidget(ActionType);
			});

		ExtensionPointInfo.OnExecExtensionPointRemoveAction.BindUObject(this, &UDLUIExtensionPoint::OnRemoveWidget);

		UDLUIManagerSubsystem::Get(this)->GetUIExtensionSystem()
		                                ->RegisterExtensionPoint(ExtensionPointInfo);
	}
}

void UDLUIExtensionPoint::SetEntryWidgetClass()
{
	if (UDLUIManagerSubsystem::Get(this))
	{
		const auto TempClass = UDLUIManagerSubsystem::Get(this)->
		                       GetUIExtensionSystem()->GetWidgetWithTag(this->ExtensionPointInfo.ExtensionPointId);
		EntryWidgetClass = nullptr;
		if (TempClass)
		{
			//EntryWidgetClass = TempClass;
		}
	}
}

void UDLUIExtensionPoint::AddCurrentWidget(EDLUIExtensionActionType ActionType)
{
	const auto TempWidget = UDLUIManagerSubsystem::Get(this)->
	                        GetUIExtensionSystem()->GetWidgetWithTag(this->ExtensionPointInfo.ExtensionPointId);

	if (ActionType == EDLUIExtensionActionType::RemoveWidget)
	{
		ResetInternal(true);
		ChildrenWidget = nullptr;
	}
	else if (ActionType == EDLUIExtensionActionType::AddWidgetOnly ||
		ActionType == EDLUIExtensionActionType::AddWidgetAndHide ||
		ActionType == EDLUIExtensionActionType::AddWidgetAndShow)
	{
		bool IsShow = true;
		if (ActionType == EDLUIExtensionActionType::AddWidgetAndHide)
		{
			TempWidget->SetVisibility(ESlateVisibility::Collapsed);
			IsShow = false;
		}
		if (TempWidget!= ChildrenWidget)
		{
			ChildrenWidget = TempWidget;
			AddEntryChild(*TempWidget);

			OnAddChildren.Broadcast();
			if (Cast<UDLUIExtensionWidget>(TempWidget))
			{
				Cast<UDLUIExtensionWidget>(TempWidget)->OnAddWidgetToPoint(IsShow);
			}
		}
	}
}

void UDLUIExtensionPoint::OnRemoveWidget(EDLUIExtensionPointRemoveAction ActionType)
{
	if (ActionType == EDLUIExtensionPointRemoveAction::RemoveWidgetOnly)
	{
		ResetInternal(true);
	}
	else if (ActionType == EDLUIExtensionPointRemoveAction::RemoveWidgetAndPoint)
	{
		ResetInternal(true);
		ExtensionPointInfo.OnExecExtensionPointAction.Unbind();
	}
}

void UDLUIExtensionPoint::AddCurrentWidget(UUserWidget* Widget, EDLUIExtensionActionType ActionType,
                                           const FDLUIExtensionActionPayloadData& Data)
{
}


#if WITH_EDITOR
void UDLUIExtensionPoint::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	// 编译时的检测信息
	//if (!EntryWidgetClass)
	//{
	//	CompileLog.Error(FText::Format(LOCTEXT("Error_DynamicEntryBox_MissingEntryClass", "{0} has no EntryWidgetClass specified - required for any Dynamic Entry Box to function."), FText::FromString(GetName())));
	//}
	//else if (CompileLog.GetContextClass() && EntryWidgetClass->IsChildOf(CompileLog.GetContextClass()))
	//{
	//	CompileLog.Error(FText::Format(LOCTEXT("Error_DynamicEntryBox_RecursiveEntryClass", "{0} has a recursive EntryWidgetClass specified (reference itself)."), FText::FromString(GetName())));
	//}
}
#endif

#if WITH_EDITOR
const FText UDLUIExtensionPoint::GetPaletteCategory()
{
	return LOCTEXT("DLCustomUI", "DLCustomUI");
}
#endif

#undef LOCTEXT_NAMESPACE
