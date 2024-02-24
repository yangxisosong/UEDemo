#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "CharacterTypesDef.h"
#include "DataRegistrySubsystem.h"
#include "DLGameplayPredictTaskComponent.h"
#include "GameFramework/Character.h"
#include "Interface/ICharacterAttack.h"
#include "Interface/ICharacterStateAccessor.h"
#include "Interface/ICharacterStateListener.h"
#include "Interface/ICharacterWeapon.h"
#include "Interface/IDLLockableUnit.h"
#include "Interface/INotifyCharacter.h"
#include "Interface/IUnitSceneInfoInterface.h"
#include "VisualLogger/VisualLoggerDebugSnapshotInterface.h"
#include "DLCharacterBase.generated.h"

struct FCharacterMovementInfo;
struct FDTCharacterInfoBaseRow;
struct FPlayerInitData;
class UDLCharacterAsset;
class UDLGameplayPredictTaskComponent;
class ADLPlayerStateBase;
struct FInputActionValue;
class IDLWeapon;
class UDLCharacterMovementComponentBase;
class IPlayerAttributeAccessor;
class UHeroAbilityBase;
class UDLUnitAbilitySystemComponent;
class UInputComponent;
class UGameplayAbility;
class UGameplayEffect;

struct FInputActionValue;


USTRUCT()
struct FCharacterLockInfo
{
	GENERATED_BODY()
public:

	UPROPERTY()
		AActor* Target;

	UPROPERTY()
		int8 LockPointIndex = INDEX_NONE;

public:

	bool IsValid() const
	{
		return Target && LockPointIndex != INDEX_NONE;
	}
};


USTRUCT(BlueprintType)
struct FDLUnitUnderAttackInfo
{
	GENERATED_BODY()
public:

	// 伤害造成者
	UPROPERTY(BlueprintReadWrite)
		AActor* EffectCauser = nullptr;

	// 标识被打的这个行为
	UPROPERTY(BlueprintReadWrite)
		FGameplayTagContainer  AttackTags;
};





DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterStateChange, FGameplayTag, InTag, int32, TagCount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterUnderAttack, const FDLUnitUnderAttackInfo&, AttackInfo);

