#pragma once
#include "CoreMinimal.h"
#include "DLAbilityHitDef.h"
#include "DLGameplayAbilityTypes.h"
#include "DLUnitSelectorFitler.h"
#include "GameAbilitySysDef.h"
#include "VCDataAssetDef.h"
#include "Abilities/GameplayAbility.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Interface/IDLAbilityEventTrigger.h"
#include "Interface/IDLAbilityWeaponAttack.h"
#include "Interface/ICharacterAttack.h"
#include "Interface/IDLVirtualCameraController.h"
#include "DLGameplayAbilityBase.generated.h"

struct FDLAbilityActorInfo;
class UDataAsset_RootMotion;
class UAbilityTask_ApplyRootMotion_Base;
class IDLAbilityContextDataGetter;
class UDLGameplayTargetSelectorCoordinate;
struct FGameplayEventData;
struct FActiveGameplayEffectHandle;
class UDLAbilityEventBase;
class UDLGameplayAbilityBase;
struct FCharacterAttackArg;



USTRUCT(BlueprintType)
struct FAbilityAnticipationArg
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int32 LongPressInputId = 0;
};


DECLARE_DYNAMIC_DELEGATE(FAbilitySimpleDynamicDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FAbilityComplateDynamicDelegate, bool, IsCancel);


USTRUCT(BlueprintType)
struct FAbilityRequireGEStack
{
	GENERATED_BODY()

public:

	static constexpr int32 InvalidStackCount = -1;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<UGameplayEffect> GEClass;

	// 最大值  [MinStackCount, MaxStackCount]
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int32 MaxStackCount = InvalidStackCount;

	// 最小值  [MinStackCount, MaxStackCount]
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int32 MinStackCount = InvalidStackCount;

};

USTRUCT(BlueprintType)
struct FDLAbilityAnimationInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSoftObjectPtr<UAnimMontage> AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Rate = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FName MontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UDataAsset_RootMotion* RootMotionData;
};


// 武器的上下文信息
// 这部分数据显然是动态给武器指定的，以改变武器的一些行为
USTRUCT(BlueprintType)
struct FDLAbilityWeaponContext
{
	GENERATED_BODY()

public:

