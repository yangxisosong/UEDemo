
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "DLAbilityLog.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "DLAbilitySystemGlobal.h"
#include "DLAbilityTagsDef.h"
#include "GameplayTagsManager.h"
#include "Abilities/GameplayAbility.h"
#include "NativeGameplayTags.h"
#include "DLAbilityTagsDef.h"

struct FDLAbilitySystem : public IModuleInterface
{
	virtual void StartupModule() override
	{
#if WITH_EDITOR
		// register settings
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		if (SettingsModule != nullptr)
		{
			ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Game", "AbilitySystem",
				FText::FromString(TEXT("AbilitySystem")), FText::FromString(TEXT("AbilitySystem")),
				GetMutableDefault<UDLAbilitySystemGlobal>()
			);
		}

		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

		Manager.OnFilterGameplayTagChildren.AddLambda([](const FString& FilterString, TSharedPtr<FGameplayTagNode>& TagNode, bool& OutShouldHide)
		{
			OutShouldHide = false;

			//UE_LOG(LogTemp, Log, TEXT("OnFilterGameplayTagChildren %s  %s"), *FilterString, *TagNode->GetCompleteTagString())


		});

		Manager.OnFilterGameplayTag.AddLambda([](const UGameplayTagsManager::FFilterGameplayTagContext& InContext, bool& OutShouldHide)
		{
			OutShouldHide = false;

			UE_LOG(LogTemp, Verbose, TEXT("OnFilterGameplayTag %s"), *InContext.TagNode->GetCompleteTagString());

			if (!InContext.ReferencingPropertyHandle)
			{
				return;
			}

			TArray<UObject*> OuterObjects;
			InContext.ReferencingPropertyHandle->GetOuterObjects(OuterObjects);
			const auto Options = InContext.ReferencingPropertyHandle->GetMetaData(TEXT("GetOptions"));
			if (!Options.IsEmpty() && OuterObjects.Num() > 0)
			{
				const auto Property = InContext.ReferencingPropertyHandle->GetProperty();
				if (Property)
				{
					UObject* TargetObject = OuterObjects[0];
					const auto OpFunction = TargetObject->FindFunction(*Options);

					if (OpFunction)
					{
						struct FOptionRet
						{
							FGameplayTagContainer Tags;

						} Ret;

						TargetObject->ProcessEvent(OpFunction, &Ret);

						if (Ret.Tags.HasTag(InContext.TagNode->GetCompleteTag()))
						{
							OutShouldHide = false;
						}
						else
						{
							OutShouldHide = true;
						}

						return;

					}
				}
			}


			auto CheckHideProperty = [](const UGameplayTagsManager::FFilterGameplayTagContext& InContext, const FGameplayTag& FilterTag)
			{
				bool OutShouldHide = true;

				// 只有两个集合 有其中一个是另外一个的子集的情况 在不会被过滤
				if (FilterTag.MatchesTag(InContext.TagNode->GetCompleteTag()))
				{
					OutShouldHide = false;
				}
				else
				{
					if (InContext.TagNode->GetCompleteTag().MatchesTag(FilterTag))
					{
						OutShouldHide = false;
					}
				}

				return OutShouldHide;
			};

			bool IsGAProperty = false;

			const auto Property = InContext.ReferencingPropertyHandle->GetProperty();
			if (Property)
			{
				UClass* GAClass = UGameplayAbility::StaticClass();
				UClass* Current = Property->GetOwnerClass();
				while (Current)
				{
					if (Current == GAClass)
					{
						IsGAProperty = true;
						break;
					}

					Current = Current->GetSuperClass();
				}

				if (IsGAProperty)
				{
					for (const auto& Tag : UDLAbilitySystemGlobal::Get().FilterGATagArr)
					{
						OutShouldHide = CheckHideProperty(InContext, Tag);
						if (!OutShouldHide)
						{
							break;
						}
					}
				}
			}

			// 这里可以过滤一些 Tag
			static FName AbilityTagsName(TEXT("AbilityTags"));
			if (IsGAProperty && InContext.ReferencingPropertyHandle->GetProperty() && InContext.ReferencingPropertyHandle->GetProperty()->GetFName() == AbilityTagsName)
			{
				//OutShouldHide = CheckHideProperty(InContext, EAbilityTagDef::Category);
			}

		});
#endif // WITH_EDITOR
	}

	virtual void ShutdownModule() override
	{
#if WITH_EDITOR

		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (SettingsModule != nullptr)
		{
			SettingsModule->UnregisterSettings("Project", "Game", "AbilitySystem");
		}
#endif
	}
};

IMPLEMENT_MODULE(FDLAbilitySystem, DLAbilitySystem);


DEFINE_LOG_CATEGORY(LogDLAbilityTargetSelector);
DEFINE_LOG_CATEGORY(LogDLAbilityChain);
DEFINE_LOG_CATEGORY(LogDLAbility);
DEFINE_LOG_CATEGORY(LogDLGECalculate);
DEFINE_LOG_CATEGORY(LogDLAbilitySystem);
DEFINE_LOG_CATEGORY(LogDLAbilityCue);
DEFINE_LOG_CATEGORY(LogDLAbilityActionEvent);
DEFINE_LOG_CATEGORY(LogDLRoll);



namespace EAbilityTagDef
{
	UE_DEFINE_GAMEPLAY_TAG(ByCallerData_CutTenacityValue, "ByCallerData.CutTenacityValue");
}