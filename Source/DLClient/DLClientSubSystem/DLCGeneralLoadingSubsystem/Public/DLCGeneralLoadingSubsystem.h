// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLClientSubsystemBase.h"
#include "IDLLoadingProcessor.h"
#include "UObject/Object.h"
#include "DLCGeneralLoadingSubsystem.generated.h"

/**
 *
 */
UCLASS(Abstract)
class DLCGENERALLOADINGSUBSYSTEM_API UDLCGeneralLoadingSubsystem
	: public UDLClientSubsystemBase
	, public IDLLoadingProcessor
{

	GENERATED_BODY()

protected:

	virtual void Tick(float DeltaTime) override;

	virtual bool IsLoading(FString& OutDebugLoadingReason) const override;

	virtual const FString& GetProcessorName() const override;

	virtual TSharedRef<SWidget> GetOrCreateLoadingUI() override;

	virtual void OnShowLoadingUI() override;

	virtual void OnRemoveLoadingUI() override;

	virtual EDLLoadingProcessorPriority GetPriority() const override;


protected:

	bool CheckNeedLoading();


private:

	void HandlePreLoadMap(const FString& String);

	void HandlePostLoadMap(UWorld* World);

protected:

	virtual void InitSubsystem(const FClientSubsystemCollection& Collection) override;

	virtual void UninitSubsystem() override;

private:

	FString DebugReason;

	bool bCurrentlyInLoadMap = false;
	FString LoadingMapName;

	bool NeedLoading = false;

	FDelegateHandle PreLoadMapHandle;
	FDelegateHandle PostLoadMapHandle;

	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<UUserWidget> LoadingWidgetClass;

	UPROPERTY()
		UUserWidget* LoadingWidget = nullptr;

	// Hole LoadingWidget 内部 SWidget 的 Ref
	TSharedPtr<SWidget> LoadingSWidget;
};
