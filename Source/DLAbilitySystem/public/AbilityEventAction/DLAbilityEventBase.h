#pragma once

#include "CoreMinimal.h"
#include "DLAbilityActionFeatures.h"
#include "DLAbilityEventBase.generated.h"


class UDetermineTargetsStrategy;
class UDLUnitAbilitySystemComponent;
class UDLGameplayAbilityBase;
class UDLAbilityAction;

USTRUCT(BlueprintType)
struct FDLAbilityActionContext
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
		UDLGameplayAbilityBase* Ability = nullptr;
};


UCLASS(Abstract, EditInlineNew, CollapseCategories)
class DLABILITYSYSTEM_API UDLAbilityEventBase : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Instanced)
		TArray<UDLAbilityAction*> ActionList;

public:

	virtual bool Validation(TArray<FString>& OutErrMsg) { return true; };

protected:

	const UDLGameplayAbilityBase* GetOuterAbility()const;

	void TriggerActionList(const FDLAbilityActionContext& Context);
};


UCLASS(meta = (DisplayName = HitUnitEvent))
class DLABILITYSYSTEM_API UDLAbilityEventWeaponHitActor
	: public UDLAbilityEventBase
	, public IDLAbilityAFHitActor
	, public IDLAbilityAFHitResults
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (GetOptions = GetHitEventTagsOption))
		FGameplayTagContainer  HitEventTags;


private:

	UFUNCTION()
		FGameplayTagContainer GetHitEventTagsOption() const;

private:

	// Runtime Data

	FGameplayAbilityTargetDataHandle TargetDataHandle;

public:

	UDLAbilityEventWeaponHitActor()
	{
	}


	void TriggerAction(const FDLAbilityActionContext& Context, const FGameplayAbilityTargetDataHandle& DataHandle)
	{
		TargetDataHandle = DataHandle;

		this->TriggerActionList(Context);
	}

protected:

	virtual bool Validation(TArray<FString>& OutErrMsg) override;

	virtual FGameplayAbilityTargetDataHandle GetTargetData() const override;

	virtual const TArray<FHitResult>& GetHitResults() const override;
};



UCLASS(meta = (DisplayName = CustomEvent))
class DLABILITYSYSTEM_API UDLAbilitySimpleEvent
	: public UDLAbilityEventBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
		FGameplayTagContainer  EventTags;

public:

	void TriggerAction(const FDLAbilityActionContext& Context);

protected:

	virtual bool Validation(TArray<FString>& OutErrMsg) override;;

};


UENUM(BlueprintType)
enum class EDLAbilityStageChangeType : uint8
{
	None,
	EnterChannel,
	EndChannel,
	EnterPre,
	EndPre,
	EnterSpell,
	EndSpell,
	EnterPost,
	EndPost,
};


UCLASS(meta = (DisplayName = StageChangeEvent))
class DLABILITYSYSTEM_API UDLAbilityStageChangeEvent
	: public UDLAbilityEventBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
		EDLAbilityStageChangeType EventType;

public:

	void TriggerAction(const FDLAbilityActionContext& Context);

};
