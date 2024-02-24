#pragma once
#include "CoreMinimal.h"
#include "IDLLockableUnit.generated.h"


UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UDLLockableUnit : public UInterface
{
	GENERATED_BODY()
};


USTRUCT(BlueprintType)
struct FLockableUnitConfig
{
	GENERATED_BODY()
public:

	// 玩家的可锁定距离
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 PlayerCanLockDistanceSquared = 0;

	// 玩家的可锁定最大距离, 玩家超过这个距离，就会解除锁定
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 PlayerCanLockMaxDistanceSquared = 0;

	// 被锁定的优先级,  值越大 越优先被锁定
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 BeLockedPriority = 0;

	// 玩家的自由锁定最大距离
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 PlayerCanFreeLockMaxDistanceSquared = 0;

	// 玩家的固定锁定的点
	UPROPERTY(EditAnywhere, Category = BaseValue, BlueprintReadOnly)
		int32 FixedLockPointIndex = 0;
};

class DLGAMEPLAYTYPES_API IDLLockableUnit : public IInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
		virtual class UDLLockPointComponent* GetLockPointComponent(int32 Index) = 0;

	UFUNCTION(BlueprintCallable)
		virtual int32 GetDefaultLockPointComponent() = 0;

	UFUNCTION(BlueprintCallable)
		virtual const TArray<UDLLockPointComponent*>& GetAllLockPoint() const = 0;

	UFUNCTION(BlueprintCallable)
		virtual const FLockableUnitConfig& GetLockableUnitConfig() const = 0;
};


UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class DLGAMEPLAYTYPES_API UDLLockPointComponent : public USceneComponent
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
		int32 Index;
	
	UPROPERTY(EditDefaultsOnly)
		float LockArmLengthOffset = 0;
public:

	UFUNCTION(BlueprintCallable)
	TScriptInterface<IDLLockableUnit> GetLockableUnit() const
	{
		return GetOwner();
	}

};