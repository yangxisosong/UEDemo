#pragma once

#include "DLGameplayTagLibrary.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"

#include "DLUIExtensionTypeDef.generated.h"

UENUM(BlueprintType)
enum class EDLUIExtensionActionType : uint8
{
	None,
	AddWidgetOnly,
	AddWidgetAndShow,
	AddWidgetAndHide,
	RemoveWidget,
};

UENUM(BlueprintType)
enum class EDLUIExtensionPointRemoveAction: uint8
{
	None,
	//只移除扩展点内的UI 不移除扩展点
	RemoveWidgetOnly,
	//移除扩展点和UI
	RemoveWidgetAndPoint
};


/**
* @brief UI 扩展行为的负载数据， 支持安全的Cast
*/
struct FDLUIExtensionActionPayloadData
{
	FDLUIExtensionActionPayloadData(const UScriptStruct* InType, const void* InPayloadData)
		:Type(InType), PayloadData(InPayloadData)
	{
	}

	template<typename PlayloadType>
	const PlayloadType* GetPlayloadData()
	{
		const UScriptStruct* TargetType = TBaseStructure<PlayloadType>::Get();
		ensureAlwaysMsgf(TargetType == Type, TEXT("转换失败，与目标类型不符合"));
		if (Type == TargetType && PayloadData)
		{
			return static_cast<const PlayloadType*>(PayloadData);
		}
		return nullptr;
	}

	template<typename PlayloadType>
	bool CanCast()
	{
		const UScriptStruct* TargetType = TBaseStructure<PlayloadType>::Get();
		return TargetType == Type;
	}


private:

	FDLUIExtensionActionPayloadData(const FDLUIExtensionActionPayloadData&) = delete;
	FDLUIExtensionActionPayloadData(FDLUIExtensionActionPayloadData&&) = delete;
	FDLUIExtensionActionPayloadData& operator== (FDLUIExtensionActionPayloadData&&) = delete;
	FDLUIExtensionActionPayloadData& operator== (const FDLUIExtensionActionPayloadData&) = delete;

private:

	const UScriptStruct* Type = nullptr;

	const void* PayloadData = nullptr;
};



/**
* @brief 描述一个 UI 扩展点
*/
USTRUCT(BlueprintType)
struct FDLUIExtensionPointDesc
{
	GENERATED_BODY()
public:

	// 扩展点的 ID 标识 [必填]
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayTag ExtensionPointId;

	// 允许的扩展的 Class [可选]
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<TSoftClassPtr<UWidget>> AllowAnyWidgetClassArray;


	// 允许支持的接口 [可选]
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<TSoftClassPtr<UInterface>> AllowAnyInterfaceClass;


	// 允许匹配的 Tag [可选]
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FDLGameplayTagRequirements RequirementTags;

public:

	DECLARE_DELEGATE_TwoParams(FOnExecExtensionPointAction, EDLUIExtensionActionType, const FDLUIExtensionActionPayloadData&);
	DECLARE_DELEGATE_OneParam(FOnExecExtensionPointRemoveAction, EDLUIExtensionPointRemoveAction);

	// 当 UIExtensionSystem 生成事件时，调用这个 delegate
	FOnExecExtensionPointAction OnExecExtensionPointAction;

	// 移除扩展点代理
	FOnExecExtensionPointRemoveAction OnExecExtensionPointRemoveAction;

};


/**
* @brief UI 扩展系统的通用 Handle
*/
USTRUCT(BlueprintType)
struct FDLUIExtensionHandle
{
	GENERATED_BODY()
public:

	int32 MagicId = 0;

	static FDLUIExtensionHandle GeneratedHandle()
	{
		static int32 StaticHandleCount = 0;
		return StaticHandleCount++;
	}

	static FDLUIExtensionHandle InvalidHandle;

	FDLUIExtensionHandle()
	{
		MagicId = GeneratedHandle().MagicId;
	}

	/** Used so we can have a TMap of this struct */
	FORCEINLINE friend uint32 GetTypeHash(const FDLUIExtensionHandle& Tag)
	{
		return Tag.MagicId;
	}

	bool operator==(const FDLUIExtensionHandle& Handle) const { return  MagicId == Handle.MagicId; }

private:

	FDLUIExtensionHandle(const int32 MagicNumber) : MagicId(MagicNumber) { }

	bool operator!=(const FDLUIExtensionHandle& Handle) const { return  MagicId != Handle.MagicId; }

	FString ToString() const
	{

		// TODO 做一个可直观的一个文本
		return FString::Printf(TEXT("DLUIExtensionHandle %d"), MagicId);
	}
};




DECLARE_MULTICAST_DELEGATE_TwoParams(FOnConditionInstanceOrDestroyWidget, const UScriptStruct*, const void* PayloadData);


struct FDLUIExtWidgetDelegateCondition
{
	TSharedPtr<FOnConditionInstanceOrDestroyWidget> OnCondition;

	FGameplayTag MsgType;

	EDLUIExtensionActionType ExecAction = EDLUIExtensionActionType::AddWidgetOnly;
};


USTRUCT(BlueprintType)
struct FDLUIExtWidgetMsgCondition
{
	GENERATED_BODY()
public:

	// 监听 MsgSystem 的事件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTag MsgType;

	// MsgSystem 携带的数据类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UScriptStruct* MsgPayloadDataType;

	// 触发这个消息需要执行的事件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EDLUIExtensionActionType ExecAction = EDLUIExtensionActionType::AddWidgetOnly;
};

USTRUCT(BlueprintType)
struct FDLUIExtensionWidgetCondition
{
	GENERATED_BODY()

public:

	// 执行操作的 Msg
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FDLUIExtWidgetMsgCondition> InstansMsgConditions;

public:

	// UIExtensionSystem 会监听这些 Delegate，当外部触发这个 Delegate 时会执行对应的动作
	TArray<FDLUIExtWidgetDelegateCondition> OnConditions;
};


/**
* @brief 需要扩展的 Widget 的描述
*/
USTRUCT(BlueprintType)
struct FDLUIExtensionWidgetDesc
{
	GENERATED_BODY()
public:

	// 需要实例化 Widget 的 Class
	// 系统仅支持配置 软引用，使用同步懒加载，如果发现界面在懒加载影响体验，请自行在外围的系统中对这个WidgetClass进行预加载
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = DLExtensionUserWidgetInterface))
		TSoftClassPtr<UUserWidget> WidgetClass;

	// 挂载点的 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTag ExtPointTag;

public:

	DECLARE_DELEGATE_RetVal_OneParam(UUserWidget*, FExtensionUserWidgetAllocator, TSoftClassPtr<UUserWidget>);
	DECLARE_DELEGATE_OneParam(FExtensionUserWidgetReleaser, UUserWidget*);

	// 扩展 窗口的 分配器与释放器，他们必须成对出现
	// 当需要扩展的窗口会被频繁的 Add/Remove 时，可以用来实现 对象池等技术 以降低性能开销
	FExtensionUserWidgetAllocator ExtensionUserWidgetAllocator;
	FExtensionUserWidgetReleaser ExtensionUserWidgetReleaser;
};
