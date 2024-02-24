#include "DLAbilitySubObjectLib.h"

#include "DLGameplayAbilityBase.h"



bool UDLAbilitySubObjectLib::MakeSubObjOutgoingHitDefData(UGameplayAbility* Ability, const TArray<FGameplayTag>& HitDefName,
	TArray<FDLSubObjectOutgoingHitData>& OutHitData)
{
	TArray<FDLSubObjectOutgoingHitData> Ret;

	const auto DLAbility = Cast<UDLGameplayAbilityBase>(Ability);

	if (!DLAbility)
	{
		return false;
	}

	for (const auto& HitDef : HitDefName)
	{
		if(const FDLAbilityHitDefine* Def = DLAbility->FindAbilityHitDefine(HitDef))
		{
			auto& OutgoingHitData = Ret.AddZeroed_GetRef();

			OutgoingHitData.DefineID = Def->DefineId;

			OutgoingHitData.HitTags.AppendTags(Def->CollectAttackTags());

			OutgoingHitData.CalculateAbilityAttackInfoClass = Def->CalculateAttackInfoClass;

			for (const auto& GEDef : Def->OutgoingGEDefineArray)
			{
				auto& GEHandle = OutgoingHitData.OutgoingGESpecHandleArray.AddZeroed_GetRef();
				GEHandle = Ability->MakeOutgoingGameplayEffectSpec(GEDef.GEClass, GEDef.OverrideLevel);
			}
		}
	}

	OutHitData = MoveTemp(Ret);

	return true;
}

AActor* UDLAbilitySubObjectLib::MakeSubObjWithTransform(UGameplayAbility* Ability, const FGameplayTag SubObjDefName,
														const FTransform Transform)
{
	const auto DLAbility = Cast<UDLGameplayAbilityBase>(Ability);

	if (!DLAbility)
	{
		return nullptr;
	}


	UWorld* World = GEngine->GetWorldFromContextObject(Ability, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}


	FDLSubObjectDef ObjectDef;
	if (!DLAbility->FindSubObjectDefByName(SubObjDefName, ObjectDef))
	{
		return nullptr;
	}

	UClass* SubObjClass = ObjectDef.SubObjectClass.LoadSynchronous();
	if (!SubObjClass)
	{
		return nullptr;
	}

	const auto& SpawnConf = ObjectDef.SpawnConfig;

	FDLSubObjectSetupData SetUpData;
	SetUpData.LifeTime = SpawnConf.LifeTime;
	SetUpData.InstanceAbility = Ability;

	if (!UDLAbilitySubObjectLib::MakeSubObjOutgoingHitDefData(Ability, SpawnConf.OutgoingHitDefineIdArray, SetUpData.OutgoingHitData))
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AActor* SubObject = World->SpawnActor<AActor>(SubObjClass, Transform, SpawnParameters);
	if (!IsValid(SubObject))
	{
		return nullptr;
	}

	SubObject->SetOwner(Ability->GetActorInfo().AvatarActor.Get());
	SubObject->SetInstigator(Cast<APawn>(Ability->GetActorInfo().AvatarActor.Get()));

	const auto SetupInterface = Cast<IDLSubObjectSetup>(SubObject);
	if (!SetupInterface)
	{
		SubObject->Destroy();
		return nullptr;
	}

	if (!SetupInterface->SetupSubObject(SetUpData))
	{
		SubObject->Destroy();
		return nullptr;
	}


	return SubObject;
}
