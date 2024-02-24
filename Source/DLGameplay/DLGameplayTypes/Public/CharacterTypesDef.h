#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "CharacterTypesDef.generated.h"


USTRUCT(BlueprintType)
struct FCharacterInfoBase
{
	GENERATED_BODY()

public:
	// ID  参考策划ID规则  #TODO 规则链接地址
	UPROPERTY(EditAnywhere, Category = BaseInfoValue, BlueprintReadOnly)
		FName Id;

	// 名字  注意这里是本地化的 
	UPROPERTY(EditAnywhere, Category = BaseInfoValue, BlueprintReadOnly)
		FText Name;

	// 角色的各种标签
	UPROPERTY(EditAnywhere, Category = BaseInfoValue, BlueprintReadOnly, Meta = (Categories = "Gameplay.Character.BoneType,Gameplay.Unit.RoleType"))
		FGameplayTagContainer CharacterTags;

	// 角色关联的资产
	UPROPERTY(EditAnywhere, Category = BaseInfoValue, BlueprintReadOnly)
		FPrimaryAssetId CharacterAssetId;
};


// 部位的枚举  Tag  Gameplay.Character.Parts
UENUM(BlueprintType)
enum class EDLCharacterParts
	: uint8
{
	None,
	Head,
	LeftArm,
	RightArm,
	Body,
	LeftLeg,
	RightLeg,
};

