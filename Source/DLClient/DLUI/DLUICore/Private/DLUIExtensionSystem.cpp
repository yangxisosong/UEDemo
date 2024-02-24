#include "DLUIExtensionSystem.h"
#include "DLUIManagerSubsystem.h"
#include "GameplayTagContainer.h"
#include "DLClient/DLClientSubSystem/DLUIManagerSubsystem/Public/DLUIExtension/DLUIExtensionPoint.h"

#define LOCTEXT_NAMESPACE "UDLUIExtensionWidget"

DEFINE_LOG_CATEGORY(LogUIExtensionSystem);

FDLUIExtensionHandle UDLUIExtensionSystem::RegisterExtensionPoint(const FDLUIExtensionPointDesc& PointDesc)
{
	const auto Handle = FDLUIExtensionHandle::GeneratedHandle();
	PointMap.Add(PointDesc.ExtensionPointId, PointDesc);
	PointHandleMap.Add(Handle, PointDesc);
	if (CacheWidgetAction.Contains(PointDesc.ExtensionPointId))
	{
		AddWidgetToPoint(PointDesc.ExtensionPointId, CacheWidgetAction[PointDesc.ExtensionPointId],
		                 nullptr, nullptr);

		CacheWidgetAction.Remove(PointDesc.ExtensionPointId);
	}
	return Handle;
}

void UDLUIExtensionSystem::UnregisterExtensionPoint(FDLUIExtensionHandle Handle)
{
	if (PointHandleMap.Contains(Handle))
	{
		if (PointMap.Contains(PointHandleMap[Handle].ExtensionPointId))
		{
			PointMap[PointHandleMap[Handle].ExtensionPointId].
				OnExecExtensionPointRemoveAction.Execute(EDLUIExtensionPointRemoveAction::RemoveWidgetAndPoint);

			PointMap.Remove(PointHandleMap[Handle].ExtensionPointId);
		}
		PointHandleMap.Remove(Handle);
	}
}

void UDLUIExtensionSystem::RegisterExtensionWidget(FDLUIExtensionWidgetDesc WidgetDesc,
                                                   const FDLUIExtensionWidgetCondition& Condition)
{
	UIExtensionWidgetInfo.Add(WidgetDesc.ExtPointTag, WidgetDesc);

	UGameplayMessageSubsystem* MsgSystem = UDLUIManagerSubsystem::Get(this)->
	                                       GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>();

	UE_LOG(LogUIExtensionSystem, Log, TEXT("RegistMessage"));

	ensureAlwaysMsgf(MsgSystem, TEXT("MsgSystem 系统一定能够启动"));

	for (auto i : Condition.OnConditions)
	{
		i.OnCondition.Get()->AddLambda([this,i](const UScriptStruct* SenderStructType, const void* PayloadData)
		{
			this->OnGetMessage(i.MsgType, i.ExecAction, SenderStructType, PayloadData);
		});
	}

	if (Condition.InstansMsgConditions.Num() > 0)
	{
		for (auto j : Condition.InstansMsgConditions)
		{
			auto ThunkCallback = [this, j, WidgetDesc](FGameplayTag ActualTag, const UScriptStruct* SenderStructType,
			                                           const void* SenderPayload)
			{
				this->OnGetMessage(WidgetDesc.ExtPointTag, j.ExecAction, SenderStructType, SenderPayload);
			};

			const auto Handle = MsgSystem->RegisterListener(
				j.MsgType, ThunkCallback, j.MsgPayloadDataType, EGameplayMessageMatch::ExactMatch);

			ListenerHandles.Add(Handle);
		}
	}
}

void UDLUIExtensionSystem::RegisterOrInstanceExtensionWidget(FDLUIExtensionWidgetDesc WidgetDesc,
                                                             const FDLUIExtensionWidgetCondition& Condition,
                                                             const FCanInstanceExtensionWidget& CheckCanInstanceWidget)
{
	const bool CanInstance = CheckCanInstanceWidget.Execute();

	RegisterExtensionWidget(WidgetDesc, Condition);

	if (CanInstance)
	{
		InstanceExtensionWidget(WidgetDesc);

		AddWidgetToPoint(WidgetDesc.ExtPointTag, EDLUIExtensionActionType::AddWidgetOnly,
		                 nullptr, nullptr);
	}
}

