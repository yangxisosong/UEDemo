#pragma once
#include "CoreMinimal.h"
#include "DLAbilitySubObjectLib.h"
#include "DLGameplayAbilityBase.h"
#include "GameplayEffectTypes.h"
#include "GameplayTaskOwnerInterface.h"
#include "DLGameplaySubObjectBase.generated.h"


class UCalculateAbilityAttackInfo;
class UDLUnitAbilitySystemComponent;
class UDLGameplayTargetSelectorBase;




UCLASS(CollapseCategories, EditInlineNew, Blueprintable)
class UDLSubObjectHitUnitAction
	: public UObject
{
	GENERATED_BODY()
public:

	// 是否启用
	UPROPERTY(EditAnywhere)
		bool IsEnabled = true;

public:

	void Trigger(class ADLGameplaySubObjectBase* Object, const FHitResult& HitResult);

protected:

	UFUNCTION(BlueprintNativeEvent)
		void OnTrigger(class ADLGameplaySubObjectBase* Object, const FHitResult& HitResult);
};


UCLASS(CollapseCategories, EditInlineNew, Blueprintable)
class UDLSubObjectNormalAction
	: public UObject
{
	GENERATED_BODY()
public:

	// 是否启用
	UPROPERTY(EditAnywhere)
		bool IsEnabled = true;

	void Trigger(class ADLGameplaySubObjectBase* Object);

protected:

	UFUNCTION(BlueprintNativeEvent)
		void OnTrigger(class ADLGameplaySubObjectBase* Object);
};


UENUM(BlueprintType)
enum class EDLSubObjectDestroyReason : uint8
{
	// 生成时间到了
	LifeTimeout,

	// 达到最大的击中数
	ExceededMaxHitCount,
};



UCLASS()
class ADLGameplaySubObjectBase
	: public AActor
	, public IGameplayTaskOwnerInterface
	, public IDLSubObjectSetup
{
private:
	GENERATED_BODY()

public:

	ADLGameplaySubObjectBase();

protected:


	UPROPERTY()
		TArray<FDLSubObjectOutgoingHitData> OutgoingHitDataArray;

	//最长生命期，0.f 为无限制, 这个在值 具体由 使用方确定
	UPROPERTY()
		float MaxLifeTime = 3.f;

	UPROPERTY()
		TWeakObjectPtr<UGameplayAbility> InstanceAbility;

protected:

	/**
	 * 可以碰撞的Actor数量
	 * 是只能碰撞一个Actor，还是可以碰撞多个 ， 可以用来实现一些穿刺效果
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SubObjectHit)
		int32 MaxOverlapActorCount = 1;

	/**
	 * @brief 达到最大的 Hit 数销毁子物体
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SubObjectHit)
		bool bExceededMaxHitCountDestroy = true;

	/*
	 * 子物体的所属者会不会被该子物体碰撞
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SubObjectHit)
		bool bOwnerActorCanOverlap = false;


	// 子物体的Tag，需要从 所属者继承一部分过来
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BaseInfo)
		FGameplayTagContainer SubObjTags;

	// 子物体 存活时间到了时的 Action 列表
	UPROPERTY(Instanced, EditDefaultsOnly, Category = ActionList)
		TArray<UDLSubObjectNormalAction*>  TimeOutActions;

	// 子物体生成时的 Action 列表
	UPROPERTY(Instanced, EditDefaultsOnly, Category = ActionList)
		TArray<UDLSubObjectNormalAction*>  GenerateActions;

	// 子物体销毁时的 Action 列表
	UPROPERTY(Instanced, EditDefaultsOnly, Category = ActionList)
		TArray<UDLSubObjectNormalAction*>  DestroyActions;

public:

	/**
	 * @brief 子物体生成
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = Action)
		void OnGenerateAction();

	/**
	 * @brief 子物体销毁
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = Action)
		void OnDestroyAction(EDLSubObjectDestroyReason Reason);

	/**
	 * @brief 子物体存活时间到了
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = Action)
		void OnLifeTimeoutAction();

	/**
	* @brief 击中单位
	* @param HitResult
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = Action)
		void OnHitUnitAction(const FHitResult& HitResult);

	/**
	 * @brief 当子物体的击中数超过上限 时
	 * @param
	 * @see  ExceededMaxHitCountDestroy
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void OnExceededMaxHitCount();

public:

#pragma region IDLSubObjectSetup
	virtual bool SetupSubObject(const FDLSubObjectSetupData& Data) override;
#pragma endregion

public:

	virtual void BeginPlay() override final;

	virtual void Tick(float DeltaSeconds) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override;

	virtual AActor* GetGameplayTaskOwner(const UGameplayTask* Task) const override;

	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;

private:

	void ActionGenerate();

	void ActionTimeOut();

	void ActionDestroy();

	void OnTimeOut();

	FHitResult GetCollisionResult(UPrimitiveComponent* OverlappedComponent,const FHitResult& HitResult) const;

public:

	UFUNCTION(BlueprintCallable)
		TArray<FActiveGameplayEffectHandle> ApplyGameplayEffectWithHitResult(const FHitResult& HitResult, FGameplayTag OutgoingHitDataDefName);


	UFUNCTION(BlueprintCallable)
		bool IsExceededMaxOverlapActorCount()const;

	UFUNCTION(BlueprintCallable)
		void MarkDestroy();


	/**
	 * @brief 更新 Hit 单位
	 * @param HitUnit
	 */
	bool UpdateHitCount(const FHitResult& HitUnit);


	/**
	 * @brief 子物体是否开始销毁
	 * @return
	 */
	UFUNCTION(BlueprintPure)
		bool IsBeginDestroy() const;


private:

	UFUNCTION(BlueprintCallable)
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:

	struct FSubObjectTempData
	{
		TSet<TWeakObjectPtr<AActor>> TempOverlapActor;
		int32 CurrentOverlapActorCount = 0;
		FTimerHandle LifeTimeHandle;
		bool DeferDestroy = false;
	};

	// Runtime 数据  考虑到以后可能要做对象池
	FSubObjectTempData TempData;

	UPROPERTY()
		UGameplayTasksComponent* GameplayTasksComponent;

	UPROPERTY()
		TArray<UGameplayTask*> ActiveTaskArray;
};


