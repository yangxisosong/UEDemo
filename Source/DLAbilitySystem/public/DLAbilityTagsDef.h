#pragma once
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

namespace EAbilityTagDef
{
	namespace EState
	{
		// 阻止移动
		static const FName BlockMove = TEXT("State.BlockMove");


	};

	namespace EByCallerData
	{
		static const FName CooldownInterval = TEXT("ByCallerData.CooldownInterval");
	};


	// 一些特殊的 Tag
	namespace ESpecial
	{
		static const FName CommitAbility = TEXT("Ability.Special.CommitAbility");
	};

	namespace EAbility
	{
		namespace ECategory
		{
			namespace EActivateType
			{
				static const FName Active = TEXT("Ability.Category.ActivateType.Active");
			}
		}
	};


	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ByCallerData_CutTenacityValue);
};


