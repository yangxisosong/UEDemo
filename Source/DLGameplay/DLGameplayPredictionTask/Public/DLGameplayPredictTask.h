#pragma once

#include "CoreMinimal.h"
#include "DLGameplayPredictTaskTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/MovementComponent.h"
#include "DLGameplayPredictTask.generated.h"


class UDLGameplayPredictTaskComponent;


USTRUCT(BlueprintType)
struct FPredictTaskActorInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
		AActor* AvatarActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
		USkeletalMeshComponent* SkeletalMeshComp = nullptr;

	UPROPERTY(BlueprintReadWrite)
		UMovementComponent* MoveComp = nullptr;
};


/**
 * @brief 预测任务的基类
 *
 * 这个预测支持预测任何 Role 的 Actor 行为，有别于GAS只能预测 LocalPlayer 的行为
 *
 * 原理:
 *
 *	操作的 Actor Role
 *	ROLE_SimulatedProxy		直接执行任务，服务端确认后会调用  OnRecvVerify
 *	ROLE_AutonomousProxy	直接执行任务，服务端确认后会调用  OnRecvVerify
 *	ROLE_Authority			直接执行任务
 *
 * 注意：
 *
 *	只能用来做一些表现上的事情! 毕竟你无法修改 ROLE_SimulatedProxy 的任何数据，也不要尝试去修改 !!!
 *
 */
UCLASS(Blueprintable, Abstract)
class DLGAMEPLAYPREDICTIONTASK_API UDLGameplayPredictTask
	: public UObject
{
	GENERATED_BODY()
public:

	/**
	 * @brief 任务初始化时，子类需要复写这个方法，并且将参数转换为适合的类型，保持为成员变量
	 * @param ActorInfo
	 * @param Context
	 * @return
	 */
	virtual bool OnInitTask(TSharedPtr<FPredictTaskActorInfo> ActorInfo, const FDLGameplayPredictTaskContextBase* Context);

	/**
	 * @brief 如果任务是模拟端预测的，那么服务端确认后会回掉这个函数，用来进行一些 确认预测的操作
	 */
	virtual void OnRecvVerify() {};

	/**
	 * @brief Tick
	 * @param Dt
	 * @see bEnabledTick
	 */
	virtual void Tick(float Dt) {}

	/**
	 * @brief 是否可以被另外的一个任务打断
	 * @param Other 另外的技能
	 * @return
	 */
	UFUNCTION(BlueprintNativeEvent)
		bool CanCancelTaskByOther(UDLGameplayPredictTask* Other) const;


	/**
	 * @brief 是否可以阻止另外的一个任务
	 * @param Other 
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent)
		bool CanBlockOtherTask(UDLGameplayPredictTask* Other);

	/**
	 * @brief 当阻止其他的 Task 时调用
	 * @param Other
	 */
	virtual void OnBlockOtherTask(UDLGameplayPredictTask* Other);

public:

	/**
	 * @brief 初始化任务 [Framework Only]
	 * @param Comp
	 * @param ActorInfo
	 * @param InTaskId
	 * @param Context
	 * @return
	 */
	bool InitTask(UDLGameplayPredictTaskComponent* Comp, const TSharedPtr<FPredictTaskActorInfo> ActorInfo, const FGameplayPredictTaskId& InTaskId, const FDLGameplayPredictTaskContextBase* Context);

	/**
	* @brief 激活技能  [Framework Only]
	*/
	virtual void ActiveTask();


	/**
	 * @brief 获取当前任务的 实例ID
	 * @return
	 */
	FGameplayPredictTaskId GetInstanceId()const;

public:

	/**
	 * @brief 是否是权威方
	 * @return
	 */
	UFUNCTION(BlueprintPure)
		virtual  bool HasAuthority() const;

	/**
	 * @brief 结束这个任务
	 * @param Cancel 是否是被取消的
	 */
	UFUNCTION(BlueprintCallable)
		virtual void EndTask(const bool Cancel);

	/**
	 * @brief 获取字符串
	 * @return
	 */
	UFUNCTION(BlueprintCallable)
		virtual FString ToString() const;


	/**
	 * @brief 是否在激活状态
	 * @return 
	 */
	UFUNCTION(BlueprintPure)
		bool IsActive()const;



public:

	virtual FDLGameplayPredictTaskContextBase* GetContext();

	virtual UWorld* GetWorld() const override;
public:

	/**
	* @brief 任务被激活时，从这里开始执行任务的操作，任何权限的 Role 都会被执行
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnActiveTask();

	/**
	 * @brief 任务结束时，在这里做一些结束操作，比如清除数据，结束动画等等
	 * @param Cancel
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void OnEndTask(const bool Cancel);

protected:

	UPROPERTY(Transient)
		FGameplayPredictTaskId TaskId;

	UPROPERTY(BlueprintReadOnly, Transient)
		UDLGameplayPredictTaskComponent* PredictTaskComponent;

	UPROPERTY(Transient)
		bool bIsActive = false;
		
public:

	/**
	 * @brief 这个任务的 Tags 做一些标识
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FGameplayTagContainer TaskTags;


	/**
	 * @brief 可以被这些Tag取消
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FGameplayTagContainer CancelByTags;


	/**
	 * @brief 是否启用 Tick
	 */
	UPROPERTY(EditDefaultsOnly)
		bool bEnabledTick = false;
};