	/**
	 * @brief 单位选择过滤器原型
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
		UDLUnitSelectorFilterBase* UnitSelectorFilter;

	/**
	* @brief 覆盖武器的选中框
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
		class UDLGameplayTargetSelectorShapeTraceBase* OverrideWeaponSelector;
};



USTRUCT(BlueprintType)
struct FDLAbilityVirtualCameraDefine
	: public FDLAbilityDefineBase
{
	GENERATED_BODY()
public:

	// 虚拟相机的Tag，标记控制那个虚拟相机 
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag VirtualCameraChannelTag;

	// 虚拟相机的资产列表
	UPROPERTY(EditDefaultsOnly)
		TArray<UVCDataAssetDef*> VCDataAssetArray;

	// 当技能结束后停止镜头
	UPROPERTY(EditDefaultsOnly)
		bool bWhenAbilityEndStopVC = true;
};




USTRUCT(BlueprintType)
struct FDLSubObjectSpawnConfig
{
	GENERATED_BODY()
public:

	/**
	 * @brief 需要使用的 Hit 定义
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (GetOptions = GetHitDefineIds))
		TArray<FGameplayTag> OutgoingHitDefineIdArray;

	/**
	 * @brief 这个子物体的生存时间
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float LifeTime = 3.f;
};


USTRUCT(BlueprintType)
struct FDLSubObjectDef
	: public FDLAbilityDefineBase
{
	GENERATED_BODY()

public:

	// 子物体的 Class
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (AllowedClasses = DLGameplaySubObjectBase))
		TSoftClassPtr<AActor> SubObjectClass;

	// 创建这个子物体的配置项
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FDLSubObjectSpawnConfig SpawnConfig;
};


USTRUCT(BlueprintType)
struct FAbilityStageExecInputCmdEvaluate
{
	GENERATED_BODY()

public:

	// 阶段
	UPROPERTY(EditAnywhere)
		EDLAbilityStage Stage;

	// 允许那些命令
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer AllowInputCmdTags;

public:

	bool operator==(EDLAbilityStage InStage) const
	{
		return Stage == InStage;
	}
};


USTRUCT(BlueprintType)
struct FDLAbilityAISetting
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MinCastDistance = MAX_FLT;
};

UENUM(BlueprintType)
enum class EDLAbilityUnitSelectType : uint8
{
	Weapon,
	UnitSelectorInstance,
	SubObject,
};

/**
 * @brief 定义技能释放的目标选择 数据
 */
USTRUCT(BlueprintType)
struct FDLAbilityUnitSelectDefine
	: public FDLAbilityDefineBase
{
	GENERATED_BODY()
public:

	/**
	* @brief 造成 Hit 的类型
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EDLAbilityUnitSelectType UnitSelectType;

	/**
	* @brief 武器的上下文
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Weapon
					, meta = (EditCondition = "UnitSelectType==EDLAbilityUnitSelectType::Weapon", EditConditionHides))
		FDLAbilityWeaponContext WeaponContext;


	/**
	 * @brief 选择器实例的原型
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = SelectorIns
					, meta = (EditCondition = "UnitSelectType==EDLAbilityUnitSelectType::UnitSelectorInstance", EditConditionHides))
		class UDLUnitSelectorInstance* SelectorInsPrototype;
};


USTRUCT(BlueprintType)
struct FDLAbilityCostDefine
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
		FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere)
		FGameplayEffectModifierMagnitude ModifierMagnitude;

	UPROPERTY(EditAnywhere)
		bool EnabledOverdraft = false;
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilitySimpleEvent, FGameplayTag, EventTag);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilityHitUnit, FGameplayEventData);



UCLASS(hideCategories=(Cost))
class DLABILITYSYSTEM_API UDLGameplayAbilityBase
	: public UGameplayAbility
	, public IDLAbilityEventTrigger
	, public IDLAbilityWeaponAttack
{
	GENERATED_BODY()

public:

	UDLGameplayAbilityBase();


	void ClientTryExecInputCmd(EDLAbilityStage Stage);

	FDLAbilityHitDefine* FindAbilityHitDefine(const FGameplayTag& DefName);

	FDLAbilityUnitSelectDefine* FindUnitSelectDefine(const FGameplayTag& DefName);

	const FDLAbilityVirtualCameraDefine* GetVirtualCameraDefMap(const FGameplayTag& DefId)const;

	EPrimaryInputID GetInputId()const;

	const FGameplayTagRequirements& GetCancelAbilityByTags()const { return  ActivationCancelTag; }

	const FDLAbilityAISetting& GetAISetting() const { return AISetting; }

	EPrimaryInputID GetDebugInputID()const;

	void SetActiveInputKey(const EPrimaryInputID InputID);

	ENetRole GetOwnerNetRole()const;

	FString GetDebugActiveID()const;

	void DebugGeneratedDebugActiveID();

	const FDLAbilityActorInfo* GetAbilityActorInfo() const;

	bool AllowCostOverdraft(const FGameplayAttribute& CostAttribute) const;
public:

	const TArray<FDLAbilityUnitSelectDefine>& GetUnitSelectDefines()const;

	const TArray<FDLAbilityHitDefine>& GetHitDefines() const;

	const TArray<FDLSubObjectDef>& GetSubObjectDefines() const;

	const TArray<FDLAbilityVirtualCameraDefine>& GetVirtualCameraDefines() const;

	const TArray<UDLAbilityEventBase*>& GetEventActionArray() const;

	const FDLAbilityAnimationInfo& GetAnimationInfo()const;
public:

	UFUNCTION()
		FGameplayTagContainer GetHitDefineIds() const;

	UFUNCTION()
		FGameplayTagContainer GetUnitSelectDefineIds() const;

	UFUNCTION(BlueprintCallable, Category = Ability)
		FGameplayAbilityTargetDataHandle MakeTargetDataByActor(AActor* Actor);


	UFUNCTION(BlueprintCallable)
		void SendTargetDataToServer(
					const FGameplayAbilityTargetDataHandle& TargetDataHandle,
					const FGameplayTag CustomTag,
					const FAbilitySimpleDynamicDelegate ExecWithPrediction);


	/**
	 * @brief 攻击一个实体
	 * @param InTrigger
	 * @param TargetDataHandle 攻击的目标
	 * @param UnderAttackCalculate
	 */
	UFUNCTION(BlueprintCallable)
		void AttackActor(AActor* InTrigger, FGameplayAbilityTargetDataHandle TargetDataHandle, UCalculateAbilityAttackInfo* UnderAttackCalculate);


	UFUNCTION(BlueprintCallable)
		bool TryActiveAbility(FGameplayAbilitySpecHandle AbilityHandle, bool bAllowRemoteActivation, const FAbilityComplateDynamicDelegate& Complate);


	UFUNCTION(BlueprintPure)
		bool CanBeExecuted(bool AllowPlayerAutonomous, bool AllowAIAutonomous) const;

	// 先弃用， 后边如果确定不用的话就移除
	UFUNCTION(BlueprintCallable, Meta = (DeprecatedFunction))
		FGameplayAbilitySpecHandle GetSubAbilitySpecHandleByName(FName Name);


	UFUNCTION(BlueprintCallable)
		const FGameplayTagContainer& GetAbilityTag()const;

	UFUNCTION(BlueprintPure)
		static int32 GetGEStackCount(const FGameplayAbilityActorInfo& ActorInfo, TSubclassOf<UGameplayEffect> GEClass);

	UFUNCTION(BlueprintPure)
		static float GetAttributeValue(const FGameplayAbilityActorInfo& ActorInfo, FGameplayAttribute Attribute, bool& bSuccessfullyFoundAttribute);

	UFUNCTION(BlueprintCallable)
		static void GetTargetActorByTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TArray<AActor*>& OutActor);


