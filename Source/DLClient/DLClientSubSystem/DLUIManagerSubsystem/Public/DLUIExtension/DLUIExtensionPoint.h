// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DLUIExtensionTypeDef.h"
#include "UObject/ObjectMacros.h"
#include "GameplayTagContainer.h"
#include "Components/DynamicEntryBoxBase.h"
#include "DLUIExtensionPoint.generated.h"

class APlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAddChildren);

/**
 * 锚点控件 标定动态创建UI的位置
 */
UCLASS(NotBlueprintable)
class DLUIMANAGERSUBSYSTEM_API UDLUIExtensionPoint
	: public UDynamicEntryBoxBase
{
	GENERATED_BODY()

public:
	UDLUIExtensionPoint(const FObjectInitializer& ObjectInitializer);
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	//Start of UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;
	//~End of UWidget interface

	template <typename WidgetT = UUserWidget>
	UFUNCTION(BlueprintCallable)
	WidgetT* CreateEntry(const TSubclassOf<WidgetT>& ExplicitEntryClass = nullptr)
	{
		TSubclassOf<UUserWidget> EntryClass = ExplicitEntryClass ? ExplicitEntryClass : EntryWidgetClass;
		if (EntryClass && EntryClass->IsChildOf(WidgetT::StaticClass()) && IsEntryClassValid(EntryClass))
		{
			return Cast<WidgetT>(CreateEntryInternal(EntryClass));
		}
		return nullptr;
	}

	void AddOrRemoveCallback(FGameplayTag ActualTag, bool IsAdd);

public:
	UFUNCTION(BlueprintCallable)
		UUserWidget* GetChildrenWidget();

	UPROPERTY(BlueprintAssignable, Category = "Event")
		FOnAddChildren OnAddChildren;
	
private:
	void RegistLisenter();

	void SetEntryWidgetClass();

	void AddCurrentWidget(EDLUIExtensionActionType ActionType);

	void OnRemoveWidget(EDLUIExtensionPointRemoveAction ActionType);
	
	void AddCurrentWidget(UUserWidget* Widget, EDLUIExtensionActionType ActionType, const FDLUIExtensionActionPayloadData& Data);

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;

	virtual const FText GetPaletteCategory() override;
#endif

private:
	TWeakObjectPtr<UUserWidget> ChildrenWidget;
public:
	//扩展点信息
	UPROPERTY(EditAnywhere, Category = "UI Extension")
	FDLUIExtensionPointDesc ExtensionPointInfo;

	//UI接受的消息Tag
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
	//FGameplayTag ExtensionTag;

	//预览作用 实际显示的窗口按照tag 配置的Widget 生成
	UPROPERTY(EditAnywhere, Category = "UI Extension")
	TSubclassOf<UUserWidget> EntryWidgetClass = nullptr;

	TFunction<void(FGameplayTag ActualTag, const UScriptStruct* SenderStructType, const void* SenderPayload)> Callback;
};


