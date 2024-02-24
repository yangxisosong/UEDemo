
#include "GameplayCoreLib/DLGameplayCoreLib.h"

#include "DLGameplayCoreSetting.h"
#include "GameplayTags.h"



bool UDLGameplayCoreLib::CheckUnitCampRelationship(AActor* UnitA, AActor* UnitB, FGameplayTag& OutRelationship, bool DebugEnsure)
{
	const auto ATags = GetUnitRoleTag(UnitA, DebugEnsure);
	const auto BTags = GetUnitRoleTag(UnitB, DebugEnsure);

	static TArray<FGameplayTag> RoleTagArr{
		FGameplayTag::RequestGameplayTag(TEXT("Gameplay.Unit.RoleType.Player")),
		FGameplayTag::RequestGameplayTag(TEXT("Gameplay.Unit.RoleType.Monster"))
	};

	static TArray<FGameplayTag> RelationshipTagArr{
		FGameplayTag::RequestGameplayTag(TEXT("Gameplay.Unit.CampRelationship.Friendly")),
		FGameplayTag::RequestGameplayTag(TEXT("Gameplay.Unit.CampRelationship.Hostilityr")),
		FGameplayTag::RequestGameplayTag(TEXT("Gameplay.Unit.CampRelationship.Neutrality"))
	};

	static TArray<TArray<int32>> Relationship{
		{0, 1},
		{1, 0},
	};

	const int32 IndexA = RoleTagArr.Find(ATags);
	const int32 IndexB = RoleTagArr.Find(BTags);

	if (IndexA == INDEX_NONE || IndexB == INDEX_NONE)
	{
		return false;
	}

	if (Relationship.IsValidIndex(IndexA) && Relationship.IsValidIndex(IndexB))
	{
		const auto Index = Relationship[IndexA][IndexB];
		if (RelationshipTagArr.IsValidIndex(Index))
		{
			OutRelationship = RelationshipTagArr[Index];

			return true;
		}
	}

	return false;
}

FGameplayTag UDLGameplayCoreLib::GetUnitRoleTag(AActor* Unit, bool DebugEnsure)
{
	FGameplayTag Ret;

	const IGameplayTagAssetInterface* A = Cast<IGameplayTagAssetInterface>(Unit);

	if (DebugEnsure)
	{
		ensureAlwaysMsgf(A, TEXT("单位 %s 必须实现  IGameplayTagAssetInterface"), *GetNameSafe(Unit));
	}

	if (A)
	{
		FGameplayTagContainer ATags;
		A->GetOwnedGameplayTags(ATags);

		static FGameplayTagContainer UnitRoleTag(UDLGameplayCoreSetting::Get()->UnitRoleRootTag);

		Ret = ATags.Filter(UnitRoleTag).First();
	}

	return Ret;
}