FDLUIExtensionHandle UDLUIExtensionSystem::InstanceExtensionWidget(FDLUIExtensionWidgetDesc WidgetDesc)
{
	const auto Handle = FDLUIExtensionHandle::GeneratedHandle();
	CreatWidgetByClass(WidgetDesc.ExtPointTag, WidgetDesc.WidgetClass);

	WidgetHandleMap.Add(Handle, WidgetDesc.ExtPointTag);

	return Handle;
}

void UDLUIExtensionSystem::DestroyExtensionWidget(FDLUIExtensionHandle Handle)
{
	if (WidgetHandleMap.Contains(Handle))
	{
		if (PointMap.Contains(WidgetHandleMap[Handle]))
		{
			PointMap[WidgetHandleMap[Handle]].OnExecExtensionPointRemoveAction.
			                                  Execute(EDLUIExtensionPointRemoveAction::RemoveWidgetOnly);
		}
		WidgetMap[WidgetHandleMap[Handle]] = nullptr;
		WidgetMap.Remove(WidgetHandleMap[Handle]);
		WidgetHandleMap.Remove(Handle);
	}
}

UUserWidget* UDLUIExtensionSystem::GetWidgetWithTag(FGameplayTag Tag)
{
	if (WidgetMap.Contains(Tag))
	{
		return WidgetMap[Tag];
	}
	return nullptr;
}

void UDLUIExtensionSystem::SetWidgetVisibleWithTag(FGameplayTag Tag, bool IsShow)
{
	const UDLUIExtensionWidget* Widget = nullptr;
	if (WidgetMap.Contains(Tag))
	{
		Widget = WidgetMap[Tag];
	}
	else
	{
		if(UIExtensionWidgetInfo.Contains(Tag))
		{
			Widget= CreatWidgetByClass(Tag, UIExtensionWidgetInfo[Tag].WidgetClass);
		}
	}

	if (PointMap.Contains(Tag) && Widget)
	{
		const FDLUIExtensionActionPayloadData Data(nullptr, nullptr);
		EDLUIExtensionActionType ActionType;
		if (IsShow)
		{
			ActionType = EDLUIExtensionActionType::AddWidgetAndShow;
		}
		else
		{
			ActionType = EDLUIExtensionActionType::RemoveWidget;
		}

		PointMap[Tag].OnExecExtensionPointAction.Execute(ActionType, Data);
	}
}

void UDLUIExtensionSystem::OnGetMessage(const FGameplayTag Tag, EDLUIExtensionActionType ActionType,
                                        const UScriptStruct* Type, const void* PayloadData)
{
	if (ActionType != EDLUIExtensionActionType::None)
	{
		const FDLUIExtensionActionPayloadData Data(Type, PayloadData);
		if (WidgetMap.Contains(Tag))
		{
			WidgetMap[Tag]->OnExtensionAction(ActionType);
		}
		else
		{
			const auto ClassType = GetWidgetClassWithTag(Tag);

			const auto OutWidget = CreatWidgetByClass(Tag, ClassType);

			OutWidget->OnExtensionAction(ActionType);
			OutWidget->OnWidgetInstance(Data);
		}

		//窗口和锚点都创建好 锚点执行Action
		if (WidgetMap.Contains(Tag))
		{
			if (PointMap.Contains(Tag))
			{
				PointMap[Tag].OnExecExtensionPointAction.Execute(ActionType, Data);
			}
		}
	}
}

void UDLUIExtensionSystem::AddWidgetToPoint(const FGameplayTag Tag, EDLUIExtensionActionType ActionType,
                                            const UScriptStruct* Type, const void* PayloadData)
{
	if (PointMap.Contains(Tag))
	{
		const FDLUIExtensionActionPayloadData Data(Type, PayloadData);
		PointMap[Tag].OnExecExtensionPointAction.Execute(ActionType, Data);
	}
	else
	{
		CacheWidgetAction.Add(Tag, ActionType);
	}
}

UDLUIExtensionWidget* UDLUIExtensionSystem::CreatWidgetByClass(const FGameplayTag Tag, TSoftClassPtr<UUserWidget> ClassPtr)
{
	const auto Widget = NewObject<UDLUIExtensionWidget>(this,ClassPtr.LoadSynchronous());

	if(Widget)
	{
		WidgetMap.Add(Tag, Widget);

		return Widget;
	}
	return nullptr;
}

TSoftClassPtr<UUserWidget> UDLUIExtensionSystem::GetWidgetClassWithTag(FGameplayTag Tag)
{
	if (UIExtensionWidgetInfo.Contains(Tag))
	{
		return UIExtensionWidgetInfo[Tag].WidgetClass;
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE
