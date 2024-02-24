#include "DLAbilityValidator.h"
#include "Animation\AnimNotifies\AnimNotifyState.h"
#include "DLGameplayAbilityBase.h"
#include "GameplayTagsManager.h"
#include "AbilityEventAction/DLAbilityAction.h"
#include "AbilityEventAction/DLAbilityEventBase.h"
#include "Kismet2/CompilerResultsLog.h"
#include "Kismet2/KismetEditorUtilities.h"


bool UDLAbilityValidator::CanValidateAsset_Implementation(UObject* InAsset) const
{
	if (const UBlueprint* BP = Cast<UBlueprint>(InAsset))
	{
		if (BP->ParentClass->IsChildOf<UDLGameplayAbilityBase>())
		{
			return true;
		}
	}

	return false;
}

using FValidatorPropertyCall = TFunction<void(FProperty* Property, const FGameplayTagContainer& Tag)>;

void ValidatorTagInStruct(UStruct* Struct, void* Data, const FValidatorPropertyCall& Call);

void ValidatorTagInProperty(FProperty* Property, void* Data, const FValidatorPropertyCall& Call)
{
	void* InnerData = Property->ContainerPtrToValuePtr<FGameplayTag>(Data);

	if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		if (StructProperty->Struct == FGameplayTag::StaticStruct())
		{
			const FGameplayTag* Tag = static_cast<FGameplayTag*>(InnerData);
			if (Tag)
			{
				Call(Property, FGameplayTagContainer{ *Tag });
			}
		}
		else if (StructProperty->Struct == FGameplayTagContainer::StaticStruct())
		{
			const FGameplayTagContainer* Tags = static_cast<FGameplayTagContainer*>(InnerData);
			if (Tags)
			{
				Call(Property, *Tags);
			}
		}
		else
		{
			ValidatorTagInStruct(StructProperty->Struct, InnerData, Call);
		}
	}
	else if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{
		FProperty* Inner = ArrayProperty->Inner;

		FScriptArrayHelper Helper(ArrayProperty, InnerData);

		for (int32 Index = 0; Index < Helper.Num(); Index++)
		{
			ValidatorTagInProperty(Inner, Helper.GetRawPtr(Index), Call);
		}
	}
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ValidatorTagInStruct(UStruct* Struct, void* Data, const FValidatorPropertyCall& Call)
{
	for (TFieldIterator<FProperty>It(Struct); It; ++It)
	{
		ValidatorTagInProperty(*It, Data, Call);
	}
};

EDataValidationResult UDLAbilityValidator::ValidateLoadedAsset_Implementation(
	UObject* InAsset,
	TArray<FText>& ValidationErrors)
{
	const auto& GameplayTagManager = UGameplayTagsManager::Get();


	EDataValidationResult Ret = EDataValidationResult::Valid;

	auto Validation = [&ValidationErrors, this, InAsset, &Ret](const bool Ensure, const FString& Msg)
	{
		if (!Ensure)
		{
			this->AssetFails(InAsset, FText::FromString(Msg), ValidationErrors);
			Ret = CombineDataValidationResults(Ret, EDataValidationResult::Invalid);
		}
	};

	auto ValidationWarring = [&ValidationErrors, this, InAsset, &Ret](const bool Ensure, const FString& Msg)
	{
		if (!Ensure)
		{
			this->AssetWarning(InAsset, FText::FromString(Msg));
		}
	};

	UBlueprint* BP = Cast<UBlueprint>(InAsset);

	FCompilerResultsLog RetLogCompiler;
	//RetLogCompiler.Register();
	FKismetEditorUtilities::CompileBlueprint(BP, EBlueprintCompileOptions::SkipGarbageCollection, &RetLogCompiler);
	if ((RetLogCompiler.NumErrors + RetLogCompiler.NumWarnings) > 0)
	{
		Validation(false, TEXT("CompileBlueprint Error "));
	}
	//RetLogCompiler.Unregister();

	UDLGameplayAbilityBase* Ability = BP->GeneratedClass->GetDefaultObject<UDLGameplayAbilityBase>();
	if (!Ability)
	{
		return EDataValidationResult::NotValidated;
	}

	//先检查定义是否完整
	const auto& HitDefines = Ability->GetHitDefines();
	const auto& UnitSelectDefines = Ability->GetUnitSelectDefines();
	const auto& SubObjectDefines = Ability->GetSubObjectDefines();
	const auto& VirtualCameraDefines = Ability->GetVirtualCameraDefines();

	int32 Index = 0;
	for (const auto& Def : HitDefines)
	{
		Validation(Def.CalculateAttackInfoClass != nullptr, FString::Printf(TEXT("HitDefines[%d]->CalculateAttackInfoClass Is None"), Index));
		Validation(Def.DefineId.IsValid(), FString::Printf(TEXT("HitDefines[%d]->DefineId Is Invalid"), Index));

		Index++;
	}

	Index = 0;
	for (const auto& Def : UnitSelectDefines)
	{
		if (Def.UnitSelectType == EDLAbilityUnitSelectType::UnitSelectorInstance)
		{
			Validation(Def.SelectorInsPrototype != nullptr, FString::Printf(TEXT("UnitSelectDefines[%d]->SelectorInsPrototype Is None"), Index));
		}

		Validation(Def.DefineId.IsValid(), FString::Printf(TEXT("UnitSelectDefines[%d]->DefineId Is Invalid"), Index));

		Index++;
	}


	Index = 0;
	for (const auto& Def : SubObjectDefines)
	{

		Validation(Def.SubObjectClass != nullptr, FString::Printf(TEXT("SubObjectDefines[%d]->SubObjectClass Is None"), Index));

		const auto& HitDefId = Ability->GetHitDefineIds();

		for (auto ID : Def.SpawnConfig.OutgoingHitDefineIdArray)
		{
			Validation(HitDefId.HasTag(ID), FString::Printf(TEXT("SubObjectDefines[%d]->SpawnConfig.OutgoingHitDefineIdArray %s Is Invalid"), Index, *ID.ToString()));
		}

		Validation(Def.SpawnConfig.LifeTime > 0.f, FString::Printf(TEXT("SubObjectDefines[%d]->SpawnConfig.LifeTime must > 0"), Index));

		Index++;
	}


	Index = 0;
	for (const auto& Def : VirtualCameraDefines)
	{
		Validation(Def.DefineId.IsValid(), FString::Printf(TEXT("VirtualCameraDefines[%d]->DefineId Is Invalid"), Index));

		Index++;
	}

	FGameplayTagContainer WeaponUnitHitEventList;


	auto CheckGameplayTag = [&](const UObject* Target, const FProperty* P, const FGameplayTagContainer& Tags)
	{
		if (P->HasMetaData(TEXT("ValidationTagVaild")))
		{
			Validation(!Tags.IsEmpty(), FString::Printf(
				TEXT("%s %s Is None"),
				*Target->GetFName().ToString(),
				*P->GetFName().ToString()
			));

			for (auto It = Tags.CreateConstIterator(); It; ++It)
			{
				Validation(It->IsValid(), FString::Printf(
					TEXT("%s %s Is None"),
					*Target->GetFName().ToString(),
					*P->GetFName().ToString()
				));
			}
		}

		for (auto It = Tags.CreateConstIterator(); It; ++It)
		{
			Validation(!!GameplayTagManager.FindTagNode((*It)),
				FString::Printf(
					TEXT("%s %s %s Is Invaild"),
					*Target->GetFName().ToString(),
					*P->GetFName().ToString(),
					*It->ToString()
				)
			);
		}
	};


	// 检查动画的配置
	const auto& AnimationInfo = Ability->GetAnimationInfo();

	ValidationWarring(AnimationInfo.AnimMontage.LoadSynchronous() != nullptr, TEXT("AnimationInfo.AnimMontage Is None"));
	if (const auto Montage = AnimationInfo.AnimMontage.LoadSynchronous())
	{
		// 检查Section有效
		if (AnimationInfo.MontageSection.IsValid())
		{
			Validation(Montage->GetSectionIndex(AnimationInfo.MontageSection) != INDEX_NONE, TEXT("AnimationInfo.MontageSection Is Invalid"));
		}

		// 检查通知
		for (const auto& Ntf : Montage->Notifies)
		{
			UObject* Object = nullptr;
			if (Ntf.Notify)
			{
				Object = Ntf.Notify;
			}

			if (Ntf.NotifyStateClass)
			{
				Object = Ntf.NotifyStateClass;
			}

			if (Object)
			{
				// 检查 Tag 的合法
				ValidatorTagInStruct(Object->GetClass(), Object, [&](const FProperty* P, const FGameplayTagContainer& Tag)
				{
					CheckGameplayTag(Object, P, Tag);
				});

				// 检查是否匹配
				static FName NameDLAnimNS_WeaponAttack(TEXT("ANS_WeaponAttack_C"));
				if (Object->GetClass()->GetFName() == NameDLAnimNS_WeaponAttack)
				{
					const auto DataPro = Object->GetClass()->FindPropertyByName(TEXT("WeaponAttackInfo"));
					const FWeaponAttackNtfInfo* Data = DataPro->ContainerPtrToValuePtr<FWeaponAttackNtfInfo>(Object);
					if (Data)
					{
						// Check UnitSelectDefineId
						Validation(Ability->GetUnitSelectDefineIds().HasTag(Data->UnitSelectDefineId),
									FString::Printf(TEXT("%s WeaponAttackInfo UnitSelectDefineId Is Invaild"),
										*Object->GetFName().ToString())
						);

						if (Data->EventTag.IsValid())
						{
							WeaponUnitHitEventList.AddTag(Data->EventTag);
						}
					}
				}

				//检测通知的类型信息
				static FName Name_AbilityStageChange(TEXT("AN_AbilityStageChange_C"));
				static FName Name_CustomEvent(TEXT("AN_CustomEvent_C"));
				static FName Name_CustomState(TEXT("ANS_CustomState_C"));
				auto ObjectName = Object->GetClass()->GetFName();
				if(ObjectName== Name_AbilityStageChange ||
					ObjectName == Name_CustomEvent ||
					ObjectName == Name_CustomState)
				{
					const bool IsOk = Ntf.MontageTickType == EMontageNotifyTickType::BranchingPoint;
					Validation(IsOk, FString::Printf(TEXT("%s Event Type is Error! Type Must Be BranchingPoint"),
						*Montage->GetFName().ToString()));
				}
			}
		}
	}

	Index = 0;

	TArray<FString> TempStringOut;

	// 检查Event配置对不对

	FGameplayTagContainer AbilityHitEventTags;
	const auto& HitEventArray = Ability->FindAbilityEvent(UDLAbilityEventWeaponHitActor::StaticClass());
	for (const auto Event : HitEventArray)
	{
		AbilityHitEventTags.AppendTags(Cast<UDLAbilityEventWeaponHitActor>(Event)->HitEventTags);
	}

	// 检查 动画的 EventTag 与 事件的 Tag 只能从整体上进行检查，因为有多个HitUnitEvent 和 动画事件
	Validation(AbilityHitEventTags.HasAll(WeaponUnitHitEventList), FString::Printf(TEXT("HitUnitEvent Tag Not Match Montage Weapon Event Tag")));

	const auto& EventList = Ability->GetEventActionArray();
	for (const auto& Event : EventList)
	{
		if (!Event->Validation(TempStringOut))
		{
			Ret = CombineDataValidationResults(Ret, EDataValidationResult::Invalid);
		}

		if (TempStringOut.Num() > 0)
		{
			for (const auto& Msg : TempStringOut)
			{
				this->AssetFails(InAsset,
								FText::FromString(FString::Printf(TEXT("EventActionArray[%d]%s"), Index, *Msg)),
								ValidationErrors
				);
			}

			TempStringOut.Empty();
		}

		int32 ActionIndex = 0;
		for (const auto& Action : Event->ActionList)
		{
			ValidationWarring(!!Action, TEXT("has None Action, Please remove the "));

			if (!Action)
			{
				continue;
			}

			if (!Action->Validation(TempStringOut))
			{
				Ret = CombineDataValidationResults(Ret, EDataValidationResult::Invalid);
			}

			if (TempStringOut.Num() > 0)
			{
				for (const auto& Msg : TempStringOut)
				{
					this->AssetFails(InAsset,
									FText::FromString(FString::Printf(TEXT("EventActionArray[%d] ActionList[%d]%s"), Index, ActionIndex, *Msg)),
									ValidationErrors
					);
				}

				TempStringOut.Empty();
			}

			ActionIndex++;
		}

		Index++;
	}

	// 检查所有的 Tag 是否合法


	UClass* Class = Ability->GetClass();
	ValidatorTagInStruct(Class, Ability, [&](const FProperty* P, const FGameplayTagContainer& Tag)
	{
		CheckGameplayTag(Ability, P, Tag);
	});

	if (Ret == EDataValidationResult::Valid)
	{
		this->AssetPasses(InAsset);
	}

	return Ret;
}
