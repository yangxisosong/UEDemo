
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "DTAbilityAudioRow.generated.h"


class USoundCue;
USTRUCT(BlueprintType)
struct FDTAttackAudioRow
	: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	// 攻击的类型 
	UPROPERTY(EditAnywhere, Category = Tags, Meta = (SupportTags = "Ability.AttackAudio.AttackType"))
		FGameplayTag AttackType;

	// 攻击的力量Tag
	UPROPERTY(EditAnywhere, Category = Tags, Meta = (SupportTags = "Ability.AttackAudio.AttackPower"))
		FGameplayTag AttackPower;

	// 攻击的材质属性
	UPROPERTY(EditAnywhere, Category = Tags, Meta = (SupportTags = "Ability.AttackAudio.AttackMaterialAttr"))
		FGameplayTag AttackMaterialAttr;

	// 防守的材质属性
	UPROPERTY(EditAnywhere, Category = Tags, Meta = (SupportTags = "Ability.AttackAudio.DefenseMaterialAtt"))
		FGameplayTag DefenseMaterialAtt;

	// 音频资产
	UPROPERTY(EditAnywhere, Category = Data)
		TSoftObjectPtr<USoundBase> Sound;

#if WITH_EDITORONLY_DATA
	// 备注
	UPROPERTY(EditAnywhere)
		FString Remark;
#endif

};
