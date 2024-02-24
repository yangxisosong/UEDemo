// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLClientRuntimeStageDef.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "DLClientSubsysManager.generated.h"

struct FGameplayMessageListenerHandle;
class UDLClientSubsystemBase;
class UDLClientSubsysManager;
enum class EDLClientRuntimeStage : uint8;



USTRUCT()
struct FClientSubsystemCollection
{
	GENERATED_BODY()
public:

	/**
	 * @brief 初始化收集器
	 * @param InManager
	 * @param BPSystemPaths
	 */
	void Init(UDLClientSubsysManager* InManager, const TArray<FDirectoryPath>& BPSystemPaths);


	/**
	 * @brief 尝试创建系统实例
	 * @param Stage
	 */
	void TryCreateSubsystemIns(EDLClientRuntimeStage Stage);


	/**
	 * @brief 销毁系统
	 * @param Stage 
	 */
	void TryDestroySubsystemIns(EDLClientRuntimeStage Stage);

	/**
	 * @brief 初始化依赖的系统，用来处理相同创建条件下系统之间的依赖问题
	 * @param SubsystemClass
	 * @note 依赖的系统的创建条件必须满足才可以创建
	 * @return
	 */
	UDLClientSubsystemBase* InitializeDependency(TSubclassOf<UDLClientSubsystemBase> SubsystemClass);


	/**
	 * @brief 初始化依赖的系统
	 * @tparam TSubsystemClass
	 * @return
	 */
	template <typename TSubsystemClass>
	TSubsystemClass* InitializeDependency()
	{
		return Cast<TSubsystemClass>(InitializeDependency(TSubsystemClass::StaticClass()));
	}

	/**
	 * @brief 获取子系统
	 * @tparam TSubsystemClass
	 * @return
	 */
	template <typename TSubsystemClass>
	TSubsystemClass* GetSubsystem()
	{
		return Cast<TSubsystemClass>(TypeMapInstance.FindOrAdd(TSubsystemClass::StaticClass()));
	}

private:

	void CollectionAllSubsystemClass(const TArray<FDirectoryPath>& BPSystemPaths);

	UDLClientSubsystemBase* CreateInstance(const UClass* ClassType) const;

	UDLClientSubsystemBase* CreateAndUpdateInfo(UClass* const& Class);

private:

	UPROPERTY()
		TArray<UClass*> SubsystemClasses;

	UPROPERTY()
		TArray<UClass*> AlreadyInitClass;

	UPROPERTY()
		TArray<UClass*> WaitInitClass;

	UPROPERTY()
		TMap<UClass*, UDLClientSubsystemBase*> TypeMapInstance;

	UPROPERTY()
		UDLClientSubsysManager* Manager = nullptr;

	UPROPERTY()
		EDLClientRuntimeStage CreateInsStage = EDLClientRuntimeStage::None;
};


DECLARE_MULTICAST_DELEGATE_OneParam(FOnDLClientSubsysLifeEvent, const UDLClientSubsystemBase*);

/**
 *
 */
UCLASS(Config = Game)
class DLCLIENTSUBSYSTEMCORE_API UDLClientSubsysManager
	: public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:

	/**
	 * @brief 注入客户端的阶段
	 * @param Stage
	 */
	void InjectClientRuntimeStage(EDLClientRuntimeStage Stage);

	/**
	 * @brief 获取子系统
	 * @tparam TSubsystemClass
	 * @return
	 */
	template <typename TSubsystemClass>
	TSubsystemClass* GetSubsystem()
	{
		return SubSysCollection.GetSubsystem<TSubsystemClass>();
	}

	/**
	 * @brief 获取实例
	 * @param WorldContext
	 * @return
	 */
	static UDLClientSubsysManager* Get(const UObject* WorldContext);


	/**
	 * @brief 子系统初始化完成
	 */
	FOnDLClientSubsysLifeEvent OnInitSubsystem;


	/**
	 * @brief 子系统销毁
	 */
	FOnDLClientSubsysLifeEvent OnDestroySubsystem;


private:

	void ProcessAppFrameworkMsg(FGameplayTag Channel, const struct FDLAppFrameworkMsg& Msg);

private:

	UPROPERTY()
		FClientSubsystemCollection SubSysCollection;

	UPROPERTY(Transient)
		TArray<FGameplayMessageListenerHandle> ListenerHandles;

private:


	/**
	 * @brief 在蓝图中实现的子系统的目录，框架会在启动时 扫描这些目录来搜集需要创建的类
	 */
	UPROPERTY(Config, EditAnywhere, meta = (RelativeToGameContentDir, LongPackageName))
		TArray<FDirectoryPath> BPSubsystemLoadDirectoryArray;


	/**
	 * @brief 框架是基于 Msg 驱动的，配置框架需要监听的Msg, 当收到这些 Msg 时会进行尝试创建对应的子系统
	 */
	UPROPERTY(Config, EditAnywhere)
		TMap<FGameplayTag, EDLClientRuntimeStage> ListenerMsgChannelTags;
};

