#pragma once

#include "CoreMinimal.h"
#include "DLAbilityEventBase.h"
#include "DLAbilityAction.generated.h"


class UDetermineTargetsStrategy;
struct FDLAbilityActionContext;
class UGameplayEffect;
class UDLAbilityEventBase;



UCLASS(Abstract, Blueprintable, CollapseCategories, EditInlineNew)
class UDLAbilityAction
	: public UObject
{
	GENERATED_BODY()

public:

	UDLAbilityAction();

	void Exec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event);

	virtual bool Validation(TArray<FString>& OutErrMsg) { return true; }

protected:

	/**
	 * @brief 蓝图扩展 AbilityAction
	 * @param Context 执行这个Action的上下文信息
	 * @param Event  这个动作对应的事件，使用的时候应该用 GetActionFeature 来检索对应的事件支持接口
	 * @see GetActionFeature OnExec
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = OnExec))
		void K2_OnExec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event);

private:


	/**
	 * @brief C++ 扩展 AbilityAction
	 * @param Context 执行这个Action的上下文信息
	 * @param Event  这个动作对应的事件，使用的时候应该用 GetActionFeature 来检索对应的事件支持接口
	 *
	 * @note 如果蓝图实现了 K2_OnExec  那么这个函数将不会被调用
	 * @see GetActionFeature  K2_OnExec
	 */
	virtual void OnExec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event) {}

protected:

	template<typename ActionFeature>
	ActionFeature* GetActionFeature(UDLAbilityEventBase* Event)
	{
		auto Feature = Cast<ActionFeature>(Event);
		ensureAlwaysMsgf(Feature, TEXT("不支持Feaure"));
		return Feature;
	}


	const UDLGameplayAbilityBase* GetOuterAbility()const;

protected:

	bool bCallBPExec = false;
};


