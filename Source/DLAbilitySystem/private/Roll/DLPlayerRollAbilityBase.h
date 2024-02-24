#pragma once

#include "CoreMinimal.h"
#include "DLGameplayAbilityBase.h"
#include "DLAbilityLog.h"
#include "DLPlayerRollAbilityBase.generated.h"

USTRUCT(BlueprintType)
struct FDLRollAnimation
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float StartPos = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName StartSection = NAME_None;
};

USTRUCT(BlueprintType)
struct FDLRollLockUnitAnimation :public FDLRollAnimation
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-180", ClampMax = "180"))
		float YawAngle = 0;
};

UCLASS()
class DLABILITYSYSTEM_API UDLRollAbilityBase : public UDLGameplayAbilityBase
{
	GENERATED_BODY()

public:

	/*锁定翻滚*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Roll)
		TArray<FDLRollLockUnitAnimation> LockUnitRollMontageArray;

	/*非锁定翻滚*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Roll)
		FDLRollAnimation UnLockUnitRollMontage;

	/*锁定后撤*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Roll)
		FDLRollAnimation LockUnitFellBackMontage;

	/*非锁定后撤*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Roll)
		FDLRollAnimation UnLockUnitFellBackMontage;

protected:

	UFUNCTION(BlueprintCallable)
		FDLRollLockUnitAnimation FindAnimByRotation(float Yaw) const
	{
		constexpr float Rang = 45.f / 2.f;

		const float YawClamp = FRotator::NormalizeAxis(Yaw);
		for (const auto& Data : LockUnitRollMontageArray)
		{
			if (FMath::IsNearlyEqual(YawClamp, Data.YawAngle, Rang))
			{
				return Data;
			}
		}

		return LockUnitRollMontageArray.Num() > 0 ? LockUnitRollMontageArray[0] : FDLRollLockUnitAnimation{};
	}

};
