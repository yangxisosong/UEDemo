#pragma once


#include "CoreMinimal.h"
#include "DLGameplayPredictTask.h"
#include "DLGameplayPredictTaskTypes.h"
#include "GameplayTagContainer.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionJumpForce.h"
#include "Animation/DLAnimBaseDef.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnderAttackPredictTask.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogDLUnderAttack, Log, All);

USTRUCT(BlueprintType)
struct FDLUnderAttackTaskContext : public FDLGameplayPredictTaskContextBase
{
	GENERATED_BODY()

public:

	PREDICT_TASK_ID_DEF(FDLUnderAttackTaskContext)

public:

	// 教唆者
	UPROPERTY(BlueprintReadWrite)
		AActor* EffectCauser = nullptr;

	// 被攻击的相关Tag 、Power、HitPartTag 等等
	UPROPERTY(BlueprintReadWrite)
		FGameplayTagContainer  AttackTags;

	// Hit 世界朝向
	UPROPERTY(BlueprintReadWrite)
		FVector HitForceWorldRotation;

	// 开始任务的时间点 Client
	UPROPERTY(BlueprintReadWrite)
		int64 ClientStartTaskTicks = 0;

	// HitRet
	UPROPERTY(BlueprintReadWrite)
		FHitResult HitRet;

public:

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
};


USTRUCT(BlueprintType)
struct FUnderAttackTaskActorInfo : public FPredictTaskActorInfo
{
	GENERATED_BODY()
public:


};

UENUM()
enum EUnderAttackTaskPriority
{
	None,

	// 轻 受击
	NormalUnderAttack,

	// 中 标准受击
	MiddleUnderAttack,

	// 重 标准受击
	BashUnderAttack,

	// 中 击退
	MiddleKnockBack,

	// 重 击退
	BashKnockBack,

	// 特殊
	Special,

	// 击飞
	Diaup,

	// 击倒
	KnockDown,

	Max,
};


USTRUCT(BlueprintType)
struct FDLUnderAttackInputCmdDefine
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FName DefineID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Gameplay.PlayerInputCmd.Category"))
		FGameplayTagContainer AllowInputCmdTags;


public:

	bool operator==(const FName InDefineID)const
	{
		return DefineID == InDefineID;
	}
};

UCLASS(Config = Game, MinimalAPI)
class UDLUnderAttackTask : public UDLGameplayPredictTask
{
	GENERATED_BODY()

public:

	virtual bool CanCancelTaskByOther_Implementation(UDLGameplayPredictTask* Other) const override;

	virtual bool OnInitTask(const TSharedPtr<FPredictTaskActorInfo> InActorInfo, const FDLGameplayPredictTaskContextBase* InContext) override;

	virtual void OnRecvVerify() override
	{
		const auto CurrentTicks = FDateTime::Now().GetTicks();
		const int64 RttMs = (CurrentTicks - Context.ClientStartTaskTicks) / ETimespan::TicksPerMillisecond;
		const float RttS = RttMs / 1000.f;
		this->OnVerifyTask(RttS);
	}

	virtual bool HasAuthority() const override
	{
		if (IsValid(ActorInfo.AvatarActor))
		{
			return ActorInfo.AvatarActor->GetLocalRole() == ENetRole::ROLE_Authority;
		}

		return false;
	}

	virtual void OnBlockOtherTask(UDLGameplayPredictTask* Other) override;

	virtual void ActiveTask() override;
	virtual void EndTask(const bool Cancel) override;

	virtual FDLGameplayPredictTaskContextBase* GetContext() override;

public:

	/**
	 * @brief 是否需要运行 RootMotion 来模拟移动
	 *			在使用 RootMotion 时，如果这个为 True, 那么一定需要运行 RootMotion，否则就会抖!
	 * @return
	 */
	UFUNCTION(BlueprintPure)
		bool NeedRunRootMotionSrc() const;


	/**
	 * @brief 如果这个任务是 ROLE_SimulatedProxy 预测的，
	 *			那么当服务端收到这个任务并且确认后会在 ROLE_SimulatedProxy 的任务调用这个函数
	 * @param Rtt 客户端与服务端往返通信的时间
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void OnVerifyTask(float Rtt);


	/**
	 * @brief 当阻止其他的 UnderAttack 时
	 * @param Task
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = OnBlockOtherTask)
		void K2_OnBlockOtherTask(UDLUnderAttackTask* Task);

	/**
	 * @brief 获取当前 Montage 指定 Section 的长度
	 * @param SectionName
	 * @param Suc
	 * @return
	 */
	UFUNCTION(BlueprintCallable, Category = Montage)
		float GetCurrentMontageSectionLength(const FName SectionName, bool& Suc) const;