	/**
	 * @brief 查找子物体的定义 通过名字
	 * @param DefName 子物体的名字
	 * @param OutSubObjectDef  返回子物体的定义
	 * @return 是否找到
	 */
	UFUNCTION(BlueprintCallable)
		bool FindSubObjectDefByName(FGameplayTag DefName, FDLSubObjectDef& OutSubObjectDef) const;

	/**
	* @brief
	* @param DefName
	* @param CoordinateSelector
	* @param SpawnCount 使用参数生成的次数，CoordinateSelector，假设选中2个点，SpawnCount = 3，那么结果生成 2 X 3 = 6 个子物体
	*/
	UFUNCTION(BlueprintCallable)
		TArray<AActor*> CreateSubObjectWithDefName(FGameplayTag DefName, UDLGameplayTargetSelectorCoordinate* CoordinateSelector, const int32 SpawnCount);


	/**
	 * @brief 查找技能的 Hit 定义
	 * @param DefName
	 * @param OutHitDefine
	 * @return
	 */
	UFUNCTION(BlueprintCallable)
		bool K2_FindAbilityHitDefine(FGameplayTag DefName, FDLAbilityHitDefine& OutHitDefine);

	/**
	 * @brief 是否是主动技能
	 * @return
	 */
	UFUNCTION(BlueprintPure)
		bool IsPrimaryAbility()const;


	UFUNCTION(BlueprintPure)
		EDLAbilityStage GetCurrentStage()const;


	UFUNCTION(BlueprintPure)
		TScriptInterface<IDLAbilityContextDataGetter> GetContextDataGetter() const;

	UFUNCTION(BlueprintCallable, Category = RootMotion)
		void ApplyRootMotion(UDataAsset_RootMotion* RootMotionData);