UCLASS(Config = Game)
class DLGAMEPLAYCORE_API ADLCharacterBase
	: public ACharacter
	, public INotifyCharacterWeapon
	, public IAbilitySystemInterface
	, public ICharacterWeapon
	, public ICharacterAttack
	, public ICharacterStateAccessor
	, public IGameplayTagAssetInterface
	, public INotifyCharacterFrameFrozen
	, public IVisualLoggerDebugSnapshotInterface
	, public IDLLockableUnit
	, public INotifyCharacterHideMesh
	, public INotifyCharacterChangeCollision
{
	GENERATED_BODY()

public:

#pragma region Public

	explicit ADLCharacterBase(const class FObjectInitializer& ObjectInitializer);

	/**
	 * @brief 始化角色数据
	 * @param InBaseInfo
	 * @param Asset 角色资产数据
	 */
	void InitCharacter(const FCharacterInfoBase& InBaseInfo, const UDLCharacterAsset* Asset);

	/**
	 * @brief 在服务端设置锁定的目标
	 * @param Unit 锁定的目标
	 */
	UFUNCTION(Server, Reliable)
		void ServerSetTargetLock(AActor* Unit, int32 LockPointIndex);


	/**
	 * @brief 获取角色的ID
	 * @return
	 */
	FName GetCharacterIdChecked()const;


	/**
	 * @brief 读取玩家的配置数据
	 * @tparam DTRowType 数据表的结构类型
	 * @param Id DR ID
	 * @return
	 * @see DRCharacter::CreatePlayerDataRegistryId  DRCharacter::CreateNPCDataRegistryId
	 */
	template<typename DTRowType>
	static const DTRowType* GetCharacterDataTable(const FDataRegistryId& Id)
	{
		const auto DataRegistry = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>();
		return DataRegistry->GetCachedItem<DTRowType>(Id);
	}


	/**
	 * @brief 遍历当前激活的武器数据
	 * @param MatchWeaponTags 与武器匹配的Tag
	 * @param Visitor 访问器
	 */
	void ForEachActiveWeapon(const FGameplayTagContainer& MatchWeaponTags, const TFunction<void(TScriptInterface<IDLWeapon> Weapon)>& Visitor);


	/**
	 * @brief 当前是否锁定目标
	 * @return
	 */
	bool IsLockTargetUnit() const;


	/**
	 * @brief 获取锁定的目标
	 * @return
	 */
	AActor* GetLockTargetUnit() const;

	/**
	 * @brief 获取锁定点
	 * @return
	 */
	UDLLockPointComponent* GetLockPoint() const;

	/**
	 * @brief 获取 PlayState
	 * @return
	 */
	ADLPlayerStateBase* GetPlayerStateBase() const;


	/**
	 * @brief 转到目标朝向
	 * @param InTargetRotation
	 * @param ImmediatelyTurn 是否立即转
	 * @param OverrideTurnSpeedYaw
	 */
	void TurnToTargetRotation(const FRotator& InTargetRotation, bool ImmediatelyTurn, float OverrideTurnSpeedYaw = 0.f);

	/**
	 * @brief 获取需要转到的朝向
	 * @return
	 */
	FRotator GetTurnToTargetRotation()const;


	/**
	 * @brief 获取身上的状态
	 * @return
	 */
	FGameplayTagContainer GetCurrentCharacterState() const;


	void ServerSetCharacterDied();

	UDLCharacterMovementComponentBase* GetMovementComp() const;


#pragma endregion

#pragma region IAbilitySystemInterface

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

#pragma endregion


protected:

#pragma region IVisualLoggerDebugSnapshotInterface
	virtual void GrabDebugSnapshot(FVisualLogEntry* Snapshot) const override;
#pragma endregion

#pragma region IGameplayTagAssetInterface

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

#pragma endregion

#pragma region INotifyCharacterWeapon

	virtual void BeginAttack_Implementation(const FWeaponAttackParameter& Parameter)override;

	virtual void EndAttack_Implementation(const FWeaponAttackParameter& Parameter) override;

#pragma endregion

#pragma region INotifyCharacterFrameFrozen

	virtual void FrameFrozen(FCharacterFrameFrozenArg Arg) override;
	virtual void ForwardTargetCharacterFrameFrozen(AActor* Target, FCharacterFrameFrozenArg Arg) override;

#pragma endregion

#pragma region ICharacterWeaponAccessor

	virtual TArray<AActor*> GetCurrentWeapons() override;
	virtual AActor* GetWeaponWithAttachSocketTag(FGameplayTag SocketTag) override;

#pragma endregion

#pragma region IDLLockableUnit

	virtual UDLLockPointComponent* GetLockPointComponent(int32 Index) override;
	virtual int32 GetDefaultLockPointComponent() override;
	virtual const TArray<UDLLockPointComponent*>& GetAllLockPoint() const override;
	virtual const FLockableUnitConfig& GetLockableUnitConfig() const override;

#pragma endregion

#pragma region ICharacterAttack

	UFUNCTION(BlueprintCallable)
		virtual void AttackActors(const TArray<FCharacterAttackArg>& AttackArgArr) override;

#pragma endregion

#pragma region INotifyCharacterHideMesh

	virtual void SetMeshVisibility(const FString& Reason, bool bPropagateToChildren, bool bVisible) override;

#pragma endregion

#pragma region AActor

	virtual void Tick(float DeltaSeconds) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

	virtual void BeginPlay() override;

	virtual void OnRep_PlayerState() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma endregion

#pragma region INotifyCharacterChangeCollision
	virtual void K2_ChangeCollisionPreset(const FName& CollisionPreset) override;
	virtual void K2_ChangeCollisionResponseToChannel(ECollisionChannel Channel, ECollisionResponse NewResponse) override;
	virtual void K2_ChangeCollisionResponseToAllChannels(ECollisionResponse NewResponse) override;
#pragma endregion

#pragma region UObject


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

#pragma endregion

#pragma region ADLCharacterBase

	/**
	 * @brief 初始化这个角色信息
	 *
	 *	当角色的基础信息初始化完成，这个时候才是这个角色 GamePlay 逻辑能够真正开始的起点
	 *	因为没有基础数据，没人能知道这个角色的信息是什么
	 *
	 * @param InBaseInfo
	 */
	virtual void OnInitCharacter(const FCharacterInfoBase& InBaseInfo);


	virtual void OnUninitCharacter();


	/**
	 * @brief 当锁定状态更新时调用
	 */
	virtual void UpdateLockTargetState();


	/**
	 * @brief 检测是否可以移动
	 * @return
	 */
	virtual bool OnCheckAllowMove();

	/**
	* @brief 获取当前的允许的最大速度
	* @return
	*/
	virtual float OnGetMaxSpeed();

	/**
	 * @brief 计算最大速度
	 * @param BaseSpeed
	 * @param Scale
	 * @return
	 * @see OnGetMaxSpeed
	 */
	virtual float CalculateMaxSpeed(const float BaseSpeed, const float Scale) { return BaseSpeed * Scale; };

	/**
	 * @brief 获取骨骼插槽的名字 用Tag
	 * @param BoneSocketTag
	 * @return
	 */
	virtual FName GetBoneSocketNameByTag(const FGameplayTag& BoneSocketTag);

	/**
	 * @brief 尝试进行转向
	 * @param Dt
	 */
	virtual void TryTurnToTargetRotation(float Dt);


	/**
	 * @brief 当数据准备妥当
	 * @param PS
	 * @param Data
	 */
	virtual void OnPlayerStateInitDataReady(ADLPlayerStateBase* PS, const FPlayerInitData& Data);


	/**
	 * @brief 玩家的状态Tag改变时
	 * @param AllState
	 * @param Tag
	 * @param IsAdd
	 */
	virtual void OnPlayerStateChange(const FGameplayTagContainer& AllState, const FGameplayTag& Tag, bool IsAdd);


	virtual void OnUpdateMoveInfo(const FCharacterMovementInfo& Info);


	/**
	 * @brief 获取基础的配置数据
	 * @return 
	 */
	virtual const FDTCharacterInfoBaseRow* GetBaseDataTableInfo(bool EnsureNotData = true) { return nullptr; }

#pragma endregion

#pragma region ACharacter

	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

#pragma endregion

public:

#pragma region ICharacterStateAccessor

	virtual float GetCurrentAttributeValue_Implementation(const FGameplayAttribute& Attribute) const override;
	virtual FGameplayTagContainer k2_GetCurrentCharacterState_Implementation() const override;
	virtual bool K2_IsTurnToComplate_Implementation()const override;
	virtual bool K2_IsLockTargetUnit_Implementation() const override;
	virtual AActor* K2_GetLockTargetUnit_Implementation() const override;
	virtual void K2_GetTurnToTargetRotation_Implementation(FRotator& OutTargetRotation) const override;
	virtual UDLLockPointComponent* K2_GetLockPoint_Implementation() const override;
	virtual bool K2_IsDied_Implementation() const override;
#pragma endregion

public:

	UFUNCTION(BlueprintCallable)
		void MyTestRunTask(TSubclassOf<UDLGameplayPredictTask> Task);

	UFUNCTION(Server, Reliable)
		void ServerMyTestRunTask(const FDLNetPredictTaskContext& Arg);
public:

	/**
	* @brief 给自己身上添加 Tags  (没有进行网络同步)
	* @param InTags
	*/
	UFUNCTION(BlueprintCallable)
		void AddOwnerTags(FGameplayTagContainer InTags);

	/**
	* @brief 给自己身上移除 Tags  (没有进行网络同步)
	* @param InTags
	*/
	UFUNCTION(BlueprintCallable)
		void RemoveOwnerTags(FGameplayTagContainer InTags);

protected:

	UFUNCTION(Server, Reliable)
		void ServerForwardTargetCharacterFrameFrozen(AActor* Target, const FCharacterFrameFrozenArg& Arg);

	/**
	 * @brief 请求服务端 定帧
	 * @param Arg
	 */
	UFUNCTION(Server, Reliable)
		void ServerFrameFrozen(FDLNetPredictTaskContext Arg);

	/**
	 * @brief 请求服务端转向
	 * @param InTargetRotation
	 * @param ImmediatelyTurn
	 * @param OverrideTurnSpeedYaw
	 */
	UFUNCTION(Server, Unreliable)
		void ServerTurnToTargetRotation(const FRotator& InTargetRotation, bool ImmediatelyTurn, float OverrideTurnSpeedYaw);

	/**
	 * @brief 请求服务端 击中其他的 Actor
	 * @param Arg
	 */
	UFUNCTION(Server, Reliable)
		void ServerAttackActors(const TArray<FDLNetPredictTaskContext>& Arg);

	/**
	 * @brief Debug 是需要的一些逻辑可以放在这个Tick中
	 * @param DeltaSeconds
	 * @see  bEnabledDebugTick
	 */
	UFUNCTION(BlueprintNativeEvent)
		void OnDebugTick(float DeltaSeconds);

	/**
	 * @brief Rep 锁定的目标到客户端
	 */
	UFUNCTION()
		void OnRep_LockTargetUnit();

	/**
	 * @brief 是否显示Debug信息变化
	 * @param Show
	 * @see bShowDebug
	 */
	UFUNCTION(BlueprintNativeEvent)
		void OnDebugShowChange(bool Show);

	/**
	 * @brief 播放翻滚动画
	 * @param Direction
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void PlayRollAnimation(FGameplayTag Direction);

	/**
	 * @brief 打断翻滚动画
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void InterruptRollAnimation();

private:

	/**
	* @brief 派发攻击到 目标 Actor
	* @param Arg
	*/
	void DispatchUnderAttack(const TArray<FDLNetPredictTaskContext>& Arg) const;


	/**
	* @brief 自己 受到攻击
	* @param Arg
	*/
	void OnUnderAttack(const FDLNetPredictTaskContext& Arg) const;

	UClass* FindUnderAttackEffectTask(const FGameplayTagContainer& AttackTags) const;

	UClass* GetCharacterLoopholesTask() const;

	void InitHeadBar(TScriptInterface<ICharacterStateAccessor> AttributeAccessor, TScriptInterface<ICharacterStateListener> Listener);

	void EquipWeapons();

	UDLCharacterMovementComponentBase* GetMoveComponent() const;

	void TurnToCharacterRotation(float DtTime);



private:

	// 当前被激活的武器
	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TArray<AActor*> CurrentActiveWeaponArray;

	/**
	 * @brief 角色头顶的UI
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		class UWidgetComponent* WidgetComponent;

	/**
	 * @brief 这个角色的 Tags
	 */
	UPROPERTY(BlueprintReadOnly, Category = BaseInfo, Meta = (AllowPrivateAccess))
		FGameplayTagContainer CharacterTags;


	UPROPERTY()
		FGameplayTagContainer PrivateCharacterTags;

	/**
	 * @brief 角色的ID， 可用来关联配置表等
	 */
	UPROPERTY(BlueprintReadOnly, Category = BaseInfo, Meta = (AllowPrivateAccess))
		FName CharacterId;


private:

	/**
	 * @brief 是否开启 DebugTick
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess))
		bool bEnabledDebugTick = true;

	// 是否显示一些调试信息 图形
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess))
		bool bShowDebug = true;

#if WITH_EDITORONLY_DATA

	UPROPERTY(EditAnywhere, Category = Debug, meta = (AllowPrivateAccess))
		UArrowComponent* ForwardArrow;
#endif

public:

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
		FOnCharacterUnderAttack OnUnderAttackEvent;
protected:

	/**
	 * @brief 锁定的目标
	 */
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_LockTargetUnit)
		FCharacterLockInfo LockTargetUnit;

	UPROPERTY(Replicated)
		bool bIsDied = false;

	UPROPERTY(BlueprintReadOnly)
		UDLLockPointComponent* LockPoint = nullptr;

	/**
	 * @brief 监听角色的状态变化
	 */
	UPROPERTY(BlueprintAssignable, EditAnywhere)
		FOnCharacterStateChange OnCharacterStateChange;

	UPROPERTY()
		UDLGameplayPredictTaskComponent* PredictTaskComponent;

	UPROPERTY()
		const UDLCharacterAsset* MyAsset;

	UPROPERTY()
		TArray<UDLLockPointComponent*> LockPoints;

	UPROPERTY(EditDefaultsOnly)
		uint8 DefaultLockPoint = 0;


protected:

	bool bIsInit = false;


	FDelegateHandle TagChangeListenerHandle;

	FDelegateHandle AttrChangeListenerHandle;
};

