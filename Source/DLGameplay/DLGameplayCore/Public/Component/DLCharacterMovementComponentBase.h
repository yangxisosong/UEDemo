#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DLCharacterMovementComponentBase.generated.h"

struct FCharacterMovementInfo;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMoveStateChange, bool);

DECLARE_DELEGATE_RetVal(bool, FCheckAllowMove);
DECLARE_DELEGATE_RetVal(float, FGetMaxSpeedAttribute);

struct FDLMoveStrategyInterface
{
	FCheckAllowMove CheckAllowMove;
	FGetMaxSpeedAttribute MaxSpeedAttribute;


	bool IsAllowMove(const bool Default = true)const
	{
		if (CheckAllowMove.IsBound())
		{
			return CheckAllowMove.Execute();
		}
		return Default;
	}

	float GetMaxSpeedAttribute(const float Default = 100.f) const
	{
		if (MaxSpeedAttribute.IsBound())
		{
			return MaxSpeedAttribute.Execute();
		}
		return Default;
	}

};

USTRUCT(BlueprintType)
struct FDLCharacterTurnToInfo
{
	GENERATED_BODY()
public:

	/**
	* @brief 转向是否完成
	*/
	UPROPERTY()
		bool bIsTurnToComplate = true;

	/**
	* @brief 转到目标的 Rotator
	*/
	UPROPERTY()
		FRotator TargetTurnToRotation;

public:

	UPROPERTY(NotReplicated)
		float TurnSpeedYaw = 180.f;

	UPROPERTY(NotReplicated)
		bool bImmediatelyTurn = false;
};


/**
 * @brief 游戏角色的移动组件 （Player、NPC）
 *
 *	这个类用来处理移动的通用情况
 *
 *	如果后续有一些特定情况，需要继承这个类，并且使用  UInterface 提供特定的功能接口
 *
 *	其他系统应该可以拿到这个 类型的实例
 *
 *	如果是特定情况，可以取接口进行操作，而不是Cast到特定类型
 *
 */
UCLASS()
class DLGAMEPLAYCORE_API UDLCharacterMovementComponentBase
	: public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual float GetMaxSpeed() const override;

	virtual void AddInputVector(const FVector WorldVector, const bool bForce) override;

	virtual void PhysicsRotation(float DeltaTime) override;
public:

	/**
	 * @brief 注册移动的策略，移动的策略与 Character 是松耦合的 Bind 关系
	 * @param InStrategy
	 */
	void RegisterMoveStrategy(const FDLMoveStrategyInterface& InStrategy);


	/**
	 * @brief 监听移动状态改变的代理  静止 ->  Move
	 * @param InDelegate
	 * @return
	 */
	FDelegateHandle AddMoveStateChangeDelegate(const FOnMoveStateChange::FDelegate& InDelegate);


	/**
	 * @brief 移除状态改变的代理
	 * @param Handle
	 * @see  AddMoveStateChangeDelegate
	 */
	void RemoveMoveStateChangeDelegate(const FDelegateHandle& Handle);


	/**
	 * @brief 显示Debug信息的接口
	 * @param Canvas
	 * @param DebugDisplay
	 * @param YL
	 * @param YPos
	 */
	virtual void ShowDebugInfo(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos);

	UFUNCTION(Server, Reliable)
		void ServerTurnToTargetRotation(const FRotator& InTargetRotation, bool ImmediatelyTurn,
													   float OverrideTurnSpeedYaw);

	/**
	* @brief 转到目标朝向
	* @param InTargetRotation
	* @param ImmediatelyTurn 是否立即转
	* @param OverrideTurnSpeedYaw
	*/
	void TurnToTargetRotation(const FRotator& InTargetRotation, bool ImmediatelyTurn, float OverrideTurnSpeedYaw = 0.f);

	void UpdateMoveInfo(const FCharacterMovementInfo& Info);

	const FGameplayTag& GetCurrentMoveInfoTag() const { return  CurrentMoveInfoTag; }

public:

	/**
	* @brief 获取可变的 TurnInfo， 仅仅在需要改变值是使用这个API，不要乱用，否则会导致网络同步问题
	* @return
	*/
	FDLCharacterTurnToInfo& GetTurnToInfo_Mutable();

	/**
	* @brief 获取只读的 TurnInfo
	* @return
	*/
	FDLCharacterTurnToInfo GetTurnToInfo() const;


private:

	/**
	* @brief 网络同步的 TurnToInfo
	* @note 主控端使用的 是  AutonomousTurnInfo
	* @see GetTurnToInfo_Mutable   GetTurnToInfo    不要直接操作这两个值
	*/
	UPROPERTY(ReplicatedUsing = OnRep_NetRepTurnToInfo)
		FDLCharacterTurnToInfo NetRepTurnToInfo;

	UFUNCTION()
		void OnRep_NetRepTurnToInfo() const;

	/**
	* @brief 主控端本地的 Turn Info
	*/
	UPROPERTY()
		FDLCharacterTurnToInfo AutonomousTurnInfo;

private:

	int32 NoMoveCount = 0;

	bool bIsMoving = false;

	FOnMoveStateChange OnMoveStateChange;

	FDLMoveStrategyInterface StrategyInterface;

	int32 ViewportDisplayHandle = -1;

	FGameplayTag CurrentMoveInfoTag;
};