	/**
	 * @brief 设置当前 Montage 的播放速率
	 * @param Rate
	 */
	UFUNCTION(BlueprintCallable, Category = Montage)
		void SetCurrentMontagePlayRate(const float Rate);


	/**
	 * @brief 获取当前的 Montage
	 * @return
	 */
	UFUNCTION(BlueprintPure, Category = Montage)
		UAnimMontage* GetCurrentMontageChecked()const;


	/**
	 * @brief 设置当前的 Montage
	 * @param Montage
	 */
	UFUNCTION(BlueprintCallable, Category = Montage)
		void SetCurrentMontage(UAnimMontage* Montage);


	/**
	 * @brief 给这个任务的所属角色增加 Tag
	 * @note 服务端、客户端 都应该用逻辑调用这个方法
	 * @param Tags
	 */
	UFUNCTION(BlueprintCallable, Category = GameplayTag)
		void AddGameplayTagsToOwner(FGameplayTagContainer Tags);

	/**
	 * @brief
	 * @param Tags
	 */
	UFUNCTION(BlueprintCallable, Category = GameplayTag)
		void RemoveGameplayTagsToOwner(FGameplayTagContainer Tags);

		UFUNCTION(BlueprintCallable)
	bool IsLocalPlayerController() const;

protected:

	UFUNCTION(BlueprintCallable, Category = InputCmd)
		void SwitchAllowInputCmdDef(FName DefId);

	UFUNCTION(BlueprintCallable, Category = InputCmd)
		void ClearAllowInputCmdDef();
		

public:

	/** TODO  后续会被新的 Debug 系统替换
	 * @brief   "临时"    画一个 Debug 的线，
	 * @param LineStart
	 * @param LineEnd
	 * @param LineColor
	 * @param Duration
	 * @param Thickness
	 */
	UFUNCTION(BlueprintCallable, Category = Debug, meta = (DevelopmentOnly))
		void DrawDebugLine(const FVector LineStart, const FVector LineEnd, const FLinearColor LineColor, const float Duration = 0.f, const float Thickness = 0.f) const;


protected:

	UPROPERTY(Transient)
		FGameplayTagContainer AddTags;

	UPROPERTY(BlueprintReadOnly, Transient)
		FDLUnderAttackTaskContext Context;

	UPROPERTY(BlueprintReadOnly, Transient)
		FUnderAttackTaskActorInfo ActorInfo;

	UPROPERTY(Transient)
		UAnimInstance* AnimInstance;

	UPROPERTY(Transient)
		UAnimMontage* CurrentMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config | Montages")
		FDLAnimMontageAsset UnderHitMontage_Forward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config | Montages")
		FDLAnimMontageAsset UnderHitMontage_Backward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config | Montages")
		FDLAnimMontageAsset UnderHitMontage_Left;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config | Montages")
		 FDLAnimMontageAsset UnderHitMontage_Right;

private:

	FName CurrentApplyAllowInputDefId;

protected:


	/**
	 * @brief 这个任务的优先级
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TEnumAsByte<EUnderAttackTaskPriority> TaskPriority;

	/**
	 * @brief 定义允许的输入命令
	 */
	UPROPERTY(EditDefaultsOnly)
		TArray<FDLUnderAttackInputCmdDefine> AllowInputCmdDefine;


	/**
	 * @brief 如果 外部条件允许 （比如没有 霸体 BUFF 等情况），执行任务是 首先打断当前的 技能
	 */
	UPROPERTY(EditDefaultsOnly)
		bool bIsConditionInterruptAbility = true;
};


UENUM(BlueprintType)
enum class EUnderAttackDirection : uint8
{
	None,
	Forward,
	Backward,
	Left,
	Right
};


UCLASS(Config = DLGameplayCore)
class UUnderAttackLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * @brief
	 * @param DirTag 方向的Tag
	 * @return 返回相对于角色正前方的旋转值
	 */
	UFUNCTION(BlueprintCallable)
		static FRotator CalUnderAttackRotationByDir(const FGameplayTag DirTag)
	{
		const float* ValuePtr = GetDefault()->CalUnderAttackRotationByDirConfig.Find(DirTag);
		const float Add = ValuePtr ? *ValuePtr : 0.f;
		FRotator Ret = FRotator::ZeroRotator;
		Ret.Yaw = Add;
		return Ret;
	}

	static UUnderAttackLib* GetDefault()
	{
		return StaticClass()->GetDefaultObject<UUnderAttackLib>();
	}

public:

	UPROPERTY(EditAnywhere, Config)
		TMap<FGameplayTag, float> CalUnderAttackRotationByDirConfig;
};
