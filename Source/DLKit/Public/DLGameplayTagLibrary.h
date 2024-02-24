#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

#include "DLGameplayTagLibrary.generated.h"



USTRUCT(BlueprintType)
struct DLKIT_API FDLGameplayTagRequirements
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTagContainer RequireTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTagContainer IgnoreTags;

public:

	bool RequirementsMet(const FGameplayTagContainer& Container) const
	{
		const bool HasRequired = Container.HasAll(RequireTags);
		const bool HasIgnored = Container.HasAny(IgnoreTags);

		return HasRequired && !HasIgnored;
	}

	bool IsEmpty() const
	{
		return (RequireTags.Num() == 0 && IgnoreTags.Num() == 0);
	}

	FString ToString() const
	{
		FString Str;

		if (RequireTags.Num() > 0)
		{
			Str += FString::Printf(TEXT("require: %s "), *RequireTags.ToStringSimple());
		}
		if (IgnoreTags.Num() > 0)
		{
			Str += FString::Printf(TEXT("ignore: %s "), *IgnoreTags.ToStringSimple());
		}

		return Str;
	}
};



UCLASS()
class DLKIT_API UDLGameplayTagLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure)
		static FGameplayTag FilterTagSingle(const FGameplayTagContainer Tags, const FGameplayTag RootTag)
	{
		return Tags.Filter(FGameplayTagContainer{ RootTag }).First();
	}

	UFUNCTION(BlueprintPure)
		static FGameplayTagContainer FilterTags(const FGameplayTagContainer Tags, const FGameplayTag RootTag)
	{
		return Tags.Filter(FGameplayTagContainer{ RootTag });
	}

	UFUNCTION(BlueprintPure)
		static FString ToString(FGameplayTagContainer Tags, bool bIsSimple = false)
	{
		return bIsSimple ? Tags.ToStringSimple() : Tags.ToString();
	}

	UFUNCTION(BlueprintPure)
		static void GetGamePlayTagArray(TArray<FGameplayTag>& Result, const FGameplayTag& RootTag)
	{
		const auto Tags = UGameplayTagsManager::Get().FindTagNode(RootTag);
		if (Tags.IsValid())
		{
			const auto Children = Tags.Get()->GetChildTagNodes();
			for (const auto& Tag : Children)
			{
				Result.Emplace(Tag->GetCompleteTag());
			}
		}
	}
};
