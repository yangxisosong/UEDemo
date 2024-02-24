// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLUnitAbilitySystemComponent.h"
#include "Components/ActorComponent.h"
#include "DLPlayerAbilitySysComponent.generated.h"

class UAbilityChainAsset;
class UDLPlayerInputCmdAbility;
class UAbilityChain;

UCLASS()
class DLABILITYSYSTEM_API UDLPlayerAbilitySysComponent
	: public UDLUnitAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UDLPlayerAbilitySysComponent();

	EInputEvent QueryAbilityInputState(const int32 InputId) const;

	bool InitAbilityChain(UAbilityChainAsset* Asset);

	FDLAbilityInputChange& OnAbilityInputChange() { return AbilityInputChange; }

	bool InjectInputActiveAbility(int32 InputKey, UObject* AbilityContext);

	// 禁用技能输入
	void DisabledAbilityInput();

	// 启用技能
	void EnabledAbilityInput();

protected:

#pragma region UActorComponent

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

#pragma region UAbilitySystemComponent

	// 接管系统的输入
	virtual void AbilityLocalInputPressed(int32 InputId) override;
	virtual void AbilityLocalInputReleased(int32 InputId) override;

	virtual void BindAbilityActivationToInputComponent(UInputComponent* InputComponent, FGameplayAbilityInputBinds BindInfo) override;
	virtual void OnPlayerControllerSet() override;
	virtual bool GetShouldTick() const override;
#pragma endregion

#pragma region UDLCharacterAbilitySystemComponent

	virtual void DisplayDebugInfo(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) const override;

#pragma endregion


private:

	void OnClientActiveAbility(UGameplayAbility* Ability) const;
	void OnClientAbilityEnd(const FAbilityEndedData& Data) const;
	void DispatchAbilityLocalInput(EInputEvent Event, int32 InputId);
	void RefreshDebugInput();
private:

	FDLAbilityInputChange AbilityInputChange;
	TMap<int32, EInputEvent> AbilityInputMap;
	int32 ViewportHandle = INDEX_NONE;

private:

	UPROPERTY()
		UAbilityChain* AbilityChain = nullptr;

	UPROPERTY()
		TSubclassOf<UGameplayAbility> CurrentAbilityChainCheckout;


	bool bIsDebugInput = false;

	bool bIsEnabledInput = false;
};
