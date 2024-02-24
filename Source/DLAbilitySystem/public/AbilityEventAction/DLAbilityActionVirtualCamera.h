#pragma once

#include "CoreMinimal.h"
#include "DLAbilityAction.h"
#include "DLGameplayAbilityBase.h"
#include "VCDataAssetDef.h"
#include "Interface/IDLVirtualCameraController.h"
#include "DLAbilityActionVirtualCamera.generated.h"


UCLASS(DisplayName = ActivateVirtualCamera)
class UDLAbilityActionActivateVirtualCamera
	: public UDLAbilityAction
{
	GENERATED_BODY()

public:

	// 虚拟相机的定义ID
	UPROPERTY(EditDefaultsOnly,  meta = (GetOptions = "GetVirtualCameraDefIdOptions"))
		FName VirtualCameraDefId;

	// 激活时，设定这个 VC 的超时策略
	UPROPERTY(EditDefaultsOnly)
		EVCTimeOutAction ActivateTimeOutAction;

private:

	UFUNCTION()
		TArray<FName> GetVirtualCameraDefIdOptions() const;
public:

	virtual void OnExec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event) override;
};


UCLASS(DisplayName = StopVirtualCamera)
class UDLAbilityActionStopVirtualCamera
	: public UDLAbilityAction
{
	GENERATED_BODY()

public:

	// 虚拟相机的定义ID
	UPROPERTY(EditDefaultsOnly,  meta = (GetOptions = "GetVirtualCameraDefIdOptions"))
		FName VirtualCameraDefId;

private:

	UFUNCTION()
		TArray<FName> GetVirtualCameraDefIdOptions() const;
public:

	virtual void OnExec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event) override;
};