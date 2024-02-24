#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
#include "IDLWeapon.generated.h"

struct FWeaponAttackContext;


USTRUCT(BlueprintType)
struct FWeaponAttackArg
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
		FGameplayTag EventTag;
};


USTRUCT(BlueprintType)
struct FWeaponAttachInfo
{
	GENERATED_BODY()

public:

	// 挂载的插槽
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Gameplay.Character.BoneSocket"))
		FGameplayTag SocketName;

	/**
	 * @brief 修正的变换矩阵
	 *
	 *	Socket 的矩阵是不做调整的，需要用这个矩阵 来适配武器的朝向等
	 *
	 *	如果不同的人型骨骼，那么 骨骼 Socket 也需要进行适配的，以保证 所有的 Socket 在世界中是一致的
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform Transform;


	bool operator== (const FGameplayTag& SocketTag)const
	{
		return SocketName == SocketTag;
	}
};



UINTERFACE(BlueprintType, NotBlueprintable, MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UDLWeapon
	: public  UGameplayTagAssetInterface
{
	GENERATED_BODY()
};

class DLWEAPONSYSTEM_API IDLWeapon
	: public IGameplayTagAssetInterface
{
	GENERATED_BODY()
public:

	virtual FName GetWeaponId() const = 0;

	virtual void BeginAttack(const FWeaponAttackArg& Arg) = 0;
	virtual void EndAttack() = 0;
	virtual bool IsAttacking() const = 0;

	virtual bool AttachToCharacter(ACharacter* OwnerCharacter, const FGameplayTag& SocketTag, const FName& SocketName) = 0;
	virtual void DetachToCharacter() = 0;

	virtual TOptional<FWeaponAttachInfo> GetWeaponAttachInfo(const FGameplayTag& SocketTag) = 0;

	virtual AActor* CastToActor() = 0;

	// 获取物理材质
	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual UPhysicalMaterial* GetPhysicalMaterial() const = 0;

	virtual void SetAttackContext(const FWeaponAttackContext& AttackContext) = 0;

	virtual void ClearAttackContext() = 0;

	virtual void SetCurrentBoneSocket(const FGameplayTag& BoneSocket) = 0;

	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual FString ToString() const = 0;

	UFUNCTION(BlueprintCallable, Category = Weapon)
		virtual FGameplayTag GetCurrentBoneSocket() const = 0;
};

