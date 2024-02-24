#pragma once

#include "AbilitySystemComponent.h"
#include "DLAbilityAction.h"
#include "DLAbilityActionApplyHitDefine.generated.h"


UCLASS(DisplayName = ApplyHitDefine)
class UDLAbilityActionApplyHitDefine
	: public UDLAbilityAction
{
	GENERATED_BODY()

protected:

	virtual void OnExec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event) override;

public:

	UPROPERTY(EditAnywhere, meta = (GetOptions = "GetHitDefineIdOptions"))
		FName HitDefineId;

private:

	UFUNCTION()
	TArray<FName> GetHitDefineIdOptions() const;
};
