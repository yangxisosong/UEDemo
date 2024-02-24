#pragma once
#include "AbilitySystemGlobals.h"
#include "DLGameplayTagLibrary.h"
#include "GameAbilitySysDef.h"
#include "DLAbilitySystemGlobal.generated.h"


class UInputMappingContext;

class UInputAction;

USTRUCT()
struct FDLAbilityInputAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		UInputAction* Pressed;

	UPROPERTY(EditAnywhere)
		UInputAction* Released;
};


USTRUCT(BlueprintType)
struct FDLAbilityUnitMaterialMapPhysics
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTag AbilityUnitMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TEnumAsByte<EPhysicalSurface>> PhysicsSurfaceType;
};


UCLASS(config = Game)
class DLABILITYSYSTEM_API UDLAbilitySystemGlobal
	: public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:

	UPROPERTY()
		FGameplayTag ActivateFailGEStackCountTag;

	UPROPERTY(Config, EditAnywhere)
		FName ActivateFailGEStackCountName;

	UPROPERTY(Config, EditAnywhere)
		TMap<EPrimaryInputID, TSoftObjectPtr<UInputAction>> AbilityInputActions;

	UPROPERTY(Config, EditAnywhere)
		TSoftObjectPtr<UInputMappingContext> IMCAbilityBase;

	UPROPERTY(Config, EditAnywhere)
		TSoftObjectPtr<UInputMappingContext> IMCAbilityDebug;

	// GA 过滤的 Tag
	UPROPERTY(Config, EditAnywhere)
		TArray<FGameplayTag> FilterGATagArr;

	// 攻击类型 的 RootTag
	UPROPERTY(Config, EditAnywhere)
		FGameplayTag AttackTypeTagRoot;
		

	// 技能音效的 DataRegistry 的ID 
	UPROPERTY(Config, EditAnywhere)
		TSoftObjectPtr<UDataTable> DTAbilityAudio;

	// 技能中的单位的 材质 与 物理材质的映射
	UPROPERTY(Config, EditAnywhere)
		TArray<FDLAbilityUnitMaterialMapPhysics> UnitMaterialMapPhysics;

	// 默认的击中单位需要的满足的 Tag
	UPROPERTY(Config, EditAnywhere)
		FDLGameplayTagRequirements DefaultHitUnitRequirementTags;

public:

	static UDLAbilitySystemGlobal& Get()
	{
		return *Cast<UDLAbilitySystemGlobal>(IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals());
	}

	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;

	virtual FGameplayAbilityActorInfo* AllocAbilityActorInfo() const override;


protected:

	virtual void InitGlobalTags() override;
};