	UFUNCTION(BlueprintCallable, Category = RootMotion)
		void EndRootMotion();

public:

	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerEventHitUnit(const FGameplayTag& HitEventTag, const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;

	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerEventSimple(FGameplayTag EventTag, bool NeedClientPrediction = false) override;


	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerEventAbilityStageChange(EDLAbilityStage NewStage, bool NeedClientPrediction = false)override;


	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerEventActiveVC(FGameplayTag VCDefId, EVCTimeOutAction VCTimeOutAction) override;

	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerEventStopVC(FGameplayTag VCDefId) override;


	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerEventExecInputCmd() override;


	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void TriggerCustomStateEvent(bool IsBeginState, const FGameplayTag& Tag, bool NeedClientPrediction = false) override;


	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void AddWittingEndNtfState(UObject* AnimNtfState) override;

	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual void RemoveWittingEndNtfState(UObject* AnimNtfState) override;

	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		virtual bool CanTriggerEventByAnimNtf(UAnimationAsset* Anim, UAnimNotifyState* EndAnimNtfStateObj) override;

	UFUNCTION(BlueprintCallable, Category = GATriggerEvent)
		TArray<UDLAbilityEventBase*> FindAbilityEvent(TSubclassOf<UDLAbilityEventBase> EventClass) const;

public:


	virtual void BeginAttack(const FWeaponAttackNtfInfo& Parameter) override;

	virtual void EndAttack(const FWeaponAttackNtfInfo& Parameter) override;

	virtual void OnAbilityStageChange(EDLAbilityStage NewStage);

	virtual void GetSubAbilityClassArr(TArray<TSubclassOf<UDLGameplayAbilityBase>>& Out) const;

	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual UGameplayEffect* GetCostGameplayEffect() const override;



public:

	// 技能开始施法
	UFUNCTION(BlueprintImplementableEvent, Category = AbilityStage)
		void OnSpellStart();

	// 技能前摇开始
	UFUNCTION(BlueprintImplementableEvent, Category = AbilityStage)
		void OnPreStart();

	// 后摇开始
	UFUNCTION(BlueprintImplementableEvent, Category = AbilityStage)
		void OnPostStart();

	// 引导阶段开始
	UFUNCTION(BlueprintImplementableEvent, Category = AbilityStage)
		void OnChannelStart();

	// 开关类型的技能, 关闭
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityStage | Toogle")
		void OnToggleOff();

	// 开关类型的技能, 打开
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityStage | Toogle")
		void OnToggleOn();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = OnAbilityStageChange, Category = AbilityStage)
		void K2_OnAbilityStageChange(EDLAbilityStage InCurrentStage, EDLAbilityStage InNewStage);

	// 自定义事件触发
	UFUNCTION(BlueprintImplementableEvent, DisplayName = OnCustomEvent, Category = AbilityEvent)
		void OnSimpleEvent(const FGameplayTag& Tag);

	// 创建子物体
	UFUNCTION(BlueprintImplementableEvent)
		TArray<AActor*> OnCreateSubObjectWithDefName(FGameplayTag DefName, UDLGameplayTargetSelectorCoordinate* CoordinateSelector, int32 SpawnCount);

	// 自定义状态触发
	UFUNCTION(BlueprintNativeEvent, DisplayName = OnCustomStateEvent, Category = AbilityEvent)
		void OnCustomStateEvent(bool IsBeginState, const FGameplayTag& Tag);

protected:

	FDelegateHandle ActiveAbilityEndDelegateHandle;
	FDelegateHandle ActiveAbilityDelegateHandle;
	FDelegateHandle AbilitySpecialHandle;
	FGameplayTagContainer AbilitySpecialTags;

	TOptional<FWeaponAttackNtfInfo> AttackingWeaponInfo;

public:

	// 技能ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BaseInfo, meta = (AllowPrivateAccess))
		FName AbilityId;

	// 技能名字
	UPROPERTY(EditDefaultsOnly)
		FText AbilityName;

	// 技能描述
	UPROPERTY(EditDefaultsOnly)
		FText AbilityDesc;

protected:

	// Debug 按键
	UPROPERTY(EditDefaultsOnly, Category = Debug, meta = (AllowPrivateAccess))
		EPrimaryInputID DebugInputId;

