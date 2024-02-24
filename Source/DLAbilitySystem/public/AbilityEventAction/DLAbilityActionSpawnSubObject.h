#pragma once

#include "CoreMinimal.h"
#include "DLAbilityAction.h"
#include "DLGameplayAbilityBase.h"
#include "UObject/Object.h"
#include "DLAbilityActionSpawnSubObject.generated.h"

class UDLGameplayTargetSelectorCoordinate;


UCLASS(DisplayName = "SpawnSubObject(ServerOnly)")
class UDLAbilityActionSpawnSubObject
	: public UDLAbilityAction
{
	GENERATED_BODY()
public:

	/**
	 * @brief 选择子物体生成的坐标信息 （位置、朝向）
	 */
	UPROPERTY(EditAnywhere)
		UDLGameplayTargetSelectorCoordinate* SpawnCoordinateSelector; 

	/**
	 * @brief 子物体的定义的名字
	 * @see UDLGameplayAbilityBase::SubObjectDefArray
	 */
	UPROPERTY(EditAnywhere,  meta = (GetOptions = "GetSubObjectDefNameOptions"))
		FName SubObjectDefId;

	/**
	 * @brief 生成的次数，CoordinateSelector，假设选中2个点，SpawnCount = 3，那么结果生成 2 X 3 = 6 个子物体
	 */
	UPROPERTY(EditAnywhere)
		int32 SpawnCount = 1;

private:

	UFUNCTION()
		TArray<FName> GetSubObjectDefNameOptions() const;

public:

	virtual void OnExec(const FDLAbilityActionContext& Context, UDLAbilityEventBase* Event) override;
};