	// Hit 信息的的定义
	UPROPERTY(EditDefaultsOnly, Category = AbilityDefine, meta = (AllowPrivateAccess))
		TArray<FDLAbilityHitDefine> HitDefines;

	// 目标选择相关的定义
	UPROPERTY(EditDefaultsOnly, Category = AbilityDefine, meta = (AllowPrivateAccess))
		TArray<FDLAbilityUnitSelectDefine> UnitSelectDefines;

	// 这个技能用到的子物体的定义数组
	UPROPERTY(EditDefaultsOnly, Category = AbilityDefine, meta = (AllowPrivateAccess))
		TArray<FDLSubObjectDef> SubObjectDefines;

	// 虚拟相机的定义表  Id, Def
	UPROPERTY(EditDefaultsOnly, Category = AbilityDefine)
		TArray<FDLAbilityVirtualCameraDefine> VirtualCameraDefines;

	// 消耗的定义
	//UPROPERTY(EditDefaultsOnly, Category = AbilityDefine)
	//	TArray<FDLAbilityCostDefine> CostDefines;


	// 允许透支的 Cost 属性
	UPROPERTY(EditDefaultsOnly, Category=Costs)
		TArray<FGameplayAttribute> CostAttributeOverdraft;

	// AI 的定义
	UPROPERTY(EditDefaultsOnly, Category = AI)
		FDLAbilityAISetting AISetting;

	// 这个技能动画相关的信息
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BaseInfo, meta = (AllowPrivateAccess))
		FDLAbilityAnimationInfo AnimationInfo;

	// 这个技能触发的事件以及绑定的Action
	UPROPERTY(EditDefaultsOnly, Category = BaseInfo, Instanced)
		TArray<UDLAbilityEventBase*> EventActionArray;

	// 各个阶段的输入评估，只有玩家的技能需要配置
	UPROPERTY(EditDefaultsOnly, Category = BaseInfo)
		TArray<FAbilityStageExecInputCmdEvaluate> StageInputCmdEvaluateArray;

	// 当 Owner 身上有这些 Tag 时，技能会被取消
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (AllowPrivateAccess))
		FGameplayTagRequirements ActivationCancelTag;

	UPROPERTY(EditDefaultsOnly, Category = Condition)
		FAbilityRequireGEStack RequireGEStack;

	UPROPERTY(EditDefaultsOnly, Category = Cooldowns)
		FScalableFloat CooldownDuration;

	UPROPERTY(EditDefaultsOnly, Category = Cooldowns)
		FGameplayTagContainer CooldownTags;

	/**
	* @brief 免疫 Hit GE
	*/
	UPROPERTY(EditDefaultsOnly, Category = ImmuneHit, AdvancedDisplay)
		TSubclassOf<UGameplayEffect> ImmuneHitGEClass;

	/**
	 * @brief 免疫Hit的事件Tag
	 */
	UPROPERTY(EditDefaultsOnly, Category = ImmuneHit, AdvancedDisplay)
		FGameplayTag ImmuneHitEventTag;

	UPROPERTY()
		FActiveGameplayEffectHandle ActiveImmuneHitGEHandle;

	// 动态韧性相关

	// 出手韧性系数  公式-> 出手韧性系数*基础韧性值+出手韧性值
	UPROPERTY(EditDefaultsOnly, Category = TenacityDynamic)
		float TenacityDynamicCoefficient;

	// 出手韧性值  公式-> 出手韧性系数*基础韧性值+出手韧性值
	UPROPERTY(EditDefaultsOnly, Category = TenacityDynamic)
		float TenacityDynamicBase;

	UPROPERTY(EditDefaultsOnly, Category = TenacityDynamic, AdvancedDisplay)
		TSubclassOf<UGameplayEffect> TenacityDynamicGEClass;

	UPROPERTY(EditDefaultsOnly, Category = TenacityDynamic, AdvancedDisplay)
		FGameplayTag TenacityDynamicEventTag;

	UPROPERTY(EditDefaultsOnly, Category = TenacityDynamic, AdvancedDisplay)
		FGameplayTag TenacityDynamicSetByCallerTag;

	UPROPERTY()
		FActiveGameplayEffectHandle TenacityDynamicGEHandle;

	UPROPERTY(EditDefaultsOnly, Category = CutTenacity, AdvancedDisplay)
		TSubclassOf<UGameplayEffect> CutTenacityGEClass;

protected:


#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void PostInitProperties() override;

	virtual bool Modify(bool bAlwaysMarkDirty) override;

	void CheckEditorState();
#endif


protected:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
		UAbilityTask_ApplyRootMotion_Base* RootMotionTask;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
		EDLAbilityStage CurrentStage;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
		EPrimaryInputID ActiveInputKey = EPrimaryInputID::None;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess))
		FGameplayTagContainer GeneratedWeaponHitEventTags;

	UPROPERTY()
		bool bUsedWeapon = false;


	/**
	 * @brief 正在等待 End 通知的 NtfState
	 */
	UPROPERTY()
		TArray<UObject*> WaitingEndNtfStates;


	UPROPERTY(Transient)
		UGameplayEffect* InstanceGE;


#if WITH_EDITORONLY_DATA

		UPROPERTY()
		FString ActiveID;

#endif

protected:

	// 这个 Ability 中使用的其他 子 Ability  TODO 
	//UPROPERTY(EditDefaultsOnly, Category = BaseInfo)
	TMap<FName, TSubclassOf<UDLGameplayAbilityBase>> SubAbilityMap;

	TArray<FGameplayTag> ActivatingVCDefIds;

	FAbilityStageExecInputCmdEvaluate* LastInputCmdEval = nullptr;
public:
	friend class UDLAbilityActionApplyGE;
	friend class UDLAbilityActionApplyHitDefine;
};



USTRUCT(BlueprintType)
struct FDLAnticipationApplyGEInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TSubclassOf<UGameplayEffect> EffectClass;

	// 激活的GE等级
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		int32 Level = 1;

	// GE 的层数
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		int32 StackCount = 1;

	// 技能结束时，是否需要取消 GE
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		bool NeedStopWhenAbilityEnd = true;
};


USTRUCT(BlueprintType)
struct FDLChannelTimeSpanStageInfo
{
	GENERATED_BODY()

public:

	// 时间区间的最小值
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MinTimeSpan = 0.f;

	// 从哪个动画区间开始激活施法
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (GetOptions = GetActiveStartSections))
		FName ActiveStartSection;

	// 动画所需要的程序位移
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UDataAsset_RootMotion* RootMotionData;
};

UCLASS()
class DLABILITYSYSTEM_API UDLChannelAbilityBase
	: public UDLGameplayAbilityBase
{
	GENERATED_BODY()

public:

	UDLChannelAbilityBase();

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


	virtual void OnAbilityStageChange(EDLAbilityStage NewStage) override;

private:

	// 最短的时间 （秒） 如果不满足最短时间直接会导致放不出来
	UPROPERTY(EditDefaultsOnly, Category = Channel, BlueprintReadOnly, meta = (AllowPrivateAccess))
		float MinAnticipationTime = 1.f;


	// 蓄力的不同等级对应的信息
	UPROPERTY(EditDefaultsOnly, Category = Channel, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TArray<FDLChannelTimeSpanStageInfo> ChannelTimeSpanStageInfoArray;


	// 技能激活时 激活的GE 会在蓝图的 ActivateAbility 之前被自动激活
	UPROPERTY(EditDefaultsOnly, Category = Channel, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TArray<FDLAnticipationApplyGEInfo> AutoActiveEffectInfoArray;

public:

	UFUNCTION()
		TArray<FName> GetActiveStartSections();

	UFUNCTION(BlueprintCallable)
		FDLChannelTimeSpanStageInfo FindBastChannelTimeSpanStageInfo(float CurrentTimeSpan);

private:

	void EndAutoActiveGE();


protected:

	TArray<FActiveGameplayEffectHandle> AutoEndGEHandleArray;
};

